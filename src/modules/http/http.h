#pragma once

#include <string>
#include <map>
#include <functional>
#include <thread>
#include <mutex>
#include <queue>
#include <vector>
#include <atomic>

#include "curl/curl.h"

class http {
public:
    struct Response {
        int response_code;
        std::string header;
        std::string body;
        bool success;
        double elapsed;
    };

    struct Request {
        std::string method = "GET";
        std::string url;
        std::string body;
        std::map<std::string, std::string> headers;
        long timeout = 30;
        std::function<void(Response)> callback;
    };

    static http& instance();

    void update();
    void shutdown();

    void get(const std::string& url, std::function<void(Response)> callback, const std::string& userAgent = "curl/7.42.0");
    void post(const std::string& url, const std::string& body, const std::map<std::string, std::string>& headers, std::function<void(Response)> callback);
    void put(const std::string& url, const std::string& body, const std::map<std::string, std::string>& headers, std::function<void(Response)> callback);
    void del(const std::string& url, std::function<void(Response)> callback);
    void request(Request req);

private:
    http();
    ~http();

    void workerLoop();
    Response perform(const Request& req);

    std::vector<std::thread> workers;
    std::queue<Request> pending;
    std::mutex pendingMutex;

    std::queue<std::pair<std::function<void(Response)>, Response>> completed;
    std::mutex completedMutex;

    std::atomic<bool> running;
};
