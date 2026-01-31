#pragma once

#include <string>
#include <lua.hpp>

class http{
    struct Response{
        long status;
        std::string body;
        bool success;
    };

    Response get(const std::string url);
};
