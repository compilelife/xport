#ifndef READ_MEDIA_H
#define READ_MEDIA_H

#include "imedia.h"
#include <memory>
#include <mutex>

namespace xport{
    
class IReader{
public:
    virtual ~IReader(){}
    virtual std::string read() = 0;
    virtual int64_t from() = 0;
    virtual int64_t to() = 0;
};

class ReadMedia : public IMedia{
private:
    std::mutex mReadLock;
    std::shared_ptr<IMedia> mMedia;
    int mId;
    bool mClosed;
    bool mOpened;

private:
    class Reader : public IReader{
    private:
        bool mAlive;
        ReadMedia* mMedia;
        const uint64_t mFrom;
        const uint64_t mTo;
    public:
        Reader(ReadMedia* media, int64_t from, int64_t to)
            :mAlive(true), mMedia(media), mFrom(from), mTo(to){
        }
        void markDead() {mAlive=false;}
    public:
        virtual std::string read();
        virtual int64_t from() {return mFrom;}
        virtual int64_t to() {return mTo;}
    };
    std::weak_ptr<Reader> mLastReader;

public:
    ReadMedia(const std::shared_ptr<IMedia>& media);
    ~ReadMedia();
    std::shared_ptr<IReader> createReader(int64_t from, int64_t to);
    bool isIdle();
    int id(){return mId;}
    void setId(int id){mId = id;}

//继承自IMedia
public:
    virtual bool open();
    virtual void close();
    virtual std::string read(){return mMedia->read();}
    virtual int64_t seek(int64_t offset){return mMedia->seek(offset);}
    virtual int64_t size(){return mMedia->size();}

public:
    virtual bool seekable(){return mMedia->seekable();}
    virtual int idleTimeout(){return mMedia->idleTimeout();}
    virtual std::string mimeType(){return mMedia->mimeType();};
};


}


#endif