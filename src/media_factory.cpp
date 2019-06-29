#include "media_factory.h"
#include "log.h"

using namespace std;

#define TAG "MediaFactory"

namespace xport{
    
MediaFactory::MediaFactory(){}

MediaFactory& MediaFactory::getInstance() {
    static MediaFactory instance;
    return instance;
}

void MediaFactory::registerCreator(const shared_ptr<IMediaCreator>& creator){
    if (!creator)
        return;
    mCreators.push_back(creator);
}

shared_ptr<IMediaCreator> MediaFactory::chooseCreator(MediaRequest& req){
    int index = -1;
    int maxScore = 0;

    for (size_t i = 0; i < mCreators.size(); i++) {
        auto score = mCreators[i]->scoreRequest(req);
        if (score > maxScore){
            maxScore = score;
            index = i;

            if (maxScore >= 100)
                break;
        }
    }

    if (index >= 0){
        return mCreators[index];
    }

    logw("no creator");
    return nullptr;
}

shared_ptr<IMedia> MediaFactory::createMedia(MediaRequest& req) {
    auto creator = chooseCreator(req);

    if (creator) {
        return shared_ptr<IMedia>(creator->create(req));
    }

    return nullptr;
}

void MediaFactory::reset(){
    mCreators.clear();
}

void releaseOwnershipToRegsiter(IMediaCreator* creator){
    MediaFactory::getInstance().registerCreator(shared_ptr<IMediaCreator>(creator));
}

}