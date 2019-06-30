#ifndef READ_MEDIA_H
#define READ_MEDIA_H

#include "imedia.h"
#include <memory>
#include <mutex>
#include "aloop.h"

namespace xport{
    
class IReader{
public:
    virtual ~IReader(){}
    virtual std::string read() = 0;
    virtual int64_t from() = 0;
    virtual int64_t to() = 0;
    virtual int mediaId() = 0;
};

class ReadMedia : public IMedia, public std::enable_shared_from_this<ReadMedia>{
private:
    std::mutex mReadLock;
    std::shared_ptr<IMedia> mMedia;
    int mId;
    bool mClosed;
    bool mOpened;
    std::string mTag;

private:
    class Reader : public IReader{
    private:
        bool mAlive;
        std::shared_ptr<ReadMedia> mMedia;
        const int64_t mFrom;
        const int64_t mTo;
        std::string mLabel;
        std::shared_ptr<aloop::AMessage> mCloseNotify;
    public:
        Reader(const std::shared_ptr<ReadMedia>& media,
            const std::shared_ptr<aloop::AMessage>& closeNotify,
            int64_t from, 
            int64_t to);
        ~Reader();
        void markDead() {mAlive=false;}
        std::string toString(){return mLabel;}
        int mediaId(){return mMedia->id();}
    public:
        virtual std::string read();
        virtual int64_t from() {return mFrom;}
        virtual int64_t to() {return mTo;}
    };
    std::weak_ptr<Reader> mLastReader;

private:
    ReadMedia(const std::shared_ptr<IMedia>& media);
public:
    static std::shared_ptr<ReadMedia> create(const std::shared_ptr<IMedia>& media){
        return std::shared_ptr<ReadMedia>(new ReadMedia(media));
    }
    ~ReadMedia();
    std::shared_ptr<IReader> createReader(const std::shared_ptr<aloop::AMessage>& readerCloseNotify, int64_t from, int64_t to);
    bool isIdle();
    int id(){return mId;}
    void setId(int id);

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