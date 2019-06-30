#ifndef IMEDIA_H
#define IMEDIA_H

#include <string>
#include <stdint.h>

namespace xport
{

/**
 * \brief 代表一种媒体资源
 * 
 * 用户需要继承该类来定义自己的媒体资源
 * “媒体资源”主要要定义的行为有open/close/read/seek
 * 
 * 媒体资源还可以按是否可以seek细分为两类：SeekableMedia和StreamMedia
 * 用户可以按照自定义媒体资源的特性，直接继承SeekableMdia或StreamMedia来定义。
 */
class IMedia{
public:
    virtual ~IMedia(){}

public:
    /**
     * 打开媒体资源
     * \return 是否打开成功。如果打开失败，框架会确保不再调用IMedia的其他接口
     */
    virtual bool open() = 0;

    /**
     * 关闭媒体资源。媒体资源关闭后，框架会确保不再调用IMedia的其他接口
     */
    virtual void close() = 0;

    /**
     * 读取一段媒体数据。用户可以自行决定一次read应返回的数据量大小
     * \return 装载在string里的媒体数据（string是可以存放任意二进制数据的，可以参考demo如何返回数据）。返回空字符串("")表示数据已读取完毕。
     */
    virtual std::string read() = 0;

    /**
     * \brief 请求跳转到媒体的offset位置开始读取数据
     * 
     * seek一般发生在SeekableMedia上(seekable()==true)
     * 对于StreamMedia(seekable()==false)，如果客户端发送了seek请求，或者断开重连时也会调用该函数
     * 
     * \param offset 偏移位置
     * \return seek后的媒体位置。一般情况下，需要返回offset。如果无法处理seek请求，返回-1。对于StreamMedia，可以返回0表示从头开始播放（相应地之后的read应该返回从0开始的数据）
     */
    virtual int64_t seek(int64_t offset) = 0;

    /**
     * \return 返回媒体资源的总长度。如果是是StreamMedia，返回-1。 
     */
    virtual int64_t size() = 0;

public:
    /**
     * \return 告知本资源是否支持seek
     */
    virtual bool seekable() = 0;
    
    /**
     * 当没有客户度读取媒体数据时，隔多久后（单位ms）由框架自动关闭本媒体资源。
     * 
     * \note 小于0表示一旦客户端关闭http连接，立即关闭本媒体资源；等于0表示尽快关闭
     */
    virtual int idleTimeout(){return 3000;}

    /**
     * 通过http返回的数据类型，一般情况下不需要修改
     */
    virtual std::string mimeType(){return "video/mpeg";}
};

/**
 * \brief 代表可以seek的资源
 */
class SeekableMedia: public IMedia{
public:
    virtual bool seekable() {return true;}
};

/**
 * \brief 代表不可seek的资源
 */
class StreamMedia: public IMedia{
public:
    virtual ~StreamMedia(){}
protected:
    /**
     * 当客户端请求seek，或者重连时，触发该函数。
     * 
     * @return true 重置为从头开始播放（或直接忽略该请求）；false 发生错误
     */
    virtual bool reset() = 0;
public:
    int64_t seek(int64_t offset) {return reset() ? 0 : -1;}
    virtual int64_t size() {return -1;}
public:
    virtual bool seekable() {return false;}
};

} // namespace xport


#endif