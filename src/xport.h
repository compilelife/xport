#ifndef XPORT_H
#define XPORT_H

namespace xport
{
    enum{
        LOG_LEVEL_INFO,
        LOG_LEVEL_WARN,
        LOG_LEVEL_ERR
    };

    void setPrintFunc(void(*printFunc)(int level, const char* msg));
} // namespace xport


#include "http_server.h"
#include "imedia.h"
#include "imedia_creator.h"
#include "media_request.h"
#include "media_factory.h"

#endif