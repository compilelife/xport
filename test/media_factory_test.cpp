#include "common.h"
#include "../src/media_factory.h"
#include "../src/httplib.h"

class MediaFactoryTest: public testing::Test{
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}

public:
    void SetUp(){
        mMediaRequestImpl = new httplib::Request;
        mRequest = new MediaRequest((MediaRequestImpl*)mMediaRequestImpl);
    }
    void TearDown(){
        delete mMediaRequestImpl;
        MediaFactory::getInstance().reset();
    }
protected:
    MediaRequest* mRequest;
    httplib::Request* mMediaRequestImpl;
};

class IdableCreator : public IMediaCreator{
    function<int(MediaRequest&)> mScoreFunc;
public:
    IdableCreator(function<int(MediaRequest&)> scoreFunc)
        :mScoreFunc(scoreFunc){
    }
    ~IdableCreator(){
        // delete id;
    }
public:
    IMedia* id{new ASeekableMedia};//请不要在生产环境里这么做，除非可以支持多个客户端同时read同一个media
    int scoreRequest(MediaRequest& req){
        return mScoreFunc(req);
    }
    IMedia* create(MediaRequest& req){
        return id;
    }
};

TEST_F(MediaFactoryTest, SingleCreator) {
    shared_ptr<IdableCreator> creator(new IdableCreator([](MediaRequest& req){
        return 100;
    }));
    MediaFactory::getInstance().registerCreator(creator);

    auto media = MediaFactory::getInstance().createMedia(*mRequest);
    ASSERT_EQ(creator->id, media.get());
}

TEST_F(MediaFactoryTest, SelectCreator) {
    auto encCreator = new IdableCreator([](MediaRequest& req){
        auto path = req.segments();
        if (path.size() >= 1 && path[0] == "enc"){
            return 100;
        }
        return 0;
    });

    auto cameraCreator = new IdableCreator([](MediaRequest& req){
        auto path = req.segments();
        if (path.size() >= 1 && path[0] == "camera"){
            return 100;
        }
        return 0;
    });

    releaseOwnershipToRegsiter(encCreator);
    releaseOwnershipToRegsiter(cameraCreator);

    mMediaRequestImpl->path = "/camera";

    auto media = MediaFactory::getInstance().createMedia(*mRequest);
    ASSERT_EQ(cameraCreator->id, media.get());
}

TEST_F(MediaFactoryTest, GetNothing){
    ASSERT_EQ(nullptr, MediaFactory::getInstance().createMedia(*mRequest));

    MediaFactory::getInstance().registerCreator(nullptr);

    ASSERT_EQ(nullptr, MediaFactory::getInstance().createMedia(*mRequest));
}