#include "platform/linux/process.h"
#include <elf.h>
#include <openssl/evp.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <array>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <memory>
#include <sstream>
#include <stdexcept>

namespace nimby::platform::linux_os {
namespace {
std::filesystem::path proc(int pid, const char* name) { return std::filesystem::path("/proc")/std::to_string(pid)/name; }
uint64_t start_time(int pid) {
    std::ifstream input(proc(pid,"stat")); std::string line; std::getline(input,line);
    const auto end=line.rfind(')'); if(end==std::string::npos)throw std::runtime_error("Processus absent");
    std::istringstream fields(line.substr(end+2)); std::string field;
    for(int number=3;number<=22;++number)if(!(fields>>field))throw std::runtime_error("Identité de processus invalide");
    return std::stoull(field);
}
}
BinaryIdentity identify(const std::filesystem::path& path) {
    std::ifstream file(path,std::ios::binary); if(!file)throw std::runtime_error("Binaire inaccessible");
    Elf64_Ehdr header{};file.read(reinterpret_cast<char*>(&header),sizeof header);
    if(file.gcount()!=sizeof header)throw std::runtime_error("En-tête binaire tronqué");
    BinaryIdentity result;
    result.elf64=std::memcmp(header.e_ident,ELFMAG,SELFMAG)==0 && header.e_ident[EI_CLASS]==ELFCLASS64 &&
        header.e_ident[EI_DATA]==ELFDATA2LSB && header.e_machine==EM_X86_64 && (header.e_type==ET_DYN || header.e_type==ET_EXEC);
    if(!result.elf64)throw std::runtime_error("ELF Linux x86-64 requis");
    file.clear();file.seekg(0);
    std::unique_ptr<EVP_MD_CTX,decltype(&EVP_MD_CTX_free)> context(EVP_MD_CTX_new(),EVP_MD_CTX_free);
    if(!context || EVP_DigestInit_ex(context.get(),EVP_sha256(),nullptr)!=1)throw std::runtime_error("SHA-256 indisponible");
    std::array<char,65536> block;
    while(file) {file.read(block.data(),block.size());auto n=file.gcount();if(n>0){result.size+=n;if(EVP_DigestUpdate(context.get(),block.data(),n)!=1)throw std::runtime_error("Erreur SHA-256");}}
    if(!file.eof())throw std::runtime_error("Lecture binaire interrompue");
    std::array<unsigned char,32> digest{};unsigned length=0;
    if(EVP_DigestFinal_ex(context.get(),digest.data(),&length)!=1 || length!=digest.size())throw std::runtime_error("Erreur SHA-256");
    std::ostringstream hex;hex<<std::hex<<std::setfill('0');for(auto byte:digest)hex<<std::setw(2)<<unsigned(byte);
    result.sha256=hex.str();return result;
}
Process::Process(int pid):pid_(pid),start_time_(0) {
    if(pid<=0)throw std::invalid_argument("PID positif requis");
    start_time_=start_time(pid);
    memory_=open(proc(pid,"mem").c_str(),O_RDONLY|O_CLOEXEC);
    if(!alive()){if(memory_>=0)close(memory_);memory_=-1;throw std::runtime_error("Processus remplacé pendant l'ouverture");}
    if(memory_<0)try{broker_=std::make_unique<MemoryBrokerClient>(pid,start_time_);}catch(...){}
}
Process::~Process(){if(memory_>=0)close(memory_);}
bool Process::alive() const noexcept {try{return start_time(pid_)==start_time_;}catch(...){return false;}}
std::filesystem::path Process::executable() const {if(!alive())throw std::runtime_error("Processus fermé ou remplacé");return std::filesystem::read_symlink(proc(pid_,"exe"));}
std::vector<Mapping> Process::mappings() const {
    if(!alive())throw std::runtime_error("Processus fermé ou remplacé");
    std::ifstream input(proc(pid_,"maps"));if(!input)throw std::runtime_error("Cartographie mémoire inaccessible");
    std::vector<Mapping> result;std::string line;
    while(std::getline(input,line)) {
        std::istringstream row(line);std::string range,permissions,offset,device,inode,path;
        if(!(row>>range>>permissions>>offset>>device>>inode))throw std::runtime_error("Cartographie mémoire invalide");
        std::getline(row,path);auto start=path.find_first_not_of(' ');path=start==std::string::npos?"":path.substr(start);
        auto split=range.find('-');if(split==std::string::npos)throw std::runtime_error("Plage mémoire invalide");
        result.push_back({std::stoull(range.substr(0,split),nullptr,16),std::stoull(range.substr(split+1),nullptr,16),
            std::stoull(offset,nullptr,16),permissions.find('r')!=std::string::npos,permissions.find('x')!=std::string::npos,path});
    }
    if(!alive())throw std::runtime_error("Processus remplacé pendant la lecture");
    return result;
}
uint64_t Process::image_base() const {
    const auto image=executable().string();for(const auto& map:mappings())if(map.path==image && map.offset==0)return map.begin;
    throw std::runtime_error("Image ELF absente de la cartographie mémoire");
}
bool Process::read(uint64_t address,void* output,size_t size) const noexcept {
    if(!output || !size || size>256*1024*1024 || address<0x10000 || address>INT64_MAX-size)return false;
    if(broker_)return broker_->read(address,output,size);
    // The descriptor refers to this address space, even after PID reuse/exec.
    // It stops returning data when that address space exits. Opening it performs
    // the kernel's ptrace access check; no global policy is changed.
    const auto got=memory_>=0?pread(memory_,output,size,static_cast<off_t>(address)):-1;
    if(got!=static_cast<ssize_t>(size)){std::memset(output,0,size);return false;}return true;
}
}
