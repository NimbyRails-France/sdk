#pragma once
#include <engine/physical_view.h>
#include <array>
#include <cstring>
#include <vector>

namespace nimby::windows::automatic {
// Geometry captured from the game's own route traversal, never reconstructed
// from a straight-line graph or a reservation. Occupation is NOT cached here:
// the integration callback queries it again before using this route prefix.
// Native pointers and layout checks belong only to this Windows adapter.
class PhysicalRoute {
    struct Section {uintptr_t address;uint64_t id;double from,to,metric,offset,length;};
    std::vector<Section> sections_;
    std::array<unsigned char,0xc0> path_{}; // Motion -> Drive -> Path, qualified binary.
    std::vector<uint64_t> pathIds_;
    uintptr_t motion_=0,pathBegin_=0;
    uint64_t train_=0;
    double head_=0,covered_=0;
    bool valid_=false;
    template<class T> static T field(const auto& bytes,size_t offset){T value{};std::memcpy(&value,bytes.data()+offset,sizeof value);return value;}
    template<class Read> bool matches(Read&& read) const {
        std::array<unsigned char,0xc0> current{};uint64_t id{};uint8_t driving{};
        if(!valid_||!read(motion_,&id,sizeof id)||id!=train_||
           !read(motion_+0x4b0,&driving,sizeof driving)||driving!=1||
           !read(motion_+0x290,current.data(),current.size())||current!=path_)return false;
        // A path can be edited in place without reallocating its vector. Compare
        // its full IDs too; do not keep a former branch merely because the head
        // still occupies a common upstream section. Stack storage stays bounded.
        std::array<uint64_t,64> chunk{};
        for(size_t i=0;i<pathIds_.size();i+=chunk.size()){
            const auto count=std::min(chunk.size(),pathIds_.size()-i);
            if(!read(pathBegin_+i*sizeof(uint64_t),chunk.data(),count*sizeof(uint64_t))||
               !std::equal(chunk.begin(),chunk.begin()+count,pathIds_.begin()+i))return false;
        }
        return true;
    }
public:
    void clear(){valid_=false;sections_.clear();pathIds_.clear();}
    template<class Read> void begin(uintptr_t motion,uint64_t train,double head,Read&& read){
        clear();motion_=motion;train_=train;head_=head;covered_=0;
        if(!std::isfinite(head)||train>>48!=5||!read(motion+0x290,path_.data(),path_.size())||path_[0x90]!=1)return;
        const auto begin=field<uintptr_t>(path_,0xa8),end=field<uintptr_t>(path_,0xb0),cap=field<uintptr_t>(path_,0xb8);
        if(begin<0x10000||end<=begin||cap<end||(end-begin)%8||end-begin>16384*8||cap-begin>65536*8)return;
        pathBegin_=begin;pathIds_.resize((end-begin)/8);
        if(!read(begin,pathIds_.data(),end-begin)||
           std::any_of(pathIds_.begin(),pathIds_.end(),[](auto id){return id>>48!=1;}))return;
        valid_=true;if(!matches(read))clear();
    }
    // Append only the prefix needed for simulated visibility. The final native
    // section can be longer: its metric still bounds later head displacement.
    template<class Read> void append(uintptr_t address,double from,double to,double length,double offset,Read&& read){
        if(!valid_||offset>=200)return;
        uint64_t id{};double metric{};
        if(sections_.size()>=4096||!std::isfinite(from)||!std::isfinite(to)||from<0||from>1||to<0||to>1||
           !std::isfinite(length)||length<0||!std::isfinite(offset)||std::abs(offset-covered_)>.01||
           !read(address,&id,sizeof id)||id>>48!=1||!read(address+0x88,&metric,sizeof metric)||
           !std::isfinite(metric)||metric<=0||std::abs(length-std::abs(to-from)*metric)>.01){clear();return;}
        if(length>0)sections_.push_back({address,id,from,to,metric,offset,length});
        covered_=offset+length;
    }
    // Called synchronously on the native simulation thread. Query reads the
    // current physical occupancy and excludes this train. No wall-clock delay
    // can substitute for that read, and no unseen geometry extends clearance.
    template<class Read,class Query> nimby::engine::automatic::PhysicalView refresh(
        uintptr_t motion,double head,uint64_t now,Read&& read,Query&& query) const {
        using nimby::engine::automatic::PhysicalView;
        if(motion!=motion_||!std::isfinite(head)||head<head_-1e-6||!matches(read))return {};
        uint64_t headTrack{};double headFraction{};int8_t headDirection{};
        if(!read(motion+0x3a8,&headTrack,sizeof headTrack)||!read(motion+0x3b0,&headFraction,sizeof headFraction)||
           !read(motion+0x3b8,&headDirection,sizeof headDirection)||!std::isfinite(headFraction)||headFraction<0||headFraction>1)return {};
        const double travelled=std::max(0.0,head-head_);
        PhysicalView result{head,0,200,now,true};bool located=false;
        for(const auto& section:sections_){
            if(section.offset+section.length<=travelled)continue;
            const auto delta=std::max(0.0,travelled-section.offset);
            const auto direction=section.to>section.from?1:-1;
            double from=section.from+direction*delta/section.metric;
            if(!located){
                if(headTrack!=section.id||headDirection!=direction||
                   std::abs(headFraction-from)*section.metric>.01)return {};
                from=headFraction;located=true;
            }
            // Rounding at an endpoint must not turn the remaining range around.
            if(from<0||from>1||(section.to-from)*direction<0)return {};
            uint64_t id{};double metric{};
            if(!read(section.address,&id,sizeof id)||id!=section.id||
               !read(section.address+0x88,&metric,sizeof metric)||metric!=section.metric)return {};
            const double length=std::min(std::abs(section.to-from)*metric,200-result.covered);
            const double to=from+direction*length/metric;
            const auto free=query(section.address,from,to,length);
            if(!std::isfinite(free)||free<0||free>length)return {};
            if(free<length)result.free=std::min(result.free,result.covered+free);
            result.covered+=length;
            if(free<length||result.covered>=200)break;
        }
        if(!located||!matches(read))return {};
        return result;
    }
};
}
