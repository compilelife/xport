#ifndef IMEDIA_CREATOR_H
#define IMEDIA_CREATOR_H

#include "imedia.h"
#include "media_request.h"

namespace xport
{
/**
 * \brief  媒体创建器
 * 
 * 定义何时以何种方式创建媒体
 * xport允许定义多个IMediaCreator，即允许用同一个http服务为客户端提供不同类型的媒体资源
 */
class IMediaCreator{
public:
    virtual ~IMediaCreator(){}
public:
    /**
     * 对于客户端请求req，通过返回值（分数）告知xport是否有本实例负责创建IMedia
     * @return 0~100的数值。当存在多个IMediaCreator时，由返回值最大的IMediaCreator创建IMedia
     */
    virtual int scoreRequest(MediaRequest& req) = 0;
    /**
     * @return 为客户端请求req创建的IMedia
     */
    virtual IMedia* create(MediaRequest& req) = 0;
};

/**
 * \brief 注册creator到MediaFactory
 * 
 * 等效于MediaFactory::getInstance().registerCreator(shared_ptr<IMediaCreator>(creator));
 * 
 * \param creator 要注册的IMediaCreator。函数调用结束后，不应再继续使用传递进来的creator实例。
 * \note 典型的用法是releaseOwnershipToRegister(new MediaCreator) (无需再调用delete)
 */
void releaseOwnershipToRegsiter(IMediaCreator* creator);

} // namespace xport


#endif