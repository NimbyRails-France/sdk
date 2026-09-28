#pragma once
#include <nimby/mod.hpp>
#include <nimby/detail/observation_session.hpp>
#include <nimby/detail/platform/host.hpp>
#include <limits>

namespace nimby::kotlin {
using ToolCall=int(*)(int,int64_t*,int,double*,int,char*,int);
// Scoped on the worker: foreign threads and retained Kotlin contexts cannot
// access a borrowed capture. The connection is explicitly released at Stop.
inline std::unique_ptr<detail::ObservationSession>& toolReader(){static auto* p=new std::unique_ptr<detail::ObservationSession>;return *p;}
struct ToolFrame {
    GameSession game;
    Snapshot::Ptr captured;
};
inline thread_local ToolFrame* currentTool=nullptr;
struct ToolScope {
    ToolFrame frame;
    ToolFrame* previous;
    explicit ToolScope(const GameSession& game):frame{game,{}},previous(currentTool){currentTool=&frame;}
    ~ToolScope(){currentTool=previous;}
};
inline detail::ObservationSession& toolConnection(){
    if(!toolReader())toolReader()=std::make_unique<detail::ObservationSession>(detail::platform::currentProcessId());
    return *toolReader();
}
inline int toolCall(int op,int64_t* ints,int size,double* nums,int numCount,char* text,int textSize)noexcept {
    try {
        if(!currentTool)return NIMBY_INVALID_HANDLE;
        if(size<0||numCount<0||textSize<0||size>4000000||numCount>1000000||textSize>8192||
           (!ints&&size)||(!nums&&numCount)||(!text&&textSize))return NIMBY_INVALID_ARGUMENT;
        auto& frame=*currentTool;
        if(op==1){
            if(size!=3)return NIMBY_INVALID_ARGUMENT;
            frame.captured=toolConnection().capture();
            if(!frame.captured->getGameSession()||*frame.captured->getGameSession()!=frame.game){frame.captured.reset();return NIMBY_DATA_UNAVAILABLE;}
            if(!frame.captured->getTrackMetrics())return NIMBY_DATA_UNAVAILABLE;
            ints[0]=static_cast<int64_t>(frame.captured->getAllTrackNodes().size());
            ints[1]=static_cast<int64_t>(frame.captured->getAllTrackJunctions().size());
            ints[2]=static_cast<int64_t>(frame.captured->getAllSignals().size());return NIMBY_OK;
        }
        if(op>=2&&op<=4){
            if(!frame.captured)return NIMBY_DATA_UNAVAILABLE;
            if(op==2){
                const auto rows=frame.captured->getAllTrackNodes();
                if(size!=int(rows.size()*3)||numCount!=int(rows.size()))return NIMBY_INVALID_ARGUMENT;
                std::unordered_map<uint64_t,double> lengths;
                for(const auto& row:*frame.captured->getTrackMetrics())lengths.emplace(row.track_id,row.length_m);
                for(size_t i=0;i<rows.size();++i){const auto& row=rows[i];
                    ints[i*3]=int64_t(row.getId());ints[i*3+1]=int64_t(row.getLinkAId().value_or(0));ints[i*3+2]=int64_t(row.getLinkBId().value_or(0));
                    const auto length=lengths.find(row.getId());nums[i]=length==lengths.end()?std::numeric_limits<double>::quiet_NaN():length->second;}
            }else if(op==3){
                const auto rows=frame.captured->getAllTrackJunctions();
                if(size!=int(rows.size()*4)||numCount!=int(rows.size()))return NIMBY_INVALID_ARGUMENT;
                for(size_t i=0;i<rows.size();++i){const auto& row=rows[i];ints[i*4]=int64_t(row.getBranchTrackId());ints[i*4+1]=int64_t(row.getMainTrackId());
                    ints[i*4+2]=row.getMainDirection();ints[i*4+3]=row.getBranchDirection();nums[i]=row.getMainFraction();}
            }else{
                const auto rows=frame.captured->getAllSignals();
                if(size!=int(rows.size()*4)||numCount!=int(rows.size()))return NIMBY_INVALID_ARGUMENT;
                for(size_t i=0;i<rows.size();++i){const auto& row=rows[i];ints[i*4]=int64_t(row.getId());ints[i*4+1]=int64_t(row.getTrackId());
                    ints[i*4+2]=row.getDirection();ints[i*4+3]=row.getKind();nums[i]=row.getFraction();}
            }
            return NIMBY_OK;
        }
        if(op==5){
            if(size<69||ints[0]<1||ints[0]>4||ints[3]<0||ints[3]>64)return NIMBY_INVALID_ARGUMENT;
            const auto action=uint32_t(ints[0]),count=uint32_t(ints[3]);
            if(numCount!=int(count)||size<int(4+2*count))return NIMBY_INVALID_ARGUMENT;
            NimbyConstructionRequest request{};request.size=sizeof request;request.version=1;
            request.action=action;request.token=uint64_t(ints[1]);request.source_signal=uint64_t(ints[2]);request.count=count;
            for(uint32_t i=0;i<count;++i)request.positions[i]={uint64_t(ints[4+i*2]),nums[i],int32_t(ints[5+i*2]),0};
            NimbyConstructionResult result{};result.size=sizeof result;result.version=1;
            const auto status=action==4?toolConnection().pollConstruction(request.token,result):toolConnection().construction(request,result);
            if(status!=NIMBY_OK)return status;
            if(result.size!=sizeof result||result.version!=1||result.count>64||result.state<1||result.state>6)return NIMBY_INTERNAL_ERROR;
            std::fill(ints,ints+size,0);ints[0]=result.state;ints[1]=int64_t(result.token);ints[2]=result.count;ints[3]=result.reason;ints[4]=result.can_undo;
            for(uint32_t i=0;i<result.count;++i)ints[5+i]=int64_t(result.ids[i]);return NIMBY_OK;
        }
        if(op==6){
            if(size<3||ints[2]<0||ints[2]>12||size!=3+ints[2]||!textSize)return NIMBY_INVALID_ARGUMENT;
            NimbyUiToolPanelV1 panel{};panel.size=sizeof panel;panel.version=1;panel.panel=uint64_t(ints[0]);panel.signal=uint64_t(ints[1]);panel.count=uint32_t(ints[2]);
            const char* cursor=text;const char* end=text+textSize;
            auto read=[&](auto& target){const auto zero=static_cast<const char*>(std::memchr(cursor,0,size_t(end-cursor)));
                if(!zero||size_t(zero-cursor)>=sizeof target)throw std::invalid_argument("Invalid tool panel text");
                std::memcpy(target,cursor,size_t(zero-cursor));cursor=zero+1;};
            read(panel.service);read(panel.origin);read(panel.message);
            for(uint32_t i=0;i<panel.count;++i){if(ints[3+i]<0||ints[3+i]>1)return NIMBY_INVALID_ARGUMENT;
                read(panel.buttons[i].id);read(panel.buttons[i].label);panel.buttons[i].enabled=uint32_t(ints[3+i]);}
            if(cursor!=end)return NIMBY_INVALID_ARGUMENT;
            return nimby::publishToolPanel(panel);
        }
        if(op==7){
            if(size!=1||ints[0]<0||ints[0]>2||textSize<2||textSize>4097||text[textSize-1]||std::memchr(text,0,textSize-1))return NIMBY_INVALID_ARGUMENT;
            const char* levels[]{"INFO","WARN","ERROR"};detail::diagnostics::write("mods",levels[ints[0]],text);return NIMBY_OK;
        }
        if(op==8){
            if(size<4||ints[2]<0||ints[2]>12||ints[3]<0||ints[3]>4||size!=4+ints[2]+4*ints[3]||!textSize)return NIMBY_INVALID_ARGUMENT;
            NimbyUiToolPanelV2 wire{};auto& p=wire.base;p.size=sizeof wire;p.version=2;p.panel=uint64_t(ints[0]);p.signal=uint64_t(ints[1]);p.count=uint32_t(ints[2]);wire.input_count=uint32_t(ints[3]);
            const char* cursor=text;const char* end=text+textSize;
            auto read=[&](auto& target){const auto zero=static_cast<const char*>(std::memchr(cursor,0,size_t(end-cursor)));
                if(!zero||size_t(zero-cursor)>=sizeof target)throw std::invalid_argument("Invalid tool input text");
                std::memcpy(target,cursor,size_t(zero-cursor));cursor=zero+1;};
            read(p.service);read(p.origin);read(p.message);
            for(uint32_t i=0;i<p.count;++i){if(ints[4+i]<0||ints[4+i]>1)return NIMBY_INVALID_ARGUMENT;
                read(p.buttons[i].id);read(p.buttons[i].label);p.buttons[i].enabled=uint32_t(ints[4+i]);}
            for(uint32_t i=0;i<wire.input_count;++i){auto& n=wire.inputs[i];const auto at=4+p.count+i*4;
                for(uint32_t k=0;k<3;++k)if(ints[at+k]<INT32_MIN||ints[at+k]>INT32_MAX)return NIMBY_INVALID_ARGUMENT;
                if(ints[at+3]<0||ints[at+3]>1)return NIMBY_INVALID_ARGUMENT;
                read(n.id);read(n.label);n.value=int32_t(ints[at]);n.minimum=int32_t(ints[at+1]);n.maximum=int32_t(ints[at+2]);n.enabled=uint32_t(ints[at+3]);}
            if(cursor!=end)return NIMBY_INVALID_ARGUMENT;
            return nimby::publishToolPanel(wire);
        }
        if(op==9){
            if(size<3||ints[2]<0||ints[2]>64||size!=3+2*ints[2]||numCount!=ints[2])return NIMBY_INVALID_ARGUMENT;
            NimbyUiSignalPreviewV1 wire{};wire.size=sizeof wire;wire.version=1;wire.panel=uint64_t(ints[0]);wire.signal=uint64_t(ints[1]);wire.count=uint32_t(ints[2]);
            if(wire.count){
                if(!textSize)return NIMBY_INVALID_ARGUMENT;
                const char* cursor=text;const char* end=text+textSize;
                auto read=[&](auto& target){const auto zero=static_cast<const char*>(std::memchr(cursor,0,size_t(end-cursor)));
                    if(!zero||zero==cursor||size_t(zero-cursor)>=sizeof target)throw std::invalid_argument("Invalid preview text");
                    std::memcpy(target,cursor,size_t(zero-cursor));cursor=zero+1;};
                read(wire.service);read(wire.origin);if(cursor!=end)return NIMBY_INVALID_ARGUMENT;
                for(uint32_t i=0;i<wire.count;++i){
                    if(ints[4+i*2]!=1&&ints[4+i*2]!=-1)return NIMBY_INVALID_ARGUMENT;
                    wire.positions[i]={uint64_t(ints[3+i*2]),nums[i],int32_t(ints[4+i*2]),0};
                }
            }else if(textSize)return NIMBY_INVALID_ARGUMENT;
            return nimby::publishSignalPreview(wire);
        }
        return NIMBY_INVALID_ARGUMENT;
    }catch(const nimby::Exception& e){return int(e.code());}
    catch(const std::invalid_argument&){return NIMBY_INVALID_ARGUMENT;}
    catch(...){detail::diagnostics::exception("mods","tool callback");return NIMBY_INTERNAL_ERROR;}
}
}
