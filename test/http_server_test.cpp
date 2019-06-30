#include "common.h"
#include "../src/httplib.h"
#include <regex>

using namespace httplib;

static const string gMediaData = "This is a mock media data";
static const char* gHost = "127.0.0.1";
static const int gPort = 8000;
static const char* gStreamMediaPath = "/media/stream";
static const char* gSeekableMediaPath = "/media/seekable";

static inline string readMediaData(int64_t& offset){
    if (offset >= static_cast<int64_t>(gMediaData.size())){
        return "";
    }

    const int unitSize = 5;
    auto left = gMediaData.size() - offset;
    if (left >= unitSize){
        auto ret = gMediaData.substr(offset, unitSize);
        offset += unitSize;
        return ret;
    }

    auto ret = gMediaData.substr(offset);
    offset += left;
    return ret;
}

class MockStreamMedia : public StreamMedia{
    int64_t mPos{0};
public:
    virtual bool open(){return true;}
    virtual void close(){};
    std::string read(){return readMediaData(mPos);}
protected:
    virtual bool reset(){
        mPos = 0;
        return true;
    }
};

class MockSeekableMedia : public SeekableMedia{
    int64_t mPos{0};
public:
    bool open(){return true;}
    void close(){}
    std::string read(){return readMediaData(mPos);}
    int64_t seek(int64_t offset){mPos = offset;return offset;}
    int64_t size(){return gMediaData.size();}
};

class MockStreamMediaCreator: public IMediaCreator{
public:
    int scoreRequest(MediaRequest& req){
        auto paths = req.segments();
        if (paths.size() >= 2 && paths[1] == "stream"){
            return 100;
        }
        return 0;
    }
    IMedia* create(MediaRequest& req){
        return new MockStreamMedia;
    }
};

class MockSeekableMediaCreator: public IMediaCreator{
public:
    int scoreRequest(MediaRequest& req){
        auto paths = req.segments();
        if (paths.size() >= 2 && paths[1] == "seekable"){
            return 100;
        }
        return 0;
    }
    IMedia* create(MediaRequest& req){
        return new MockSeekableMedia;
    }
};

class HTTPServerTest : public testing::Test{
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}

public:
    void SetUp(){
        releaseOwnershipToRegsiter(new MockStreamMediaCreator);
        releaseOwnershipToRegsiter(new MockSeekableMediaCreator);
        ASSERT_TRUE(mHttpServer.startAsync(gHost, gPort));
    }
    void TearDown(){
       mHttpServer.stop();
       MediaFactory::getInstance().reset();
    }

protected:
    HttpServer mHttpServer;
};

static void checkCookie(const string& cookie, const string& path){
    regex idPattern("id=(-?\\d+)");
    regex pathPattern("path=(.+)");

    smatch sm;
    ASSERT_TRUE(regex_search(cookie, sm, idPattern))<<cookie;
    ASSERT_TRUE(regex_search(cookie, sm, pathPattern))<<cookie;
    ASSERT_EQ(path, sm.str(1))<<cookie;
}

static string getId(const string& cookie){
    regex idPattern("id=(-?\\d+)");

    smatch sm;
    regex_search(cookie, sm, idPattern);
    return sm.str(1);
}

TEST_F(HTTPServerTest, checkStreamHeader){
    Client client(gHost, gPort);
    Headers headers;
    auto response = client.Get(gStreamMediaPath, headers);

    ASSERT_TRUE(response);

    ASSERT_EQ(200, response->status);
    ASSERT_STREQ("bytes 0-", response->get_header_value("Content-Range").c_str());
    ASSERT_FALSE(response->has_header("Content-Length"));
    ASSERT_STREQ("bytes", response->get_header_value("Accept-Ranges").c_str());
    checkCookie(response->get_header_value("Set-Cookie"), gStreamMediaPath);
}

TEST_F(HTTPServerTest, checkSeekableHeader){
    Client client(gHost, gPort);
    Headers headers;
    auto response = client.Get(gSeekableMediaPath, headers);

    ASSERT_TRUE(response);

    ASSERT_EQ(206, response->status);
    ASSERT_EQ("bytes 0-"+to_string(gMediaData.size()-1)+"/"+to_string(gMediaData.size()), response->get_header_value("Content-Range"));
    ASSERT_EQ(to_string(gMediaData.size()), response->get_header_value("Content-Length"));
    ASSERT_EQ("bytes", response->get_header_value("Accept-Ranges"));
    checkCookie(response->get_header_value("Set-Cookie"), gSeekableMediaPath);
}

TEST_F(HTTPServerTest, readStreamMedia){
    Client client(gHost, gPort);
    auto response = client.Get(gStreamMediaPath);
    ASSERT_EQ(gMediaData, response->body);
}

TEST_F(HTTPServerTest, readSeekableMedia){
    Client client(gHost, gPort);
    auto response = client.Get(gSeekableMediaPath);
    ASSERT_EQ(gMediaData, response->body);
}

TEST_F(HTTPServerTest, seekStreamMedia){
    Client client(gHost, gPort);
    Headers header;
    header.emplace("Range", "byte=5-");
    auto response = client.Get(gStreamMediaPath, header);
    ASSERT_EQ(gMediaData, response->body);
}

TEST_F(HTTPServerTest, seekSeekableMedia){
    string id = "";
    {
        Client client(gHost, gPort);
        Headers header;
        header.emplace("Range", "bytes=0-5");
        auto response = client.Get(gSeekableMediaPath, header);
        ASSERT_EQ(gMediaData.substr(0, 6), response->body);
        id = getId(response->get_header_value("Set-Cookie"));
        ASSERT_FALSE(id.empty());
    }

    {
        Client client(gHost, gPort);
        Headers header;
        header.emplace("Range", "bytes=11-");
        header.emplace("Cookie", "id="+id);
        auto response = client.Get(gSeekableMediaPath, header);
        ASSERT_EQ(gMediaData.substr(11), response->body);
        ASSERT_EQ(id, getId(response->get_header_value("Set-Cookie")));
    }
    
    {
        Client client(gHost, gPort);
        Headers header;
        header.emplace("Range", "bytes=6-10");
        header.emplace("Cookie", "id="+id);
        auto response = client.Get(gSeekableMediaPath, header);
        ASSERT_EQ(gMediaData.substr(6, 5), response->body);
        ASSERT_EQ(id, getId(response->get_header_value("Set-Cookie")));
    }
}