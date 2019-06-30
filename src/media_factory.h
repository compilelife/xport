#ifndef MEDIA_FACTORY
#define MEDIA_FACTORY

#include "imedia_creator.h"
#include <memory>
#include <vector>

namespace xport
{

/**
 * \brief MediaFactory用于注册IMediaCreator，和自动选择合适的IMediaCreator用于创建媒体资源的实例
 */
class MediaFactory{
private:
    MediaFactory();
    std::vector<std::shared_ptr<IMediaCreator>> mCreators;

public:
    static MediaFactory& getInstance();

public:
    /**
     * 注册一个IMediaCreator
     */
    void registerCreator(const std::shared_ptr<IMediaCreator>& creator);
    
    /**
     * @return 根据req选择合适的IMediaCreator创建IMedia。如果没有合适的IMediaCreator或创建失败，就返回nullptr
     */
    std::shared_ptr<IMedia> createMedia(MediaRequest& req);
    void reset();//主要用于测试

private:
    std::shared_ptr<IMediaCreator> chooseCreator(MediaRequest& req);
};

} // namespace xport


#endif