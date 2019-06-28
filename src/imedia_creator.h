#ifndef IMEDIA_CREATOR_H
#define IMEDIA_CREATOR_H

#include "imedia.h"
#include "media_request.h"

namespace xport
{

class IMediaCreator{
public:
    virtual ~IMediaCreator(){}
public:
    virtual int scoreRequest(MediaRequest& req) = 0;
    virtual IMedia* create(MediaRequest& req) = 0;
};

void releaseOwnershipToRegsiter(IMediaCreator* creator);

} // namespace xport


#endif