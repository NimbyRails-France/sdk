#include "platform/linux/process.h"
#include <sys/wait.h>
#include <unistd.h>
#include <cassert>
#include <cstring>
#include <iostream>
int main() {
    using namespace nimby::platform::linux_os;
    Process self(getpid());const auto identity=identify(self.executable());
    assert(identity.elf64 && identity.sha256.size()==64 && identity.size>0);
    uint64_t source=0x123456789abcdef0ULL,copy=0;
    assert(self.read(reinterpret_cast<uint64_t>(&source),&copy,sizeof copy) && source==copy);
    assert(!self.read(0,&copy,sizeof copy));
    unsigned char magic[4]{};assert(self.read(self.image_base(),magic,4) && std::memcmp(magic,"\177ELF",4)==0);
    int ready[2],finish[2];assert(pipe(ready)==0 && pipe(finish)==0);
    auto pid=fork();assert(pid>=0);
    if(pid==0){close(ready[0]);close(finish[1]);char c=1;assert(write(ready[1],&c,1)==1);assert(read(finish[0],&c,1)==1);_exit(0);}
    close(ready[1]);close(finish[0]);char c{};assert(read(ready[0],&c,1)==1);
    Process child(pid);assert(child.read(reinterpret_cast<uint64_t>(&source),&copy,sizeof copy) && copy==source);
    assert(write(finish[1],&c,1)==1);int status=0;assert(waitpid(pid,&status,0)==pid && WIFEXITED(status));
    assert(!child.alive() && !child.read(reinterpret_cast<uint64_t>(&source),&copy,sizeof copy));
    close(ready[0]);close(finish[1]);std::cout<<"Linux process identity, bounded memory reads and exit checks passed\n";
}
