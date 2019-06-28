#ifndef MEDIA_REQUEST_H
#define MEDIA_REQUEST_H

#include <vector>
#include <string>
#include <map>

namespace xport
{

class MediaRequestImpl;
class MediaRequest {
public:
    MediaRequest(MediaRequestImpl* impl);
    ~MediaRequest();

    std::vector<std::string> segments();
    std::multimap<std::string, std::string> params();

    bool hasParam(const char *key);
    std::string getParamValue(const char *key, size_t id = 0);
    size_t getParamValueCount(const char *key);

private:
    friend class MediaRequestImpl;
    MediaRequestImpl* mImpl;

    //DISALLOW_EVIL_CONSTRUCTORS
    MediaRequest(const MediaRequest &);
    MediaRequest &operator=(const MediaRequest &);
};


} // namespace xport


#endif