#ifndef MEDIA_MANAGER_H
#define MEDIA_MANAGER_H

#include "read_media.h"
#include "media_request.h"
#include "aloop.h"
#include <map>
#include <atomic>

namespace xport
{

class MediaManager : public aloop::AHandler, public std::enable_shared_from_this<MediaManager>{
public:
    MediaManager();
    virtual ~MediaManager();
    bool init();
    void uninit();
    std::shared_ptr<ReadMedia> getMediaOrCreate(int id, MediaRequest& request);
    std::shared_ptr<ReadMedia> createMedia(MediaRequest& request);
    std::shared_ptr<aloop::AMessage> obtainMediaWatchDog();

protected:
    void onMessageReceived(const std::shared_ptr<aloop::AMessage> &msg);

private:
    std::shared_ptr<aloop::ALooper> mLooper;
    std::map<int, std::weak_ptr<ReadMedia>> mMedias;
    int mNextId;
};    

} // namespace xport


#endif