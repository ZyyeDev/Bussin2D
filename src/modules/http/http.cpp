#include "http.h"
#include <iostream>

static size_t writeFunction(void* ptr, size_t size, size_t nmemb, std::string* data) {
    data->append((char*)ptr, size * nmemb);
    return size * nmemb;
}

http& http::instance() {
    static http inst;
    return inst;
}

http::http() : running(true) {
    int threadCount = (int)std::max(2u, std::thread::hardware_concurrency());
    for (int i = 0; i < threadCount; i++) {
        workers.emplace_back(&http::workerLoop, this);
    }
}

http::~http() {
    shutdown();
}

void http::shutdown() {
    running = false;
    for (auto& t : workers) {
        if (t.joinable()) t.join();
    }
}

void http::update() {
    std::queue<std::pair<std::function<void(Response)>, Response>> toFlush;
    {
        std::lock_guard<std::mutex> lock(completedMutex);
        std::swap(toFlush, completed);
    }
    while (!toFlush.empty()) {
        auto& front = toFlush.front();
        front.first(front.second);
        toFlush.pop();
    }
}

void http::workerLoop() {
    while (running) {
        Request req;
        bool hasWork = false;
        {
            std::lock_guard<std::mutex> lock(pendingMutex);
            if (!pending.empty()) {
                req = std::move(pending.front());
                pending.pop();
                hasWork = true;
            }
        }
        if (hasWork) {
            Response res = perform(req);
            std::lock_guard<std::mutex> lock(completedMutex);
            completed.push({ req.callback, res });
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

http::Response http::perform(const Request& req) {
    Response response{};
    auto curl = curl_easy_init();
    if (!curl) {
        response.success = false;
        return response;
    }

    curl_slist* curlHeaders = nullptr;
    for (const auto& [key, value] : req.headers) {
        std::string header = key + ": " + value;
        curlHeaders = curl_slist_append(curlHeaders, header.c_str());
    }

    curl_easy_setopt(curl, CURLOPT_URL, req.url.c_str());
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, req.timeout);

    if (curlHeaders) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curlHeaders);
    }

    if (req.method == "POST") {
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, req.body.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)req.body.size());
    } else if (req.method == "PUT") {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, req.body.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)req.body.size());
    } else if (req.method == "DELETE") {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    }

    std::string responseBody;
    std::string responseHeader;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &responseHeader);

    CURLcode result = curl_easy_perform(curl);

    long responseCode = 0;
    double elapsed = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);

    if (curlHeaders) curl_slist_free_all(curlHeaders);
    curl_easy_cleanup(curl);

    response.body = responseBody;
    response.header = responseHeader;
    response.response_code = (int)responseCode;
    response.elapsed = elapsed;
    response.success = (result == CURLE_OK) && (responseCode >= 200 && responseCode < 300);

    return response;
}

void http::get(const std::string& url, std::function<void(Response)> callback, const std::string& userAgent) {
    Request req;
    req.method = "GET";
    req.url = url;
    req.callback = callback;
    req.headers["User-Agent"] = userAgent;
    request(req);
}

void http::post(const std::string& url, const std::string& body, const std::map<std::string, std::string>& headers, std::function<void(Response)> callback) {
    Request req;
    req.method = "POST";
    req.url = url;
    req.body = body;
    req.headers = headers;
    req.callback = callback;
    request(req);
}

void http::put(const std::string& url, const std::string& body, const std::map<std::string, std::string>& headers, std::function<void(Response)> callback) {
    Request req;
    req.method = "PUT";
    req.url = url;
    req.body = body;
    req.headers = headers;
    req.callback = callback;
    request(req);
}

void http::del(const std::string& url, std::function<void(Response)> callback) {
    Request req;
    req.method = "DELETE";
    req.url = url;
    req.callback = callback;
    request(req);
}

void http::request(Request req) {
    std::lock_guard<std::mutex> lock(pendingMutex);
    pending.push(std::move(req));
}