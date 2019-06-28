#include "log.h"
#include "aloop.h"
#include <functional>
#include <stdarg.h>
using namespace std;

namespace xport{

using PrintFunc = function<void(int level, const char* msg)>;
static PrintFunc g_doPrint = [](int level, const char* msg){
    const char* slevel[3] = {"INFO", "WARN", "ERR"};
    printf("[%s] %s", slevel[level], msg);
};

void setPrintFunc(void(*printFunc)(int level, const char* msg)) {
    g_doPrint = printFunc;
    aloop::setPrintFunc(printFunc);
}

void log(int level, const char* fmt, ...) {
    if (!g_doPrint)
        return;

    const int size = 1024;
    char buf[size] = {0};

    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, size, fmt, ap);
    va_end(ap);

    strcat(buf, "\n");
    g_doPrint(level, buf);
}

}
