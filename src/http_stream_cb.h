#ifndef HTTP_STREAM_CB
#define HTTP_STREAM_CB

#include "read_media.h"

namespace xport
{
    
class HttpStreamCB{
private:
    std::shared_ptr<IReader> mReader;
    const uint64_t mSize;

public:
    HttpStreamCB(const std::shared_ptr<IReader>& reader);
    std::string read(uint64_t offset);
};

} // namespace xport



#endif