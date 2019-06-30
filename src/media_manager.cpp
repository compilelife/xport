#include "media_manager.h"
#include "media_factory.h"
#include "log.h"

using namespace std;
using namespace aloop;

#define TAG "MediaManager"

namespace xport
{

enum {
    kWhatGetMedia,
    kWhatCreateMedia,
    kWhatWatchDog
};

MediaManager::MediaManager()
    :mNextId{0}{
}
void MediaManager::uninit(){
    mLooper->stop();
}

bool MediaManager::init(){
    mLooper= ALooper::create();
    if (OK != mLooper->start()){
        loge("failed to create media manager looper");
        return false;
    }
    
    if (INVALID_HANDLER_ID == mLooper->registerHandler(shared_from_this())){
        loge("failed to register media manager handler");
        return false;
    }

    return true;
}

MediaManager::~MediaManager(){
    mLooper->stop();
}

shared_ptr<ReadMedia> MediaManager::getMediaOrCreate(int id, MediaRequest& request) {
    auto msg = AMessage::create(kWhatGetMedia, shared_from_this());
    msg->setInt32("id", id);
    msg->setPointer("req", &request);

    auto response = AMessage::createNull();
    if (OK != msg->postAndAwaitResponse(&response)){
        loge("failed to post kWhatGetMedia for id: %d", id);
        return nullptr;
    }

    shared_ptr<ReadMedia> ret;
    if (!response->findObject("media", &ret)){
        loge("retrieve media from response failed for id: %d", id);
        return nullptr;
    }

    return ret;
}

shared_ptr<ReadMedia> MediaManager::createMedia(MediaRequest& request) {
    auto msg = AMessage::create(kWhatCreateMedia, shared_from_this());
    msg->setPointer("req", &request);

    auto response = AMessage::createNull();
    if (OK != msg->postAndAwaitResponse(&response)){
        loge("failed to post kWhatCreateMedia");
        return nullptr;
    }

    shared_ptr<ReadMedia> ret;
    if (!response->findObject("media", &ret)){
        loge("retrieve media from response failed");
        return nullptr;
    }

    return ret;
}

shared_ptr<AMessage> MediaManager::obtainMediaWatchDog(){
    return AMessage::create(kWhatWatchDog, shared_from_this());
}

void MediaManager::onMessageReceived(const shared_ptr<AMessage> &msg) {
    switch (msg->what())
    {
    case kWhatWatchDog:{
        shared_ptr<ReadMedia> media;
        msg->findObject("media", &media);
        if (media->isIdle()){
            logti(to_string(media->id()).c_str(), "idle timeout");
            media->close();
            mMedias.erase(media->id());
        }
        break;
    }
        
    case kWhatCreateMedia:{
        MediaRequest* req = nullptr;

        msg->findPointer("req", (void**)&req);
        auto imedia = MediaFactory::getInstance().createMedia(*req);
        auto response = AMessage::create();
        if (imedia) {
            auto id = ++mNextId;
            auto media = ReadMedia::create(imedia);
            media->setId(id);
            if (media->open()){
                response->setObject("media", media);
                mMedias[id] = media;
            }else{
                response->setObject("media", nullptr);
            }
        }else{
            response->setObject("media", nullptr);
        }

        shared_ptr<AReplyToken> replyId;
        if (msg->senderAwaitsResponse(&replyId)){
            response->postReply(replyId);
        }
        break;
    }

    case kWhatGetMedia:{
        int32_t id;
        msg->findInt32("id", &id);
        
        auto media = mMedias[id].lock();

        if (media) {
            auto response = AMessage::create();
            response->setObject("media", media);
            shared_ptr<AReplyToken> replyId;
            if (msg->senderAwaitsResponse(&replyId)){
                response->postReply(replyId);
            }
        }else{
            msg->setWhat(kWhatCreateMedia);
            msg->post();
        }

        break;
    }
    
    default:
        break;
    }
}

} // namespace xport
