#include <research/train_monitor_model.h>
#include <cassert>
#include <limits>
#include <span>
using namespace nimby::research::monitor;
struct Reader {
    std::vector<std::span<const unsigned char>> regions;
    bool read(uint64_t address,void* out,size_t length) const {
        for(auto region:regions){const auto base=reinterpret_cast<uint64_t>(region.data());
            if(address>=base&&address-base<=region.size()&&length<=region.size()-(address-base)){
                std::memcpy(out,region.data()+(address-base),length);return true;
            }
        }return false;
    }
};
int main(){
    double speed{};
    assert(parse_speed(L"30,5",speed)&&speed==30.5);
    for(const auto* text:{L"nan",L"inf",L"",L"601",L"-1",L"20oops"})assert(!parse_speed(text,speed));
    // Deliberately synthetic object offsets: the common traversal must not use
    // the Windows game stride or position of speed/presence fields.
    constexpr ObjectLayout layout{64,96,8,16,16,48};
    Bytes objects(2*layout.motionStride);
    auto put=[](auto& bytes,size_t at,auto value){std::memcpy(bytes.data()+at,&value,sizeof value);};
    put(objects,0,uint64_t(0x5000000000001));objects[8]=1;put(objects,16,12.5);
    std::array<uint64_t,1> blocks{reinterpret_cast<uint64_t>(objects.data())};
    std::array<unsigned char,48> header{};
    put(header,4,uint32_t(1));put(header,8,uint32_t(2));put(header,16,uint32_t(1));
    put(header,24,reinterpret_cast<uint64_t>(blocks.data()));
    put(header,32,reinterpret_cast<uint64_t>(blocks.data()+1));
    put(header,40,reinterpret_cast<uint64_t>(blocks.data()+1));
    Pool pool;assert(decode(header.data(),pool));
    pool.address=reinterpret_cast<uint64_t>(header.data());pool.header=header;
    Reader reader{{objects,header,{reinterpret_cast<const unsigned char*>(blocks.data()),sizeof blocks}}};
    std::vector<Item> rows;
    auto names=[](const Reader&,const unsigned char*,std::string&){return false;};
    assert(items(reader,pool,true,rows,layout,names)&&rows.size()==1&&rows[0].speed==12.5);
    put(objects,0,uint64_t(0x5000000010001));assert(!items(reader,pool,true,rows,layout,names));
    put(objects,0,uint64_t(0x5000000000001));put(objects,16,std::numeric_limits<double>::quiet_NaN());
    assert(!items(reader,pool,true,rows,layout,names));
    objects[8]=0;assert(items(reader,pool,true,rows,layout,names)&&!rows[0].present);
    header[8]^=1;assert(!items(reader,pool,true,rows,layout,names));
}
