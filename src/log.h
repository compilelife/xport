#ifndef LOG_H
#define LOG_H

namespace xport
{
    void log(int level, const char* fmt, ...);
} // namespace xport

#define logti(tag, fmt, args...) log(0, "[%s] " fmt, tag, ##args)
#define logtw(tag, fmt, args...) log(1, "[%s] " fmt, tag, ##args)
#define logte(tag, fmt, args...) log(2, "[%s] " fmt, tag, ##args)

#define logi(fmt, args...) logti(TAG, fmt, ##args)
#define logw(fmt, args...) logtw(TAG, fmt, ##args)
#define loge(fmt, args...) logte(TAG, fmt, ##args)


#include <cinttypes>

#endif