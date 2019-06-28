#include "read_media.h"
#include "log.h"

using namespace std;

namespace xport
{


ReadMedia::ReadMedia(const std::shared_ptr<IMedia>& media)
    :mMedia(media),mClosed(false),mOpened(false){
}

ReadMedia::~ReadMedia(){
    if (mOpened){
        close();
    }
}

shared_ptr<IReader> ReadMedia::createReader(int64_t from, int64_t to){
    lock_guard<mutex> l(mReadLock);
    
    {
        auto lastReader = mLastReader.lock();
        if (lastReader){
            lastReader->markDead();
        }
    }

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

        if (mMedia->seek(from) < 0){
            loge("seek failed on seekable media");
            return nullptr;
        }
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

    shared_ptr<Reader> reader(new Reader(this, from, to));
    mLastReader = reader;

    return reader;
}

bool ReadMedia::isIdle() {
    return mLastReader.expired();
}

bool ReadMedia::open(){
    if (mOpened)
        return true;
    
    mOpened = mMedia->open();
    return mOpened;
}

void ReadMedia::close(){
    if (!mClosed){
        mClosed = true;
        mMedia->close();
    }
}

string ReadMedia::Reader::read(){
    lock_guard<mutex> l(mMedia->mReadLock);
    if (!mAlive){
        logi("already die");
        return "";
    }

    return mMedia->read();
}

    
} // namespace xport