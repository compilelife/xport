#ifndef IMEDIA_H
#define IMEDIA_H

#include <string>
#include <stdint.h>

namespace xport
{

class IMedia{
public:
    virtual ~IMedia(){}

public:
    virtual bool open() = 0;
    virtual void close() = 0;
    virtual std::string read() = 0;
    virtual int64_t seek(int64_t offset) = 0;
    virtual int64_t size() = 0;

public:
    virtual bool seekable() = 0;
    virtual int idleTimeout(){return 3000;}
    virtual std::string mimeType(){return "video/mpeg";}
};

class SeekableMedia: public IMedia{
public:
    virtual bool seekable() {return true;}
};

class StreamMedia: public IMedia{
public:
    virtual ~StreamMedia(){}
protected:
    virtual bool reset() = 0;
public:
    int64_t seek(int64_t offset) {return reset() ? 0 : -1;}
    virtual int64_t size() {return -1;}
public:
    virtual bool seekable() {return false;}
};

} // namespace xport


#endif