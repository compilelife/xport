#ifndef COMMON_H
#define COMMON_H

#include "../src/xport.h"
#include <memory>
#include <gtest/gtest.h>

using namespace std;
using namespace xport;

class AStreamMedia : public StreamMedia{
public:
    virtual bool open(){return true;}
    virtual void close(){};
    virtual std::string read(){return "";}
protected:
    virtual bool reset(){return true;}
};

class ASeekableMedia: public SeekableMedia{
public:
    virtual bool open(){return true;}
    virtual void close(){};
    virtual std::string read(){return "";}
    virtual int64_t seek(int64_t offset){return offset;}
    virtual int64_t size(){return 1000L;}
};

#endif