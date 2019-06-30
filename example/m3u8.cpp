#include "../src/xport.h"
#include <stdio.h>
#include <string>

using namespace std;
using namespace xport;

#define RES_DIR "../../../mac/res/video/m3u8/"
static const int VIDEO_COUNT = 3;
static const char* VIDEOS[VIDEO_COUNT] = {"1.ts", "2.ts", "3.ts"};

//M3U8Media 把多个视频片段当做一个连续的视频流推送（类似M3U8)
class M3U8Media : public StreamMedia{
    FILE* mFp{nullptr};
    int mCurVideo{0};
public:
    bool open(){
        string path{RES_DIR};
        path+=VIDEOS[mCurVideo];
        printf("open %s\n", path.c_str());
        mFp = fopen(path.c_str(), "rb");
        return mFp != nullptr;
    }

    void close(){
        if (mFp){
            fclose(mFp);
            mFp = nullptr;
        }
    }

    string read(){
        const int unitSize = 4096;
        char buf[unitSize];
        
        auto ret = readBuf(buf, unitSize);
        if (ret > 0){
            return string(buf, ret);
        }else if (ret == 0){
            printf("eof\n");
            return "";
        }else{
            printf("error\n");
            return "";
        }

        return "";
    }

    bool reset(){
        close();
        mCurVideo = 0;
        return open();
    }

private:
    int nextVideo(){
        ++mCurVideo;
        if (mCurVideo >= VIDEO_COUNT)
            return 0;
        
        return open() ? 1 : -1;
    }

    int readBuf(char* buf, int size) {
        auto ret = fread(buf, 1, size, mFp);

        if (ret == 0) {
            auto nextRet = nextVideo();
            if (nextRet <= 0)
                return nextRet;
            else
                return readBuf(buf, size);
        }

        return ret;
    }
};

//http://127.0.0.1:8000/media/m3u8
class M3U8MediaCreator : public IMediaCreator{
public:
    int scoreRequest(MediaRequest& req){
        auto segs = req.segments();
        return segs[1] == "m3u8";
    }

    IMedia* create(MediaRequest& req){
        return new M3U8Media;
    }
};

#ifndef EXCLUDE_MAIN
int main(int argc, char* argv[]){
    releaseOwnershipToRegsiter(new M3U8MediaCreator);

    //启动http服务
    HttpServer server;
    if (!server.start("127.0.0.1", 8000)){
        printf("start http server failed\n");
        return -1;
    }

    return 0;
}
#endif