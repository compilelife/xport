#ifndef LOG_H
#define LOG_H

namespace xport
{
    void log(int level, const char* fmt, ...);
} // namespace xport

#define logi(fmt, args...) log(0, fmt, ##args)
#define logw(fmt, args...) log(1, fmt, ##args)
#define loge(fmt, args...) log(2, fmt, ##args)

#include <cinttypes>

#endif