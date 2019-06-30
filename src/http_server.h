#ifndef HTTP_SERVER
#define HTTP_SERVER

namespace xport
{
    
class HttpServerImpl;
class HttpServer{
public:
    HttpServer();
    virtual ~HttpServer();
    
    /**
     * 在独立线程启动http服务
     * \param host 在该参数指定的host上启动服务，一般用"127.0.0.1"
     * \param port http端口，可以修改默认的80端口到其他端口
     * \return 服务启动是否成功
     */
    bool startAsync(const char* host, int port);

    /**
     * 在当前线程启动http服务
     * \see startAsync
     */
    bool start(const char* host, int port);

    /**
     * 停止http服务
     */
    void stop();
private:
    HttpServerImpl* mImpl;
};

} // namespace xport



#endif