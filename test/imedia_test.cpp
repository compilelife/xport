#include <gtest/gtest.h>
#include "common.h"

TEST(IMedia, StreamMedia){
    AStreamMedia media;
    ASSERT_TRUE(media.open());
    ASSERT_EQ(0, media.seek(1000L)); //所有的seek都会导致从0开始读
    ASSERT_FALSE(media.seekable());
    ASSERT_LT(media.size(), 0);
    media.close();
}

TEST(IMedia, SeekableMedia){
    ASeekableMedia media;
    ASSERT_TRUE(media.open());
    ASSERT_TRUE(media.seekable());
    media.close();
}