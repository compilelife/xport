#include "config.h"

#ifndef _GLIBCXX_USE_C99
#include <stdio.h>
#include <stdlib.h>

#include <cinttypes>
std::string to_string(int32_t val){
    auto len = 4*sizeof(int32_t);
    char buf[len];
    snprintf(buf, len, "%d", val);
    return buf;
}
std::string to_string(int64_t val){
    auto len = 4*sizeof(int64_t);
    char buf[len];
    snprintf(buf, len, "%" PRId64, val);
    return buf;
}

std::string to_string(uint32_t val){
    auto len = 4*sizeof(uint32_t);
    char buf[len];
    snprintf(buf, len, "%u", val);
    return buf;
}
std::string to_string(uint64_t val){
    auto len = 4*sizeof(uint64_t);
    char buf[len];
    snprintf(buf, len, "%" PRIu64, val);
    return buf;
}

int stoi(const std::string& s, std::size_t* pos, int base){
    char* end = nullptr;
    auto ret = strtol(s.c_str(), &end, base);
    if (pos){
        *pos = end - s.c_str();
    }
    return ret;
}
#endif