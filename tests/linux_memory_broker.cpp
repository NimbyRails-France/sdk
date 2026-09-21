#include "platform/linux/memory_broker.h"
#include <sys/wait.h>
#include <unistd.h>
#include <cassert>
#include <fstream>
#include <sstream>
#include <iostream>

uint64_t started(int pid) {
    std::ifstream input("/proc/"+std::to_string(pid)+"/stat");std::string line;std::getline(input,line);
    std::istringstream fields(line.substr(line.rfind(')')+2));std::string field;
    for(int i=3;i<=22;++i)fields>>field;
    return std::stoull(field);
}
int main() {
    using namespace nimby::platform::linux_os;
    uint64_t source=0xf123456789abcdefULL;
    int ready[2],finish[2];assert(pipe(ready)==0&&pipe(finish)==0);
    auto pid=fork();assert(pid>=0);
    if(pid==0) {
        close(ready[0]);close(finish[1]);
        {MemoryBroker server;char value=1;assert(write(ready[1],&value,1)==1);assert(read(finish[0],&value,1)==1);}
        _exit(0);
    }
    close(ready[1]);close(finish[0]);char value=0;assert(read(ready[0],&value,1)==1);
    auto survivor=std::make_unique<MemoryBrokerClient>(pid,started(pid));
    {
        MemoryBrokerClient client(pid,started(pid));uint64_t output=0;
        assert(client.read(reinterpret_cast<uint64_t>(&source),&output,sizeof output)&&output==source);
        sleep(3); // Observation can be paused beyond the socket transfer timeout.
        assert(client.read(reinterpret_cast<uint64_t>(&source),&output,sizeof output)&&output==source);
        assert(!client.read(0x10000,&output,sizeof output)&&output==0);
        assert(client.read(reinterpret_cast<uint64_t>(&source),&output,sizeof output)&&output==source);
        assert(!client.read(reinterpret_cast<uint64_t>(&source),&output,256*1024*1024+1));
        bool rejected=false;try{MemoryBrokerClient stale(pid,started(pid)+1);}catch(...){rejected=true;}assert(rejected);
    }
    assert(write(finish[1],&value,1)==1);int status=0;assert(waitpid(pid,&status,0)==pid&&WIFEXITED(status)&&WEXITSTATUS(status)==0);
    uint64_t afterExit=123;
    assert(!survivor->read(reinterpret_cast<uint64_t>(&source),&afterExit,sizeof afterExit)&&afterExit==0);
    close(ready[0]);close(finish[1]);
    std::cout<<"Read-only game broker: peer identity, stale process, bounds and invalid memory passed\n";
}
