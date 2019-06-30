#include "common.h"
#include "../src/read_media.h"

//因为ASeekableMedia和AStreamMedia总是open成功，且不需要open即可调用其他接口，为了测试方便，以下均不调用open/close

#define checkRange(media, argFrom, argTo, retFrom, retTo)\
    {auto reader = media->createReader(nullptr, argFrom, argTo);\
    ASSERT_TRUE(reader);\
    ASSERT_EQ(retFrom, reader->from());\
    ASSERT_EQ(retTo, reader->to());}

TEST(ReadMedia, SeekableRange){
    class FixedSizeMedia : public ASeekableMedia{
    public:
        int64_t size() {return 300L;}
    };

    shared_ptr<IMedia> media(new FixedSizeMedia);
    auto size = media->size();
    auto readMedia = ReadMedia::create(media);

    checkRange(readMedia, -1, -1, 0, size-1);
    checkRange(readMedia, size, -1, size-1, size-1);
    checkRange(readMedia, -1, size, 0, size-1);
    checkRange(readMedia, 100, 200, 100, 200);
    
    ASSERT_FALSE(readMedia->createReader(nullptr, 100,50));
}

TEST(ReadMedia, StreamRange) {
    shared_ptr<IMedia> media(new AStreamMedia);
    auto readMedia = ReadMedia::create(media);

    checkRange(readMedia, -1, -1, 0, -1);
    checkRange(readMedia, 100, -1, 0, -1);

    class ResetFailedMedia : public AStreamMedia{
    protected:
        bool reset(){return false;}
    };

    ASSERT_FALSE(ReadMedia::create(shared_ptr<IMedia>(new ResetFailedMedia))->createReader(nullptr, 100, -1));
}

TEST(ReadMedia, IsIdle) {
    auto media = ReadMedia::create(shared_ptr<IMedia>(new AStreamMedia));

    auto reader = media->createReader(nullptr, 0,-1);
    ASSERT_TRUE(reader);
    ASSERT_FALSE(media->isIdle());

    reader.reset();
    ASSERT_TRUE(media->isIdle());
}

TEST(ReadMedia, ReaderConflict){
    class StringMedia: public ASeekableMedia{
    public:
        string read(){
            return "hello";
        }
    };

    auto media = ReadMedia::create(shared_ptr<IMedia>(new StringMedia));

    auto first = media->createReader(nullptr, 0, -1);
    ASSERT_FALSE(first->read().empty());

    auto second = media->createReader(nullptr, 0, -1);
    ASSERT_TRUE(first->read().empty());
    ASSERT_FALSE(second->read().empty());

    second.reset();
    ASSERT_TRUE(media->isIdle());
}