#pragma once
#include <nimby/detail/signal_routes.h>
#include <nimby/detail/track_fraction.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <optional>
#include <span>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace nimby::windows::automatic {
// Qualification diagnostics, never movement permissions. "Unqualified"
// deliberately covers both an unavailable read and a differing native value.
enum class RouteCopyReason : uint32_t {
    Ready, NoSelectedBoundary, NoScan, InvalidatedScan, Expired, CaptureClockUnqualified,
    PathUnqualified, TopologyUnqualified, HeadUnqualified,
    BoundaryUnqualified, ReadbackUnqualified, Count
};
// Only SDK-managed trains pay for this witness. Geometry is supplied by native
// scan callbacks; readback proves that the path, topology, incarnation and
// current head still match. In-place edits invalidate the witness even without
// reallocation. Track offsets are qualified by the Windows119 native profile.
class SignalRoute {
    struct Section {uintptr_t address;uint64_t track;double from,to,offset,length,metric;int direction;};
    struct Boundary {uintptr_t address;uint64_t signal,track;double fraction,position;int8_t direction;};
    struct Object {uintptr_t address;uint64_t id,track;double fraction,position;int32_t kind;int8_t storedDirection;
        bool unconditionalBalise=false;std::array<unsigned char,32> filter{};};
    struct Admission {uint64_t signal,time;bool clear,stageObserved;uint32_t crossingFlags;};
    struct Topology {
        uintptr_t address;uint64_t track;double metric;
        std::array<unsigned char,0x18> identity{};
        std::array<unsigned char,0x30> attachment{};
        std::vector<uint64_t> children;
    };
    static constexpr size_t maxChildren=4096,maxChildReferences=131072,idsPerRead=512;
    static constexpr uintptr_t addressLimit=0x7fffffff0000ULL;
    uintptr_t motion_=0,begin_=0;
    uint64_t train_=0,revision_=0,captured_=0;
    double head_=0,covered_=0;
    uint64_t headTrack_=0;
    double headFraction_=0;
    int8_t headDirection_=0;
    bool valid_=false,scanAttempted_=false;
    std::array<unsigned char,0xc0> path_{};
    std::vector<uint64_t> pathIds_;
    std::vector<Section> sections_;
    std::vector<Boundary> boundaries_;
    std::vector<Object> objects_;
    bool objectsValid_=true,objectsEnabled_=true;
    std::vector<Admission> admissions_;
    std::vector<Topology> topology_;
    std::unordered_map<uint64_t,size_t> topologyIndex_;
    size_t childReferences_=0;
    template<class T> static T field(const auto& bytes,size_t offset){T value{};std::memcpy(&value,bytes.data()+offset,sizeof value);return value;}
    static bool pointer(uintptr_t address){return address>=0x10000&&address<addressLimit&&address%8==0;}
    template<class Read> static bool readIds(uintptr_t address,std::span<uint64_t> ids,Read&& read){
        for(size_t i=0;i<ids.size();i+=idsPerRead){
            const auto count=std::min(idsPerRead,ids.size()-i);
            if(!read(address+i*sizeof(uint64_t),ids.data()+i,count*sizeof(uint64_t)))return false;
        }
        return true;
    }
    template<class Read> static bool sameTopology(const Topology& row,Read&& read){
        std::array<unsigned char,0x18> identity{};std::array<unsigned char,0x30> attachment{};double metric{};
        const auto sameRecord=[&]{
            return read(row.address,identity.data(),identity.size())&&identity==row.identity&&
                read(row.address+0x3f0,attachment.data(),attachment.size())&&attachment==row.attachment&&
                read(row.address+0x88,&metric,sizeof metric)&&metric==row.metric;
        };
        if(!sameRecord())return false;
        std::array<uint64_t,idsPerRead> ids{};
        const auto begin=field<uintptr_t>(row.attachment,0x18);
        for(size_t i=0;i<row.children.size();i+=ids.size()){
            const auto count=std::min(ids.size(),row.children.size()-i);
            if(!read(begin+i*sizeof(uint64_t),ids.data(),count*sizeof(uint64_t))||
               !std::equal(ids.begin(),ids.begin()+count,row.children.begin()+i))return false;
        }
        // An unchanged descriptor before a dependent array is insufficient:
        // the game may edit the attachment or recycle the record during reads.
        return row.children.empty()||sameRecord();
    }
    template<class Read> bool sameTopology(Read&& read)const{
        for(const auto& row:topology_)if(!sameTopology(row,read))return false;
        return true;
    }
    template<class Read> static bool sameBoundary(const Boundary& row,Read&& read){
        uint64_t id{},track{};double fraction{};int8_t direction{};int32_t kind{};
        return read(row.address,&id,sizeof id)&&id==row.signal&&
            read(row.address+0x40,&track,sizeof track)&&track==row.track&&
            read(row.address+0x48,&fraction,sizeof fraction)&&fraction==row.fraction&&
            read(row.address+0x50,&direction,sizeof direction)&&direction==-row.direction&&
            read(row.address+0x30,&kind,sizeof kind)&&kind==NIMBY_SIGNAL_PATH;
    }
    template<class Read> bool sameBoundaries(Read&& read)const{
        for(const auto& row:boundaries_)if(!sameBoundary(row,read))return false;
        return true;
    }
    template<class Read> static bool sameObject(const Object& row,Read&& read){
        uint64_t id{},track{};double fraction{};int8_t direction{};int32_t kind{};
        const bool anchor=read(row.address,&id,sizeof id)&&id==row.id&&
            read(row.address+0x40,&track,sizeof track)&&track==row.track&&
            read(row.address+0x48,&fraction,sizeof fraction)&&fraction==row.fraction&&
            read(row.address+0x50,&direction,sizeof direction)&&direction==row.storedDirection&&
            read(row.address+0x30,&kind,sizeof kind)&&kind==row.kind;
        if(!anchor||!row.unconditionalBalise)return anchor;
        std::array<unsigned char,32> filter{};
        return read(row.address+0x70,filter.data(),filter.size())&&filter==row.filter;
    }
    void rememberObject(Object row){
        if(!objectsValid_)return;
        if(objects_.size()>=256||(!objects_.empty()&&row.position<objects_.back().position)||
           std::any_of(objects_.begin(),objects_.end(),[&](const auto& old){return old.id==row.id;})){
            objectsValid_=false;return;
        }
        objects_.push_back(row);
    }
    template<class Read> bool captureTopology(uintptr_t address,uint64_t id,double metric,Read&& read){
        if(const auto found=topologyIndex_.find(id);found!=topologyIndex_.end()){
            const auto& row=topology_[found->second];return row.address==address&&row.metric==metric;
        }
        if(!pointer(address)||address>addressLimit-0x420)return false;
        Topology row{address,id,metric,{},{},{}};
        if(!read(address,row.identity.data(),row.identity.size())||field<uint64_t>(row.identity,0)!=id||
           !read(address+0x3f0,row.attachment.data(),row.attachment.size()))return false;
        const auto linkA=field<uint64_t>(row.identity,8),linkB=field<uint64_t>(row.identity,0x10);
        const auto reference=[&](uint64_t value){return !value||(value>>48==1&&value!=id);};
        const auto parent=field<uint64_t>(row.attachment,0);
        if(!reference(linkA)||!reference(linkB)||!reference(parent))return false;
        if(parent){
            const auto fraction=field<double>(row.attachment,8);const auto direction=field<int32_t>(row.attachment,0x10);
            if((linkA==0)==(linkB==0)||!std::isfinite(fraction)||fraction<0||fraction>1||
               (direction!=1&&direction!=-1))return false;
        }
        const auto begin=field<uintptr_t>(row.attachment,0x18),end=field<uintptr_t>(row.attachment,0x20),cap=field<uintptr_t>(row.attachment,0x28);
        // Native empty storage may be null or retained. Every nonempty list
        // must have aligned, bounded storage before any dependent memory read.
        if(end<begin||cap<end||cap-begin>maxChildren*sizeof(uint64_t)||(end-begin)%8||(cap-begin)%8||
           (cap&&(!pointer(begin)||cap>=addressLimit)))return false;
        const auto count=static_cast<size_t>((end-begin)/sizeof(uint64_t));
        if(count>maxChildReferences-childReferences_)return false;
        row.children.resize(count);
        if(!readIds(begin,row.children,read))return false;
        std::unordered_set<uint64_t> unique;unique.reserve(count);
        for(auto child:row.children)if(child>>48!=1||child==id||!unique.insert(child).second)return false;
        if(!sameTopology(row,read))return false;
        childReferences_+=count;topologyIndex_.emplace(id,topology_.size());topology_.push_back(std::move(row));
        return true;
    }
    template<class Read> bool samePath(Read&& read)const {
        std::array<unsigned char,0xc0> path{};uint64_t id{};uint8_t driving{};
        if(!valid_||!read(motion_,&id,sizeof id)||id!=train_||
           !read(motion_+0x4b0,&driving,sizeof driving)||driving!=1||
           !read(motion_+0x290,path.data(),path.size())||path!=path_)return false;
        std::array<uint64_t,512> ids;
        for(size_t i=0;i<pathIds_.size();i+=ids.size()){
            const auto count=std::min(ids.size(),pathIds_.size()-i);
            if(!read(begin_+i*8,ids.data(),count*8)||!std::equal(ids.begin(),ids.begin()+count,pathIds_.begin()+i))return false;
        }
        return true;
    }
public:
    struct PhysicalPrefixSection {uintptr_t address;uint64_t track;double from,to,length,offset;};
    // Private adapter witness for deferred driving-memory commits. It carries
    // no permission and must not leave the synchronous native step. The source
    // address is used only for qualified readback; IDs are compared in full.
    struct CommitSourceStamp {
        uintptr_t address{},trackAddress{};uint64_t signal{},track{};double fraction{},position{};
        int8_t direction{},orientation{},trackOrientation{};
    };
    template<class Read> bool captureCommitSources(std::span<const uint64_t> selected,
        std::span<const double> positions,std::span<CommitSourceStamp> output,Read&& read)const {
        if(selected.size()!=positions.size()||output.size()!=selected.size()||selected.size()>128||
           !samePath(read)||!sameTopology(read))return false;
        for(size_t i=0;i<selected.size();++i){
            const auto source=std::find_if(boundaries_.begin(),boundaries_.end(),
                [&](const auto& row){return row.signal==selected[i];});
            if(source==boundaries_.end()||!std::isfinite(source->position)||!std::isfinite(positions[i])||
               std::abs(source->position-positions[i])>1e-6||!sameBoundary(*source,read))return false;
            int8_t orientation{};
            if(!read(source->address+0x51,&orientation,sizeof orientation))return false;
            // Native ranges are already in the current track's frame. Do not
            // silently use a raw anchor that needs an unqualified inversion.
            const auto section=std::find_if(sections_.begin(),sections_.end(),
                [&](const auto& row){return row.track==source->track;});
            int8_t trackOrientation{};
            if(section==sections_.end()||!read(section->address+0x2c,&trackOrientation,sizeof trackOrientation)||
               (orientation&&orientation!=trackOrientation))return false;
            output[i]={source->address,section->address,source->signal,source->track,source->fraction,source->position,
                source->direction,orientation,trackOrientation};
        }
        return matchesCommitSources(output,read);
    }
    template<class Read> bool matchesCommitSources(std::span<const CommitSourceStamp> sources,Read&& read)const {
        if(!samePath(read)||!sameTopology(read))return false;
        for(const auto& source:sources){
            Boundary boundary{source.address,source.signal,source.track,source.fraction,source.position,source.direction};
            int8_t orientation{},trackOrientation{};uint64_t track{};
            if(!sameBoundary(boundary,read)||!read(source.address+0x51,&orientation,sizeof orientation)||
               orientation!=source.orientation||!read(source.trackAddress,&track,sizeof track)||track!=source.track||
               !read(source.trackAddress+0x2c,&trackOrientation,sizeof trackOrientation)||trackOrientation!=source.trackOrientation)return false;
        }
        return samePath(read)&&sameTopology(read);
    }
    void invalidate()noexcept{valid_=false;scanAttempted_=true;admissions_.clear();}
    void invalidateObjects()noexcept{objectsValid_=false;}
    bool observesObjects()const noexcept{return objectsEnabled_;}
    uint64_t revision()const noexcept{return revision_;}
    template<class Read> void begin(uintptr_t motion,uint64_t train,double head,uint64_t now,Read&& read,bool observeObjects=true){
        // Admissions belong to the exact observed signal anchors. A moved or
        // reversed signal can leave the Path and track attachments unchanged;
        // requalify the previous boundaries before preserving their revision.
        const bool unchanged=motion==motion_&&train==train_&&samePath(read)&&sameTopology(read)&&sameBoundaries(read);
        if(!unchanged){++revision_;admissions_.clear();}
        valid_=false;scanAttempted_=true;sections_.clear();boundaries_.clear();objects_.clear();
        objectsEnabled_=observeObjects;objectsValid_=observeObjects;topology_.clear();topologyIndex_.clear();childReferences_=0;
        motion_=motion;train_=train;head_=head;covered_=0;captured_=now;
        if(!std::isfinite(head)||train>>48!=5)return;
        if(!read(motion+0x3a8,&headTrack_,sizeof headTrack_)||headTrack_>>48!=1||
           !read(motion+0x3b0,&headFraction_,sizeof headFraction_)||!std::isfinite(headFraction_)||headFraction_<0||headFraction_>1||
           !read(motion+0x3b8,&headDirection_,sizeof headDirection_)||(headDirection_!=1&&headDirection_!=-1))return;
        if(unchanged){valid_=true;return;}
        uint8_t driving{};
        if(!read(motion+0x4b0,&driving,sizeof driving)||driving!=1||
           !read(motion+0x290,path_.data(),path_.size())||path_[0x90]!=1)return;
        const auto begin=field<uintptr_t>(path_,0xa8),end=field<uintptr_t>(path_,0xb0),cap=field<uintptr_t>(path_,0xb8);
        if(!pointer(begin)||end<=begin||cap<end||cap>=addressLimit||(end-begin)%8||(cap-begin)%8||
           end-begin>16384*8||cap-begin>65536*8)return;
        begin_=begin;pathIds_.resize((end-begin)/8);
        if(!readIds(begin,pathIds_,read)||
           std::any_of(pathIds_.begin(),pathIds_.end(),[](auto id){return id>>48!=1;}))return;
        valid_=true;if(!samePath(read))invalidate();
    }
    template<class Read> void append(uintptr_t address,double from,double to,double length,double offset,Read&& read){
        if(!valid_)return;
        uint64_t id{};double metric{};
        if(sections_.size()>=4096||!pointer(address)||address>addressLimit-0x420||
           !std::isfinite(from)||!std::isfinite(to)||from<0||from>1||to<0||to>1||
           !std::isfinite(length)||length<0||!std::isfinite(offset)||offset<0||std::abs(offset-covered_)>.01||
           !read(address,&id,sizeof id)||id>>48!=1||!read(address+0x88,&metric,sizeof metric)||
           !std::isfinite(metric)||metric<=0||std::abs(length-std::abs(to-from)*metric)>.01||
           !captureTopology(address,id,metric,read)){invalidate();return;}
        const int direction=to>from?1:to<from?-1:
            offset==0&&id==headTrack_&&std::abs(from-headFraction_)<=nimby::detail::trackFractionRounding?headDirection_:0;
        // Zero-length native markers carry point evidence at a track endpoint.
        // They never become occupation ranges or prove clearance by themselves.
        sections_.push_back({address,id,from,to,offset,length,metric,direction});
        covered_=offset+length;
    }
    template<class Read> void boundary(uintptr_t address,uint64_t signal,double offset,Read&& read){
        if(!valid_)return;
        uint64_t identity{},track{};double fraction{};int8_t storedDirection{};int32_t kind{};
        if(signal>>48!=8||!std::isfinite(offset)||offset<0||offset>covered_+.01||boundaries_.size()>=128||
           (!boundaries_.empty()&&offset<=boundaries_.back().position-head_)||
           !read(address,&identity,sizeof identity)||identity!=signal||!read(address+0x30,&kind,sizeof kind)||kind!=NIMBY_SIGNAL_PATH||
           !read(address+0x40,&track,sizeof track)||track>>48!=1||!read(address+0x48,&fraction,sizeof fraction)||
           !std::isfinite(fraction)||fraction<0||fraction>1||!read(address+0x50,&storedDirection,sizeof storedDirection)||
           (storedDirection!=1&&storedDirection!=-1)||
           std::any_of(boundaries_.begin(),boundaries_.end(),[&](const auto& row){return row.signal==signal;})){
            invalidate();return;
        }
        const int8_t facing=-storedDirection;
        if(!sections_.empty()){auto& point=sections_.back();
            if(point.length==0&&point.offset==offset&&point.track==track&&point.from==fraction&&point.direction==0)point.direction=facing;
        }
        boundaries_.push_back({address,signal,track,fraction,head_+offset,facing});
        if(objectsEnabled_)rememberObject({address,signal,track,fraction,head_+offset,kind,storedDirection});
    }
    // Called ONLY for a signal-bearing record emitted by the existing native
    // traversal. Its record discriminator is distinct from the object's kind.
    // Non-PATH objects do not become block boundaries or driving instructions.
    template<class Read> void encounteredObject(uintptr_t address,uint64_t signal,double offset,Read&& read){
        if(!valid_||!objectsEnabled_||!objectsValid_)return;
        uint64_t identity{},track{};double fraction{};int8_t storedDirection{};int32_t kind{};
        if(!pointer(address)||address>addressLimit-0x58||signal>>48!=8||
           !(signal&0x0000ffffffffffffULL)||!std::isfinite(offset)||offset<0||offset>covered_+.01||
           !read(address,&identity,sizeof identity)||identity!=signal||
           !read(address+0x30,&kind,sizeof kind)||kind<0||kind>6||kind==NIMBY_SIGNAL_PATH||
           !read(address+0x40,&track,sizeof track)||track>>48!=1||!(track&0x0000ffffffffffffULL)||
           !read(address+0x48,&fraction,sizeof fraction)||!std::isfinite(fraction)||fraction<0||fraction>1||
           !read(address+0x50,&storedDirection,sizeof storedDirection)||(storedDirection!=1&&storedDirection!=-1)){
            objectsValid_=false;return;
        }
        Object object{address,signal,track,fraction,head_+offset,kind,storedDirection};
        if(kind==NIMBY_SIGNAL_BALISE){
            // Existing SDK signal-filter layout, already used by network.cpp.
            // No tag lookup, script callback or new native address is involved.
            if(read(address+0x70,object.filter.data(),object.filter.size())){
                const auto mode=field<uint32_t>(object.filter,0);
                const auto begin=field<uint64_t>(object.filter,8),end=field<uint64_t>(object.filter,16),cap=field<uint64_t>(object.filter,24);
                object.unconditionalBalise=mode==0&&begin==end&&cap>=end&&cap-begin<=32768&&
                    (begin==0?cap==0:pointer(begin));
            }
        }
        rememberObject(object);
    }
    // The generic stage is copied only when no longitudinal private result was
    // substituted. A specific original crossed refusal may be retained without
    // that generic stage. Neither observation grants an SDK route permission.
    template<class Read> void admission(uint64_t signal,bool clear,uint64_t now,Read&& read,
        uint32_t crossingFlags=0,bool stageObserved=true){
        if(!samePath(read)||!sameTopology(read))return;
        const uint32_t known=NIMBY_SIGNAL_ROUTE_CROSSING_OCCUPIED|NIMBY_SIGNAL_ROUTE_CROSSING_RESERVED;
        if((crossingFlags&~known)||crossingFlags==known||(clear&&crossingFlags))return;
        auto at=std::find_if(admissions_.begin(),admissions_.end(),[&](const auto& row){return row.signal==signal;});
        if(at!=admissions_.end())*at={signal,now,clear,stageObserved,crossingFlags};
        else if(admissions_.size()<128)admissions_.push_back({signal,now,clear,stageObserved,crossingFlags});
    }
    // Export only geometry actually visited by the current native scan. The
    // caller queries physical occupation for these exact ranges; geometry by
    // itself never proves that the distance before a signal is free. Discard
    // all emitted ranges/results if final readback returns nullopt.
    template<class Read,class EmitSection,class AfterPrefix> std::optional<double> withPrefix(
        uint64_t signal,double expectedSourcePosition,double expectedHead,uint64_t now,
        Read&& read,EmitSection&& emitSection,AfterPrefix&& afterPrefix)const {
        const auto source=std::find_if(boundaries_.begin(),boundaries_.end(),[&](const auto& row){return row.signal==signal;});
        if(signal>>48!=8||source==boundaries_.end()||!std::isfinite(expectedSourcePosition)||
           !std::isfinite(expectedHead)||std::abs(source->position-expectedSourcePosition)>1e-6||expectedHead-source->position>1e-6||
           now<captured_||now-captured_>1000||!samePath(read)||!sameTopology(read)||!sameBoundary(*source,read))return std::nullopt;
        uint64_t track{};double fraction{},head{};int8_t direction{};
        if(!read(motion_+0x3a8,&track,sizeof track)||!read(motion_+0x3b0,&fraction,sizeof fraction)||
           !read(motion_+0x3b8,&direction,sizeof direction)||!read(motion_+0x3c0,&head,sizeof head)||
           head!=expectedHead||head<head_||!std::isfinite(fraction)||fraction<0||fraction>1||
           (direction!=1&&direction!=-1))return std::nullopt;
        const double first=head-head_,last=source->position-head_;
        const bool contact=track==source->track&&direction==source->direction&&
            std::abs(fraction-source->fraction)<=nimby::detail::trackFractionRounding&&std::abs(head-source->position)<=1e-6;
        if(last>covered_)return std::nullopt;
        const auto at=std::find_if(sections_.begin(),sections_.end(),[&](const auto& row){
            return row.track==track&&row.direction==direction&&first>=row.offset&&first<=row.offset+row.length&&
                std::abs(row.from+direction*(first-row.offset)/row.metric-fraction)<=nimby::detail::trackFractionRounding;
        });
        auto end=std::lower_bound(sections_.begin(),sections_.end(),last,
            [](const auto& row,double offset){return row.offset+row.length<offset;});
        for(;end!=sections_.end()&&end->offset<=last;++end)
            if(end->track==source->track&&end->direction==source->direction&&last<=end->offset+end->length&&
               std::abs(end->from+source->direction*(last-end->offset)/end->metric-source->fraction)<=nimby::detail::trackFractionRounding)break;
        if(at==sections_.end()||end==sections_.end()||end->offset>last||(!contact&&end<at))return std::nullopt;
        // Equal path coordinates on the next track mean the source was passed,
        // not zero-distance contact with that source on its own oriented track.
        if((first==last&&!contact)||(first>last&&!contact))return std::nullopt;
        const auto visit=[&](auto&& emit)->std::optional<double>{
            if(contact)return 0.0;
            double cursor=first,total=0;
            for(auto row=at;row!=sections_.end()&&row<=end;++row){
                const double start=std::max(first,row->offset),finish=std::min(last,row->offset+row->length);
                if(finish<=start)continue; // Markers provide points, never clearance.
                if(start!=cursor||(row->direction!=1&&row->direction!=-1))return std::nullopt;
                double from=row->from+row->direction*(start-row->offset)/row->metric;
                double to=row->from+row->direction*(finish-row->offset)/row->metric;
                if(start==first&&row->track==track&&row->direction==direction)from=fraction;
                if(row==end&&finish==last)to=source->fraction;
                const double length=std::abs(to-from)*row->metric;
                if(!std::isfinite(from)||!std::isfinite(to)||from<0||from>1||to<0||to>1||
                   (to-from)*row->direction<=0||!std::isfinite(length)||length<=0)return std::nullopt;
                emit(PhysicalPrefixSection{row->address,row->track,from,to,length,total});
                total+=length;cursor=finish;
            }
            if(cursor!=last||!std::isfinite(total)||std::abs(total-(last-first))>.01)return std::nullopt;
            return total;
        };
        // Reject a missing interval before any physical query or destination
        // append. The second bounded pass visits the same immutable sections.
        if(!visit([](const auto&){}))return std::nullopt;
        const auto result=visit(emitSection);
        if(!result)return std::nullopt;
        // Occupation belongs inside this readback bracket. In particular an
        // empty source contact still runs it once, then rechecks the same Path,
        // topology, head and source. Never retain the callback result on false.
        afterPrefix(*result);
        uint64_t finalTrack{};double finalFraction{},finalHead{};int8_t finalDirection{};
        if(!samePath(read)||!read(motion_+0x3a8,&finalTrack,sizeof finalTrack)||finalTrack!=track||
           !read(motion_+0x3b0,&finalFraction,sizeof finalFraction)||finalFraction!=fraction||
           !read(motion_+0x3b8,&finalDirection,sizeof finalDirection)||finalDirection!=direction||
           !read(motion_+0x3c0,&finalHead,sizeof finalHead)||finalHead!=head||
           !sameTopology(read)||!sameBoundary(*source,read))return std::nullopt;
        return result;
    }
    template<class Read,class EmitSection> std::optional<double> copyPrefix(
        uint64_t signal,double expectedSourcePosition,double expectedHead,uint64_t now,
        Read&& read,EmitSection&& emitSection)const {
        return withPrefix(signal,expectedSourcePosition,expectedHead,now,std::forward<Read>(read),
            std::forward<EmitSection>(emitSection),[](double){});
    }
    template<class Read,class Emit,class EmitSection,class EmitAnnotation=std::nullptr_t> RouteCopyReason copyDiagnosed(std::span<const uint64_t> selected,uint32_t blocks,uint64_t now,
        Read&& read,Emit&& emit,EmitSection&& emitSection,EmitAnnotation emitAnnotation=nullptr)const {
        constexpr bool selectedRoute=!std::is_same_v<EmitAnnotation,std::nullptr_t>;
        if(!scanAttempted_)return RouteCopyReason::NoScan;
        if(!valid_)return RouteCopyReason::InvalidatedScan;
        if(std::none_of(boundaries_.begin(),boundaries_.end(),[&](const auto& row){return std::binary_search(selected.begin(),selected.end(),row.signal);}))
            return RouteCopyReason::NoSelectedBoundary;
        if(now<captured_)return RouteCopyReason::CaptureClockUnqualified;
        if(now-captured_>1000)return RouteCopyReason::Expired;
        if(!samePath(read))return RouteCopyReason::PathUnqualified;
        if(!sameTopology(read))return RouteCopyReason::TopologyUnqualified;
        uint64_t track{};double fraction{},head{};int8_t direction{};
        if(!read(motion_+0x3a8,&track,sizeof track)||!read(motion_+0x3b0,&fraction,sizeof fraction)||
           !read(motion_+0x3b8,&direction,sizeof direction)||!read(motion_+0x3c0,&head,sizeof head)||
           !std::isfinite(head)||head<head_||!std::isfinite(fraction)||fraction<0||fraction>1||
           (direction!=1&&direction!=-1))return RouteCopyReason::HeadUnqualified;
        const auto travelled=head-head_;
        const auto at=std::find_if(sections_.begin(),sections_.end(),[&](const auto& row){
            const int facing=row.direction;
            if(!facing)return false;
            const auto expected=row.from+facing*(travelled-row.offset)/row.metric;
            return row.track==track&&facing==direction&&travelled>=row.offset&&travelled<=row.offset+row.length&&
                std::abs(expected-fraction)<=nimby::detail::trackFractionRounding;
        });
        if(at==sections_.end())return RouteCopyReason::HeadUnqualified;
        // Recheck after head reads. A change yields no approach, never an old
        // branch or a guessed next boundary. The game may move between captures.
        if(!samePath(read))return RouteCopyReason::PathUnqualified;
        std::array<size_t,128> boundarySections{};size_t boundaryNumber=0;
        for(const auto& boundary:boundaries_){
            if(!sameBoundary(boundary,read))return RouteCopyReason::BoundaryUnqualified;
            const auto offset=boundary.position-head_;
            auto part=std::lower_bound(sections_.begin(),sections_.end(),offset,[](const auto& value,double distance){return value.offset+value.length<distance;});
            for(;part!=sections_.end()&&part->offset<=offset;++part){
                if(part->track==boundary.track&&part->direction==boundary.direction&&offset<=part->offset+part->length&&
                   std::abs(part->from+boundary.direction*(offset-part->offset)/part->metric-boundary.fraction)<=nimby::detail::trackFractionRounding)break;
            }
            if(part==sections_.end()||part->offset>offset)return RouteCopyReason::BoundaryUnqualified;
            boundarySections[boundaryNumber++]=static_cast<size_t>(part-sections_.begin());
        }
        std::array<int32_t,256> objectDirections{};
        if constexpr(selectedRoute){
            if(!objectsValid_)return RouteCopyReason::BoundaryUnqualified;
            for(size_t i=0;i<objects_.size();++i){const auto& object=objects_[i];
                if(!sameObject(object,read))return RouteCopyReason::BoundaryUnqualified;
                const double offset=object.position-head_;
                int32_t direction=0;
                for(const auto& part:sections_){
                    if(!part.direction||part.track!=object.track||offset<part.offset||offset>part.offset+part.length)continue;
                    const double expected=part.from+part.direction*(offset-part.offset)/part.metric;
                    if(std::abs(expected-object.fraction)>nimby::detail::trackFractionRounding)continue;
                    if(direction&&direction!=part.direction)return RouteCopyReason::BoundaryUnqualified;
                    direction=part.direction;
                }
                // A lone zero-length point cannot manufacture the travel sense
                // of a non-PATH object or a connected downstream interval.
                if(!direction)return RouteCopyReason::BoundaryUnqualified;
                objectDirections[i]=direction;
            }
        }
        uint32_t rank=0;
        for(size_t i=0;i<boundaries_.size();++i){const auto& row=boundaries_[i];
            const auto distance=row.position-head;
            if(distance<0&&(row.track!=track||row.direction!=direction||
                std::abs(row.fraction-fraction)>nimby::detail::trackFractionRounding))continue;
            if(distance==0&&boundarySections[i]<static_cast<size_t>(at-sections_.begin()))continue;
            if(++rank>blocks)break;
            if(!std::binary_search(selected.begin(),selected.end(),row.signal))continue;
            uint32_t flags=NIMBY_SIGNAL_ROUTE_ORDERED;
            const bool complete=i+1<boundaries_.size();
            if(complete)flags|=NIMBY_SIGNAL_ROUTE_BLOCK_COMPLETE;
            const auto check=std::find_if(admissions_.begin(),admissions_.end(),[&](const auto& value){return value.signal==row.signal;});
            if(check!=admissions_.end()&&now>=check->time&&now-check->time<=1000){
                if(check->stageObserved)flags|=check->clear?NIMBY_SIGNAL_ROUTE_NATIVE_CLEAR:NIMBY_SIGNAL_ROUTE_NATIVE_BLOCKED;
                flags|=check->crossingFlags;
            }
            emit(NimbySignalRouteObservation{train_,row.signal,i+1<boundaries_.size()?boundaries_[i+1].signal:0,
                revision_,captured_,track,fraction,std::max(0.0,distance),direction,rank,flags,0,
                row.track,row.fraction,row.direction,NIMBY_SIGNAL_PATH,
                complete?boundaries_[i+1].track:0,complete?boundaries_[i+1].fraction:0.0,
                complete?boundaries_[i+1].direction:0,complete?uint32_t{NIMBY_SIGNAL_PATH}:0u});
            const double first=row.position-head_,last=complete?boundaries_[i+1].position-head_:covered_;
            double cursor=first;
            auto section=std::lower_bound(sections_.begin(),sections_.end(),first,
                [](const auto& value,double distance){return value.offset+value.length<=distance;});
            for(;section!=sections_.end()&&section->offset<last;++section){
                const double start=std::max(first,section->offset),end=std::min(last,section->offset+section->length);
                if(end<=start)continue;
                if constexpr(selectedRoute)if(start!=cursor)return RouteCopyReason::BoundaryUnqualified;
                const int facing=section->direction;
                const double from=section->from+facing*(start-section->offset)/section->metric;
                const double to=section->from+facing*(end-section->offset)/section->metric;
                if constexpr(selectedRoute){
                    // Preserve the actual native endpoints when no clipping
                    // is needed, rather than adding another rounding error.
                    const double directedFrom=start==section->offset?section->from:from;
                    const double directedTo=end==section->offset+section->length?section->to:to;
                    emitSection(NimbySignalRouteSection{train_,row.signal,section->track,directedFrom,directedTo});
                }else
                    emitSection(NimbySignalRouteSection{train_,row.signal,section->track,std::min(from,to),std::max(from,to)});
                cursor=end;
            }
            if constexpr(selectedRoute){
                if(cursor!=last)return RouteCopyReason::BoundaryUnqualified;
                const auto source=std::find_if(objects_.begin(),objects_.end(),[&](const auto& value){return value.id==row.signal;});
                if(source==objects_.end())return RouteCopyReason::BoundaryUnqualified;
                uint32_t ordinal=0;bool firstBoundary=false;
                for(auto object=source+1;object!=objects_.end();++object){
                    if(object->position-head_>last)break;
                    const uint32_t kindFlag=object->kind==NIMBY_SIGNAL_PATH?NIMBY_SELECTED_ROUTE_PROTECTING_SIGNAL:
                        object->kind==NIMBY_SIGNAL_BALISE?NIMBY_SELECTED_ROUTE_NATIVE_BOUNDARY:0u;
                    const bool first=kindFlag&&!firstBoundary;
                    emitAnnotation(NimbySignalRouteAnnotation{train_,row.signal,object->id,object->track,
                        object->fraction,object->position-row.position,objectDirections[object-objects_.begin()],object->kind,
                        ordinal,kindFlag|(first?NIMBY_SELECTED_ROUTE_FIRST_BOUNDARY:0u)|
                            (object->unconditionalBalise?NIMBY_SELECTED_ROUTE_UNCONDITIONAL_BALISE:0u)});
                    firstBoundary|=kindFlag!=0;
                    ++ordinal;
                    if(object->kind==NIMBY_SIGNAL_PATH)break;
                }
            }
        }
        uint64_t finalTrack{};double finalFraction{},finalHead{};int8_t finalDirection{};
        const bool qualified=samePath(read)&&read(motion_+0x3a8,&finalTrack,sizeof finalTrack)&&finalTrack==track&&
            read(motion_+0x3b0,&finalFraction,sizeof finalFraction)&&finalFraction==fraction&&
            read(motion_+0x3b8,&finalDirection,sizeof finalDirection)&&finalDirection==direction&&
            read(motion_+0x3c0,&finalHead,sizeof finalHead)&&finalHead==head&&sameTopology(read)&&sameBoundaries(read);
        if(!qualified)return RouteCopyReason::ReadbackUnqualified;
        if constexpr(selectedRoute)for(const auto& object:objects_)if(!sameObject(object,read))return RouteCopyReason::ReadbackUnqualified;
        return RouteCopyReason::Ready;
    }
    template<class Read,class Emit,class EmitSection> bool copy(std::span<const uint64_t> selected,uint32_t blocks,uint64_t now,
        Read&& read,Emit&& emit,EmitSection&& emitSection)const {
        return copyDiagnosed(selected,blocks,now,std::forward<Read>(read),std::forward<Emit>(emit),
                            std::forward<EmitSection>(emitSection))==RouteCopyReason::Ready;
    }
};
}
