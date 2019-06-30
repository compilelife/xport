#include "../src/xport.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace xport;
using namespace std;

#define KEY_LEN 8//假设我们采用固定秘钥

//EncryptedMedia 以加密文件做媒体，从http推送解密后的数据
//这里我们假设了一种简单的加密：
//加密文件中，前8个字节是密钥，之后跟随加密后的视频数据。
//视频加密的方式是把视频数据与8字节秘钥做异或运算
class EncryptedMedia: public SeekableMedia{
    FILE* mFp;
    int64_t mSize;
    uint8_t mKey[KEY_LEN];
    int mKeyOffset{0};
public:
    EncryptedMedia(const string& path){
        mFp = fopen(path.c_str(), "rb");
    }
    virtual bool open(){
        if (mFp){
            fseek(mFp, 0, SEEK_END);
            mSize = ftell(mFp);
            fseek(mFp, 0, SEEK_SET);
            if (fread(mKey, 1, KEY_LEN, mFp) < KEY_LEN){
                printf("failed to read key\n");
                fclose(mFp);
                mFp = nullptr;
                return false;
            }

            mSize -= KEY_LEN;//秘钥长度无需计入
            return true;
        }

        return false;
    }
    virtual void close(){
        if (mFp)
            fclose(mFp);
    }

    void decrypt(char* buf, size_t ret){
        char* pbuf = buf;
        do{
            *pbuf = (*pbuf) ^ (mKey[mKeyOffset++]);
            ++pbuf;
            if (mKeyOffset >= KEY_LEN) {
                mKeyOffset = 0;
            }
        }while((size_t)(pbuf - buf)<ret);
    }
    virtual std::string read(){
        const int unitSize = KEY_LEN * 512;
        char buf[unitSize];
        auto ret = fread(buf, 1, unitSize, mFp);
        if (ret == 0){
            return "";
        }

        decrypt(buf, ret);
        return string(buf, ret);
    }
    virtual int64_t seek(int64_t offset){
        auto encryptedOffset = offset + KEY_LEN;//对应的加密文件（实体文件）的偏移位置
        mKeyOffset = offset - (offset / KEY_LEN * KEY_LEN);//seek不会刚好落在8字节开头，所以要记录密钥偏移
        return fseek(mFp, encryptedOffset, SEEK_SET) < 0 ? -1 : offset;
    }
    virtual int64_t size(){
        return mSize;
    }
};

//http://127.0.0.1:8000/media/encrypt?path=/tmp/1.mp4
class EncryptedMediaCreator : public IMediaCreator{
public:
    int scoreRequest(MediaRequest& req){
        auto segs = req.segments();
        return segs[1] == "encrypt";
    }

    IMedia* create(MediaRequest& req){
        auto path = req.getParamValue("path");
        return new EncryptedMedia(path);
    }
};

#ifndef EXCLUDE_MAIN
static void encrypt(const char* path, const char* savepath);
int main(int argc, char* argv[]){
    if (argc >= 3){
        printf("encrypt %s to %s\n", argv[1], argv[2]);
        encrypt(argv[1],argv[2]);
    }else{
        printf("decrypt to http\n");
        releaseOwnershipToRegsiter(new EncryptedMediaCreator);

        //启动http服务
        HttpServer server;
        if (!server.start("127.0.0.1", 8000)){
            printf("start http server failed\n");
            return -1;
        }
    }

    return 0;
}

//这是对应的加密代码，供参考
static void encrypt(const char* path, const char* savepath){
    uint8_t key[KEY_LEN];
    srand(time(nullptr));

    for (size_t i = 0; i < KEY_LEN; i++){
        key[i] = rand()%256;
    }

    FILE* from = fopen(path, "rb");
    FILE* to = fopen(savepath, "wb+");
    if (from == nullptr || to == nullptr){
        exit(-1);
    }
    
    fwrite(key, 1, KEY_LEN, to);
    while (!feof(from)){
        char buf[KEY_LEN];
        auto ret = fread(buf, 1, KEY_LEN, from);
        for (size_t i = 0; i < ret; i++){
            buf[i] ^= key[i];
        }
        fwrite(buf, 1, ret, to);
    }
    
    fclose(from);
    fclose(to);
}
#endif