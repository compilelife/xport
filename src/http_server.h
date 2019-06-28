#ifndef HTTP_SERVER
#define HTTP_SERVER

namespace xport
{
    
class HttpServerImpl;
class HttpServer{
public:
    HttpServer();
    virtual ~HttpServer();
    bool startAsync(const char* host, int port);
    bool start(const char* host, int port);
    void stop();
private:
    HttpServerImpl* mImpl;
};

} // namespace xport



#endif