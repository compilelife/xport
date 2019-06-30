#define EXCLUDE_MAIN

#include "simple.cpp"
#include "decrypt.cpp"
#include "m3u8.cpp"

int main(int argc, char* argv[]){
    releaseOwnershipToRegsiter(new FileMediaCreator);
    releaseOwnershipToRegsiter(new EncryptedMediaCreator);
    releaseOwnershipToRegsiter(new M3U8MediaCreator);

    //启动http服务
    HttpServer server;
    if (!server.start("127.0.0.1", 8000)){
        printf("start http server failed\n");
        return -1;
    }

    return 0;
}