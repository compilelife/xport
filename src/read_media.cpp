#include "read_media.h"
#include "log.h"

using namespace std;

#define TAG (mTag.c_str())

namespace xport
{


ReadMedia::ReadMedia(const std::shared_ptr<IMedia>& media)
    :mMedia(media),mClosed(false),mOpened(false),mTag("ReadMedia"){
}

ReadMedia::~ReadMedia(){
    if (mOpened){
        close();
    }
}

shared_ptr<IReader> ReadMedia::createReader(const std::shared_ptr<aloop::AMessage>& readerCloseNotify, int64_t from, int64_t to){
    lock_guard<mutex> l(mReadLock);
    
    {
        auto lastReader = mLastReader.lock();
        if (lastReader){
            logi("finish reader %s", lastReader->toString().c_str());
            lastReader->markDead();
        }
    }

    logi("create reader...");

    if (from < 0)
        from = 0;
    
    if (mMedia->seekable()) {
        auto size = mMedia->size();
        if (to < 0 || to >= size){
            to = size - 1;
        }

        if (size <= 0){
            loge("empty media (size=%d)", size);
            return nullptr;
        }
        if (from >= size){
            from = size - 1;
        }

        auto seekRet = mMedia->seek(from);
        if (seekRet < 0){
            loge("seek failed on seekable media");
            return nullptr;
        }
        from = seekRet;
    }else{
        from = 0;
        to = -1;
        if (mMedia->seek(from) != 0){
            loge("reset failed on stream media");
            return nullptr;
        }
    }

    if (to >= 0 && from > to){
        loge("invalid range(after corrent: from=%" PRId64 ",to=%" PRId64 ")", from, to);
        return nullptr;
    }

    shared_ptr<Reader> reader(new Reader(shared_from_this(), readerCloseNotify, from, to));
    mLastReader = reader;

    logi("reader %s created", reader->toString().c_str());
    return reader;
}

bool ReadMedia::isIdle() {
    return mLastReader.expired();
}

bool ReadMedia::open(){
    if (mOpened)
        return true;
        
    logi("open");
    mOpened = mMedia->open();
    return mOpened;
}

void ReadMedia::close(){
    if (!mClosed){
        logi("close");
        mClosed = true;
        mMedia->close();
    }
}

void ReadMedia::setId(int id){
    mId = id;
    mTag = to_string(id);
}

ReadMedia::Reader::Reader(const std::shared_ptr<ReadMedia>& media,
        const std::shared_ptr<aloop::AMessage>& closeNotify,
        int64_t from, 
        int64_t to)
    :mAlive(true), mMedia(media), mFrom(from), mTo(to),
    mCloseNotify(closeNotify){
    char buf[30]={0};
    snprintf(buf, sizeof(buf), "(%p %" PRId64 " => %" PRId64 ")", this, mFrom, mTo);
    mLabel = buf;
}

string ReadMedia::Reader::read(){
    lock_guard<mutex> l(mMedia->mReadLock);
    if (!mAlive){
        logtw(mLabel.c_str(), "already die");
        return "";
    }

    return mMedia->read();
}

ReadMedia::Reader::~Reader(){
    logti(to_string(mediaId()).c_str(), "reader %s finished", mLabel.c_str());
    if (mMedia->idleTimeout() >= 0 && mCloseNotify){
        mCloseNotify->setObject("media", mMedia);
        mCloseNotify->post(mMedia->idleTimeout()*1000L);
    }
}
    
} // namespace xport