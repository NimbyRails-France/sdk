#include "platform/linux/memory_broker.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <array>
#include <cstring>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
#include <atomic>

namespace nimby::platform::linux_os {
namespace {
constexpr uint64_t protocol=0x314452464e; // NRF read protocol 1
constexpr size_t maxRead=256*1024*1024;
struct Request {uint64_t magic,address,size;};
struct Reply {uint64_t magic,size;};
struct Descriptor {int fd=-1;~Descriptor(){if(fd>=0)close(fd);}};
uint64_t startTime() {
    std::ifstream input("/proc/self/stat");std::string line;std::getline(input,line);
    auto end=line.rfind(')');if(end==std::string::npos)throw std::runtime_error("Missing process identity");
    std::istringstream fields(line.substr(end+2));std::string value;
    for(int i=3;i<=22;++i)if(!(fields>>value))throw std::runtime_error("Invalid process identity");
    return std::stoull(value);
}
std::string endpoint(int pid,uint64_t started,bool create) {
    const auto directory="/tmp/nrf-sdk-"+std::to_string(getuid());
    if(create&&mkdir(directory.c_str(),0700)<0&&errno!=EEXIST)throw std::runtime_error("Cannot create SDK runtime directory");
    struct stat metadata{};
    if(lstat(directory.c_str(),&metadata)<0||!S_ISDIR(metadata.st_mode)||metadata.st_uid!=getuid()||(metadata.st_mode&0777)!=0700)
        throw std::runtime_error("Invalid SDK runtime directory ownership");
    return directory+"/"+std::to_string(pid)+"-"+std::to_string(started)+".sock";
}
sockaddr_un addressOf(const std::string& path) {
    sockaddr_un result{};result.sun_family=AF_UNIX;
    if(path.size()>=sizeof result.sun_path)throw std::runtime_error("SDK endpoint path too long");
    std::memcpy(result.sun_path,path.c_str(),path.size()+1);return result;
}
void timeout(int fd) {
    timeval value{2,0};
    setsockopt(fd,SOL_SOCKET,SO_RCVTIMEO,&value,sizeof value);
    setsockopt(fd,SOL_SOCKET,SO_SNDTIMEO,&value,sizeof value);
}
bool receive(int fd,void* output,size_t size) {
    auto* cursor=static_cast<char*>(output);
    while(size){auto n=recv(fd,cursor,size,0);if(n<0&&errno==EINTR)continue;if(n<=0)return false;cursor+=n;size-=n;}
    return true;
}
bool sendAll(int fd,const void* input,size_t size) {
    auto* cursor=static_cast<const char*>(input);
    while(size){auto n=send(fd,cursor,size,MSG_NOSIGNAL);if(n<0&&errno==EINTR)continue;if(n<=0)return false;cursor+=n;size-=n;}
    return true;
}
bool peer(int fd,int expectedPid=0) {
    ucred credentials{};socklen_t size=sizeof credentials;
    return getsockopt(fd,SOL_SOCKET,SO_PEERCRED,&credentials,&size)==0&&size==sizeof credentials&&
        credentials.uid==getuid()&&(!expectedPid||credentials.pid==expectedPid);
}
bool sendMemory(int socket,int memory) {
    uint64_t magic=protocol;
    iovec payload{&magic,sizeof magic};
    alignas(cmsghdr) std::array<char,CMSG_SPACE(sizeof(int))> control{};
    msghdr message{};message.msg_iov=&payload;message.msg_iovlen=1;
    message.msg_control=control.data();message.msg_controllen=control.size();
    auto* header=CMSG_FIRSTHDR(&message);
    header->cmsg_level=SOL_SOCKET;header->cmsg_type=SCM_RIGHTS;header->cmsg_len=CMSG_LEN(sizeof(int));
    std::memcpy(CMSG_DATA(header),&memory,sizeof memory);
    ssize_t sent;do{sent=sendmsg(socket,&message,MSG_NOSIGNAL);}while(sent<0&&errno==EINTR);
    return sent==sizeof magic;
}
int receiveMemory(int socket) {
    uint64_t magic=0;
    iovec payload{&magic,sizeof magic};
    alignas(cmsghdr) std::array<char,CMSG_SPACE(sizeof(int))> control{};
    msghdr message{};message.msg_iov=&payload;message.msg_iovlen=1;
    message.msg_control=control.data();message.msg_controllen=control.size();
    ssize_t received;do{received=recvmsg(socket,&message,MSG_WAITALL|MSG_CMSG_CLOEXEC);}while(received<0&&errno==EINTR);
    Descriptor memory;
    unsigned descriptors=0;
    for(auto* header=CMSG_FIRSTHDR(&message);header;header=CMSG_NXTHDR(&message,header)) {
        if(header->cmsg_level!=SOL_SOCKET||header->cmsg_type!=SCM_RIGHTS||header->cmsg_len<CMSG_LEN(0))continue;
        const auto count=(header->cmsg_len-CMSG_LEN(0))/sizeof(int);
        for(size_t index=0;index<count;++index) {
            int fd=-1;std::memcpy(&fd,CMSG_DATA(header)+index*sizeof(int),sizeof fd);
            if(descriptors++==0)memory.fd=fd;else if(fd>=0)close(fd);
        }
    }
    if(received!=sizeof magic||magic!=protocol||(message.msg_flags&(MSG_CTRUNC|MSG_TRUNC))||descriptors!=1||memory.fd<0)return -1;
    const auto flags=fcntl(memory.fd,F_GETFL);
    if(flags<0||(flags&O_ACCMODE)!=O_RDONLY)return -1;
    auto result=memory.fd;memory.fd=-1;return result;
}
}
struct MemoryBroker::Impl {
    Descriptor listener,memory;
    std::string path;
    bool bound=false;
    std::atomic<bool> stopping=false;
    std::vector<std::thread> workers;
    Impl() {
        path=endpoint(getpid(),startTime(),true);
        listener.fd=socket(AF_UNIX,SOCK_STREAM|SOCK_CLOEXEC|SOCK_NONBLOCK,0);
        memory.fd=open("/proc/self/mem",O_RDONLY|O_CLOEXEC);
        if(listener.fd<0||memory.fd<0)throw std::runtime_error("Cannot open SDK observation broker");
        auto address=addressOf(path);
        if(bind(listener.fd,reinterpret_cast<sockaddr*>(&address),sizeof address)<0)throw std::runtime_error("SDK endpoint already exists");
        bound=true;
        if(chmod(path.c_str(),0600)<0||listen(listener.fd,8)<0){unlink(path.c_str());throw std::runtime_error("Cannot listen on SDK endpoint");}
        try {for(unsigned i=0;i<4;++i)workers.emplace_back([this]{run();});}
        catch(...){stopping=true;for(auto& worker:workers)worker.join();unlink(path.c_str());throw;}
    }
    ~Impl(){stopping=true;for(auto& worker:workers)worker.join();if(bound)unlink(path.c_str());}
    void run() noexcept {
        while(!stopping) {
            pollfd event{listener.fd,POLLIN,0};if(poll(&event,1,100)<=0)continue;
            Descriptor connection{accept4(listener.fd,nullptr,nullptr,SOCK_CLOEXEC)};
            if(connection.fd<0||!peer(connection.fd))continue;
            timeout(connection.fd);
            try {serve(connection.fd);}catch(...){}
        }
    }
    void serve(int fd) {
        while(!stopping) {
            // An idle client may pause observation for longer than the transfer
            // timeout. Wait for a new request without discarding its connection;
            // partial requests still have the bounded receive timeout below.
            pollfd event{fd,POLLIN,0};
            const auto ready=poll(&event,1,100);
            if(ready<0){if(errno==EINTR)continue;return;}
            if(!ready)continue;
            if(!(event.revents&POLLIN))return;
            Request request{};if(!receive(fd,&request,sizeof request))return;
            if(request.magic==protocol&&!request.address&&!request.size) {
                // Grant the same user's client a read-only descriptor bound to
                // this address space. This avoids a socket round trip for every
                // small field; it grants neither writes nor ptrace control.
                sendMemory(fd,memory.fd);return;
            }
            if(request.magic!=protocol||!request.size||request.size>maxRead||request.address<0x10000||request.address>INT64_MAX-request.size)return;
            std::vector<char> bytes(request.size);
            const auto count=pread(memory.fd,bytes.data(),bytes.size(),static_cast<off_t>(request.address));
            Reply reply{protocol,count==static_cast<ssize_t>(bytes.size())?request.size:0};
            if(!sendAll(fd,&reply,sizeof reply)||(reply.size&&!sendAll(fd,bytes.data(),bytes.size())))return;
        }
    }
};
MemoryBroker::MemoryBroker():impl_(std::make_unique<Impl>()){}
MemoryBroker::~MemoryBroker()=default;
MemoryBrokerClient::MemoryBrokerClient(int pid,uint64_t started) {
    auto address=addressOf(endpoint(pid,started,false));
    Descriptor connection{socket(AF_UNIX,SOCK_STREAM|SOCK_CLOEXEC,0)};
    if(connection.fd<0)throw std::runtime_error("Cannot connect SDK observation broker");
    timeout(connection.fd);
    if(connect(connection.fd,reinterpret_cast<sockaddr*>(&address),sizeof address)<0||!peer(connection.fd,pid))
        throw std::runtime_error("Game SDK observation broker unavailable");
    Request descriptor{protocol,0,0};
    if(sendAll(connection.fd,&descriptor,sizeof descriptor))memory_=receiveMemory(connection.fd);
    if(memory_>=0)return;
    // Older, already running game brokers support only individual reads.
    close(connection.fd);connection.fd=socket(AF_UNIX,SOCK_STREAM|SOCK_CLOEXEC,0);
    if(connection.fd<0)throw std::runtime_error("Cannot reconnect SDK observation broker");
    timeout(connection.fd);
    if(connect(connection.fd,reinterpret_cast<sockaddr*>(&address),sizeof address)<0||!peer(connection.fd,pid))
        throw std::runtime_error("Game SDK observation broker unavailable");
    socket_=connection.fd;connection.fd=-1;
}
MemoryBrokerClient::~MemoryBrokerClient(){if(socket_>=0)close(socket_);if(memory_>=0)close(memory_);}
bool MemoryBrokerClient::read(uint64_t address,void* output,size_t size) noexcept {
    if(!output||!size||size>maxRead||address<0x10000||address>INT64_MAX-size)return false;
    if(memory_>=0) {
        const auto count=pread(memory_,output,size,static_cast<off_t>(address));
        if(count==static_cast<ssize_t>(size))return true;
        std::memset(output,0,size);return false;
    }
    Request request{protocol,address,size};Reply reply{};
    if(socket_>=0&&sendAll(socket_,&request,sizeof request)&&receive(socket_,&reply,sizeof reply)&&reply.magic==protocol) {
        if(reply.size==size&&receive(socket_,output,size))return true;
        if(reply.size==0){std::memset(output,0,size);return false;}
    }
    if(socket_>=0){close(socket_);socket_=-1;}
    std::memset(output,0,size);return false;
}
}
