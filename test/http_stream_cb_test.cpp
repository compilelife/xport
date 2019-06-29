#include "common.h"
#include "../src/http_stream_cb.h"

TEST(HttpStreamCB, readInfinite) {
    class InifiniteReader : public IReader{
    public:
        string read(){return "12345";}//return 5 bytes
        int64_t from(){return 0;}
        int64_t to(){return -1;}
        int mediaId(){return 1;}
    };

    HttpStreamCB cb(shared_ptr<IReader>(new InifiniteReader));
    int testCount = 1000;
    const int unitSize = 5;
    const int expectSize = testCount * unitSize;

    uint64_t offset = 0;
    while (--testCount >= 0){
        auto s = cb.read(offset);
        EXPECT_EQ(unitSize, s.size());
        offset += s.size();
    }

    EXPECT_EQ(expectSize, offset);
}

//每次读取5字节
class TestBufReader : public IReader{
    string mBuf;
    size_t mPos{0};

public:
    static shared_ptr<IReader> create(string buf){
        return shared_ptr<IReader>(new TestBufReader(buf));
    }
    TestBufReader(string buf):mBuf(buf){}
public:
    std::string read(){
        auto left = mBuf.size()-mPos;
        if (left >= 5){
            auto pos = mPos;
            mPos+=5;
            return mBuf.substr(pos, 5);
        }else if (left <= 0){
            return "";
        }

        auto pos = mPos;
        mPos+=left;
        return mBuf.substr(pos, left);
    }
    int64_t from(){return 0;}
    int64_t to(){return mBuf.size()-1;}
    int mediaId(){return 1;}
};

TEST(HttpStreamCB, readLimit){
    auto buf = "1234567890";//两次刚好读完
    auto reader = TestBufReader::create(buf);
    weak_ptr<IReader> wpReader(reader);
    HttpStreamCB cb(reader);
    reader.reset();

    auto s1 = cb.read(0);
    ASSERT_FALSE(wpReader.expired());
    auto s2 = cb.read(s1.size());
    ASSERT_FALSE(wpReader.expired());
    auto s3 = cb.read(s1.size()+s2.size());
    ASSERT_TRUE(wpReader.expired());

    ASSERT_EQ(5, s1.size());
    ASSERT_EQ(5, s2.size());
    ASSERT_EQ(0, s3.size());
}

TEST(HttpStreamCB, readLimitFitSize){
    auto buf = "123456789";//5+4
    HttpStreamCB cb(TestBufReader::create(buf));
    auto s1 = cb.read(0);
    auto s2 = cb.read(s1.size());
    auto s3 = cb.read(s1.size()+s2.size());

    ASSERT_EQ(5, s1.size());
    ASSERT_EQ(4, s2.size());
    ASSERT_EQ(0, s3.size());
}

TEST(HttpStreamCB, readEmpty){
    HttpStreamCB cb(TestBufReader::create(""));
    auto s1 = cb.read(0);
    ASSERT_EQ(0, s1.size());
}