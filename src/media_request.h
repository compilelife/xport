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

    /**
     * @return http URL中的path部分。如对于http://127.0.0.1:8000/media/video/1.mp3，返回[media, video, 1.mp3]
     */
    std::vector<std::string> segments();
    
    /**
     * @return http URL中的参数部分，如对于http://127.0.0.1:8000/media?path=1.mp3&auth=123，返回[{path:1},{auth:123}]
     * @see hasParam, getParamValue, getParamValueCount
     */
    std::multimap<std::string, std::string> params();

    /**
     * @return 是否有key指定的参数
     */
    bool hasParam(const char *key);

    /**
     * 读取参数值
     * \param id 当同一个key有对应多个不同的value时，用该参数（索引）枚举各个value
     * @return 返回key对应的参数值。如果hasParam(key)==false，则返回空字符串("")
     */
    std::string getParamValue(const char *key, size_t id = 0);

    /**
     * @return 获取key对应的参数值个数。一般情况为1（有指定），或0（未指定）.
     */
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