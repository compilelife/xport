**为了让用户接入更容易，播放器兼容更好，代码更轻量，我开发了[zport](https://github.com/compilelife/zport)**

**本项目停止维护，仅保留作为知乎文章资源，谢谢支持**

xport 是一个轻量且使用方便的http推流库。通过它，你可以让自定义的媒体数据轻易地在任意播放器上播放。

成熟的媒体应用往往面对这样的需求：
- 自定义的封装
- 加密的视频
- 对接第三方的非标准媒体源
- 支持不同架构的播放器，或在新产品上迁移到新的播放器架构
- ……

其中一种灵活的方案是把自定义媒体数据推流为http，大部分播放器都能很好地支持http（vlc/ffmepg/mediaplayer/ijkplayer/kodi等）

xport就是这样一个推流方案，有以下特点：
- 易用的接口，可以轻易对接你的媒体源
- 完善的seek支持，以点播为session管理http连接，避免seek引起的重复初始化、多线程等各类问题
- 轻量易继承，只需要一个动态库，避免繁琐的配置

# 入门使用

["hello world"级Demo](example/simple.cpp)

## 1. 实现IMedia，定义你的媒体类型

`IMedia`按文件描述媒体，需要实现的方法一般是`open`/`close`/`read`/`seek`等。

为了方便使用，也可以继承`StreamMedia`或`SeekableMedia`，实现流媒体或可跳进媒体。

比如demo中的`FileMedia`，就是继承`SeekableMedia`，用FILE系列函数，推流本地文件。

## 2. 实现并注册IMediaCreator

`IMediaCreator`可以用于定义url格式，比如demo中定义点播链格式形如：http://127.0.0.1:8000/media?path=/tmp/1.mp4

则可以在`IMediaCreator::create`中解析到path，并创建`FileMedia`提供服务

## 3. 启动http服务

调用`HttpServer::start`即可。

现在你就可以用任意的播放器播放你的视频了。

# 编译

如需编译动态库：

```shell
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=release -DBUILD_SHARED_LIBS=1
make
```
头文件在`build/include`，动态库在`build/libxport.so`

如需编译test和example（test需要开发环境有gtest）：

```shell
mkdir build
cd build
cmake ..
make
```
example在`example/*_demo`，test在`test/xportest`

# 开发中

- 安全机制（防止未授权连接）
- 基于http的媒体自定义控制

# 代码结构

参考：https://zhuanlan.zhihu.com/p/71563192

# 赞助

**如果对本项目有兴趣，可以微信扫码支持我**

![support](images/support.jpeg)
