#include "common.h"
#include "../src/media_manager.h"
#include "../src/media_factory.h"
#include "../src/httplib.h"

using namespace httplib;

class TimeoutMedia : public AStreamMedia{
    const int mTimeout;
public:
    TimeoutMedia(int timeout):mTimeout(timeout){}
    int idleTimeout(){return mTimeout;}
};

class TimeoutMediaCreator : public IMediaCreator{
public:
    int scoreRequest(MediaRequest& req){return 100;}
    IMedia* create(MediaRequest& req){
        auto segs = req.segments();
        auto timeout = stoi(segs[0]);
        return new TimeoutMedia(timeout);
    }
};

class WatchDogTest: public testing::Test{
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}

public:
    void SetUp(){
        releaseOwnershipToRegsiter(new TimeoutMediaCreator);
        mManager.reset(new MediaManager);
        ASSERT_TRUE(mManager->init());
    }
    void TearDown(){
        MediaFactory::getInstance().reset();
        mManager->uninit();
        mManager.reset();
    }
protected:
    shared_ptr<MediaManager> mManager;
};

#define TIMEOUT_REQUEST(timeout) \
Request impl;\
impl.path="/"+to_string(timeout);\
MediaRequest req((MediaRequestImpl*)&impl);

#define PREPARE_TEST()\
weak_ptr<ReadMedia> wpMedia;\
auto media = mManager->createMedia(req);\
wpMedia = media;\
auto id = media->id();\
auto reader = media->createReader(mManager->obtainMediaWatchDog(), 0, -1);

TEST_F(WatchDogTest, positiveTimeoutMedia){
    TIMEOUT_REQUEST(100);

    PREPARE_TEST();

    //模拟客户端关闭连接
    media.reset();
    reader.reset();

    //超时前还可以获取到media
    {
        auto media = mManager->getMediaOrCreate(id, req);
        ASSERT_EQ(id, media->id());
        ASSERT_FALSE(wpMedia.expired());
    }

    //超时后media释放
    this_thread::sleep_for(chrono::milliseconds(110));
    {
        auto media = mManager->getMediaOrCreate(id, req);
        mManager->uninit();
        ASSERT_NE(id, media->id());
        ASSERT_TRUE(wpMedia.expired());
    }
}

TEST_F(WatchDogTest, negativeTimeoutMedia){
    TIMEOUT_REQUEST(-1);

    PREPARE_TEST();

    //模拟客户端关闭连接
    media.reset();
    reader.reset();

    //此时media已被释放
    {
        auto media = mManager->getMediaOrCreate(id, req);
        ASSERT_NE(id, media->id());
        ASSERT_TRUE(wpMedia.expired());
    }
}

TEST_F(WatchDogTest, zeroTimeoutMedia){
    TIMEOUT_REQUEST(0);

    PREPARE_TEST();

    //模拟客户端关闭连接
    media.reset();
    reader.reset();

    //此时media还没被释放(TODO: 这里的测试可能是会失败的，因为reader.reset()发出的watchdog可能在getMediaOrCreate前被调用)
    {
        auto media = mManager->getMediaOrCreate(id, req);
        ASSERT_NE(id, media->id());
        ASSERT_TRUE(wpMedia.expired());
    }

    this_thread::sleep_for(chrono::milliseconds(10));
    //此时media已被释放
    {
        auto media = mManager->getMediaOrCreate(id, req);
        ASSERT_NE(id, media->id());
        ASSERT_TRUE(wpMedia.expired());
    }
}