#include "common.h"
#include "../src/media_manager.h"
#include "../src/media_factory.h"
#include "../src/log.h"

#define FAIL_EXIT(msg)\
logte("MediaManagerTest", msg);\
abort();

class ShortTimeoutMedia : public ASeekableMedia{
private:
    bool mOpened{false};
    bool mClosed{false};
public:
    ~ShortTimeoutMedia(){
        if (!mOpened){
            FAIL_EXIT("open not called");
        }

        if (!mClosed){
            FAIL_EXIT("close not called");
        }
    }
public: 
    int idleTimeout(){return 10;}

    bool open(){
        if (mOpened){
            FAIL_EXIT("should not open twice");
        }
        mOpened = true;
        return true;
    }

    void close(){
        if (mClosed){
            FAIL_EXIT("should not close twice");
        }
        mClosed = true;
    }
};

class NormalCreator: public IMediaCreator{
public:
    int scoreRequest(MediaRequest& req){
        return 100;
    }
    IMedia* create(MediaRequest& req){
        return new ShortTimeoutMedia;
    }
};

class MediaManagerTest: public testing::Test{
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}

public:
    void SetUp(){
        MediaFactory::getInstance().registerCreator(shared_ptr<IMediaCreator>(new NormalCreator));
        mManager.reset(new MediaManager);
        ASSERT_TRUE(mManager->init());
    }
    void TearDown(){
        MediaFactory::getInstance().reset();
        mManager->uninit();
        mManager.reset();
    }
protected:
    MediaRequest mRequest{nullptr};
    shared_ptr<MediaManager> mManager;
};


TEST_F(MediaManagerTest, createMedia){
    ASSERT_TRUE(mManager->createMedia(mRequest));
}

TEST_F(MediaManagerTest, createMediaFailed){
    MediaFactory::getInstance().reset();
    ASSERT_FALSE(mManager->createMedia(mRequest));
}

TEST_F(MediaManagerTest, getMedia){
    auto media = mManager->createMedia(mRequest);
    ASSERT_TRUE(media);

    auto getMedia = mManager->getMediaOrCreate(media->id(), mRequest);
    ASSERT_EQ(media, getMedia);
    ASSERT_EQ(media->id(), getMedia->id());
}

TEST_F(MediaManagerTest, getMediaOrCreate){
    ASSERT_TRUE(mManager->getMediaOrCreate(1, mRequest));
}

TEST_F(MediaManagerTest, dontUseAbandonMedia){
    auto media = mManager->createMedia(mRequest);
    ASSERT_TRUE(media);
    auto id = media->id();
    media.reset(); //abandon it

    auto media2 = mManager->getMediaOrCreate(id, mRequest);
    ASSERT_TRUE(media2);
    ASSERT_NE(id, media2->id());
}