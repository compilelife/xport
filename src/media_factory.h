#ifndef MEDIA_FACTORY
#define MEDIA_FACTORY

#include "imedia_creator.h"
#include <memory>
#include <vector>

namespace xport
{
    
class MediaFactory{
private:
    MediaFactory();
    std::vector<std::shared_ptr<IMediaCreator>> mCreators;

public:
    static MediaFactory& getInstance();

public:
    void registerCreator(const std::shared_ptr<IMediaCreator>& creator);
    std::shared_ptr<IMedia> createMedia(MediaRequest& req);
    void reset();//主要用于测试

private:
    std::shared_ptr<IMediaCreator> chooseCreator(MediaRequest& req);
};

} // namespace xport


#endif