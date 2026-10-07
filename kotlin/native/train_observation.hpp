#pragma once
#include <nimby/detail/observation_values.hpp>
#include <cstring>
#include <limits>
#include <array>
#include <string_view>

namespace nimby::kotlin::trainData {
inline constexpr int pageSize=32,integerFields=32,numberFields=5,nameBytes=257,textFields=5;
inline constexpr int64_t unknown=INT64_MIN;
inline constexpr double unavailable=std::numeric_limits<double>::quiet_NaN();
inline void name(char* out,std::string_view value){
    std::memset(out,0,nameBytes);std::memcpy(out,value.data(),std::min(value.size(),size_t(nameBytes-1)));
}
inline void stationName(const Snapshot& snapshot,Id id,char* out){
    const auto station=snapshot.getStationById(id);
    name(out,station?station->getName().value_or(""):"");
}
inline int64_t capturedAt(const Snapshot& snapshot){return std::chrono::duration_cast<Milliseconds>(snapshot.getCapturedAt().time_since_epoch()).count();}
// All joins use Snapshot's indexes. The fixed page ceiling bounds callback
// allocations and data copied across the Kotlin boundary, including huge maps.
inline int trains(const Snapshot& snapshot,int64_t* ints,int size,double* nums,int numCount,char* text,int textSize){
    if(size!=2+pageSize*integerFields||numCount!=pageSize*numberFields||textSize!=pageSize*textFields*nameBytes||!ints||!nums||!text)return NIMBY_INVALID_ARGUMENT;
    const auto rows=snapshot.getAllTrains();
    if(ints[0]<0||uint64_t(ints[0])>rows.size()||ints[1]<1||ints[1]>pageSize||uint64_t(ints[1])>rows.size()-size_t(ints[0]))return NIMBY_INVALID_ARGUMENT;
    const auto offset=size_t(ints[0]),count=size_t(ints[1]);
    for(size_t i=0;i<count;++i){
        auto* out=ints+2+i*integerFields;auto* number=nums+i*numberFields;auto* names=text+i*textFields*nameBytes;
        std::fill(out,out+integerFields,0);std::fill(number,number+numberFields,unavailable);std::memset(names,0,textFields*nameBytes);
        for(const auto at:{5,6,7,8,15,22,26,27,29})out[at]=-1;
        for(const auto at:{14,16,17,18,19,20,25,30})out[at]=unknown;
        const auto& train=rows[offset+i];const auto id=train.getId();out[0]=int64_t(id);name(names,train.getName());
        if(const auto tags=snapshot.getTagIdsForObject(id))out[29]=int64_t(tags->size());
        if(const auto metadata=snapshot.getTrainMetadataById(id))out[30]=metadata->getPredictedArrivalDelayUs().value_or(unknown);
        out[3]=train.isSpeedDefaulted();if(!out[3])number[1]=train.getSpeedMps().value_or(unavailable);
        if(const auto position=train.getPosition()){
            out[1]=int64_t(position->getTrackId());out[2]=position->getDirection();number[0]=position->getFraction();
            if(const auto track=snapshot.getTrackById(position->getTrackId())){
                out[28]=int64_t(track->getStationId().value_or(0));stationName(snapshot,uint64_t(out[28]),names+4*nameBytes);
            }
        }
        if(const auto service=snapshot.getTrainServiceById(id)){
            out[4]=1;out[5]=service->getStatus()?int64_t(*service->getStatus()):-1;out[6]=service->getAlert()?int64_t(*service->getAlert()):-1;
            out[7]=service->isHidden()?int64_t(*service->isHidden()):-1;out[8]=service->isOnNetwork()?int64_t(*service->isOnNetwork()):-1;
            out[9]=int64_t(service->getLocationTrackId().value_or(0));out[10]=int64_t(service->getLocationStationId().value_or(0));
            out[11]=int64_t(service->getLineId().value_or(0));out[12]=int64_t(service->getStopTrackId().value_or(0));out[13]=int64_t(service->getStopStationId().value_or(0));
            out[14]=service->getStopIndex()?int64_t(*service->getStopIndex()):unknown;out[15]=service->getLineKind().value_or(-1);
            out[16]=service->getGameEpochSeconds().value_or(unknown);out[17]=service->getGameTimeUs().value_or(unknown);
            out[18]=service->getArrivalTimeUs().value_or(unknown);out[19]=service->getDepartureTimeUs().value_or(unknown);out[20]=service->getDispatchTimeUs().value_or(unknown);
            out[27]=service->getMotionFlags()?int64_t(*service->getMotionFlags()):-1;
            number[2]=service->getArrivalRemainingSeconds().value_or(unavailable);number[3]=service->getDepartureRemainingSeconds().value_or(unavailable);number[4]=service->getDispatchRemainingSeconds().value_or(unavailable);
            name(names+nameBytes,service->getLineName().value_or(""));stationName(snapshot,uint64_t(out[10]),names+2*nameBytes);stationName(snapshot,uint64_t(out[13]),names+3*nameBytes);
        }
        if(const auto details=snapshot.getTrainDetailsById(id)){
            out[21]=details->getOrderIndex().has_value();out[22]=details->getPassengerCount().value_or(-1);
            out[23]=int64_t(details->getScheduleId().value_or(0));out[24]=int64_t(details->getShiftId().value_or(0));
            out[25]=details->getOrderIndex()?int64_t(*details->getOrderIndex()):unknown;out[26]=details->getOrderMode();
        }
    }
    return NIMBY_OK;
}
inline int planHeader(const Snapshot& snapshot,int64_t* ints,int size,int numCount,char* text,int textSize){
    if(size!=5||numCount||textSize!=nameBytes||!ints||!text||uint64_t(ints[0])>>48!=5)return NIMBY_INVALID_ARGUMENT;
    const auto plan=snapshot.getLineStopsForTrain(uint64_t(ints[0]));
    ints[1]=-1;ints[2]=0;ints[3]=-1;ints[4]=capturedAt(snapshot);name(text,"");
    if(!plan)return NIMBY_OK;
    const auto service=snapshot.getTrainServiceById(uint64_t(ints[0]));
    const auto line=service?service->getLineId():std::nullopt;
    if(!line)return NIMBY_OK;
    ints[1]=int64_t(plan->size());ints[2]=int64_t(*line);ints[3]=service->getLineKind().value_or(-1);name(text,service->getLineName().value_or(""));return NIMBY_OK;
}
inline int stops(const Snapshot& snapshot,int64_t* ints,int size,int numCount,char* text,int textSize){
    if(size!=3+pageSize*5||numCount||textSize!=pageSize*nameBytes||!ints||!text||uint64_t(ints[0])>>48!=5)return NIMBY_INVALID_ARGUMENT;
    const auto plan=snapshot.getLineStopsForTrain(uint64_t(ints[0]));
    if(!plan)return NIMBY_DATA_UNAVAILABLE;
    if(ints[1]<0||uint64_t(ints[1])>plan->size()||ints[2]<1||ints[2]>pageSize||uint64_t(ints[2])>plan->size()-size_t(ints[1]))return NIMBY_INVALID_ARGUMENT;
    const auto offset=size_t(ints[1]),count=size_t(ints[2]);
    for(size_t i=0;i<count;++i){
        const auto& stop=(*plan)[offset+i];auto* out=ints+3+i*5;
        out[0]=stop.getIndex();out[1]=int64_t(stop.getTrackId());out[2]=int64_t(stop.getStationId());
        out[3]=stop.getArrivalOffsetSeconds()?int64_t(*stop.getArrivalOffsetSeconds()):unknown;out[4]=stop.getDepartureOffsetSeconds()?int64_t(*stop.getDepartureOffsetSeconds()):unknown;
        stationName(snapshot,stop.getStationId(),text+i*nameBytes);
    }
    return NIMBY_OK;
}
inline int catalogue(const Snapshot& snapshot,int op,int64_t* ints,int size,int numCount,char* text,int textSize){
    if(numCount||!ints)return NIMBY_INVALID_ARGUMENT;
    if(op==24){
        if(size!=2||textSize)return NIMBY_INVALID_ARGUMENT;
        const auto lines=snapshot.getAllLines();const auto tags=snapshot.getAllTags();
        ints[0]=lines?int64_t(lines->size()):-1;ints[1]=tags?int64_t(tags->size()):-1;return NIMBY_OK;
    }
    if(op==27){
        if(textSize||size<2||ints[0]<1||ints[0]>pageSize||size<1+ints[0])return NIMBY_INVALID_ARGUMENT;
        size_t tagsCount=0;const auto count=size_t(ints[0]);
        std::array<std::span<const Id>,pageSize> tags{};
        for(size_t i=0;i<count;++i){
            const auto values=snapshot.getTagIdsForObject(uint64_t(ints[1+i]));if(!values)return NIMBY_DATA_UNAVAILABLE;
            if(values->size()>4096-tagsCount)return NIMBY_RESOURCE_LIMIT;
            tagsCount+=values->size();tags[i]=*values;
        }
        if(size_t(size)!=1+count+tagsCount)return NIMBY_INVALID_ARGUMENT;
        auto at=1+count;for(size_t i=0;i<count;++i)for(const auto id:tags[i])ints[at++]=int64_t(id);
        return NIMBY_OK;
    }
    if((op!=25&&op!=26)||size!=(op==25?2+pageSize*6:2+pageSize)||textSize!=pageSize*nameBytes||!text||ints[0]<0||ints[1]<1||ints[1]>pageSize)return NIMBY_INVALID_ARGUMENT;
    const auto offset=size_t(ints[0]),count=size_t(ints[1]);
    if(op==25){
        const auto lines=snapshot.getAllLines();if(!lines)return NIMBY_DATA_UNAVAILABLE;
        if(offset>lines->size()||count>lines->size()-offset)return NIMBY_INVALID_ARGUMENT;
        for(size_t i=0;i<count;++i){
            const auto& line=(*lines)[offset+i];auto* out=ints+2+i*6;
            out[0]=int64_t(line.getId());out[1]=int64_t(line.getParentId().value_or(0));out[2]=line.hasParentInformation();out[3]=line.getKind().value_or(-1);
            const auto tags=snapshot.getTagIdsForObject(line.getId());out[4]=tags?int64_t(tags->size()):-1;
            out[5]=line.getName().has_value();name(text+i*nameBytes,line.getName().value_or(""));
        }
    }else{
        const auto tags=snapshot.getAllTags();if(!tags)return NIMBY_DATA_UNAVAILABLE;
        if(offset>tags->size()||count>tags->size()-offset)return NIMBY_INVALID_ARGUMENT;
        for(size_t i=0;i<count;++i){const auto& tag=(*tags)[offset+i];ints[2+i]=int64_t(tag.getId());name(text+i*nameBytes,tag.getName());}
    }
    return NIMBY_OK;
}
inline int characteristics(const Snapshot& snapshot,int64_t* ints,int size,double* nums,int numCount,int textSize){
    if(size!=2+pageSize*6||numCount!=pageSize*12||textSize||!ints||!nums||ints[0]<0||ints[1]<1||ints[1]>pageSize)return NIMBY_INVALID_ARGUMENT;
    const auto trains=snapshot.getAllTrains();const auto offset=size_t(ints[0]),count=size_t(ints[1]);
    if(offset>trains.size()||count>trains.size()-offset)return NIMBY_INVALID_ARGUMENT;
    for(size_t i=0;i<count;++i){
        auto* out=ints+2+i*6;auto* values=nums+i*12;
        std::fill(out,out+6,-1);std::fill(values,values+12,unavailable);
        const auto metadata=snapshot.getTrainMetadataById(trains[offset+i].getId());if(!metadata)continue;
        auto copy=[](const NimbyTrainCharacteristics& source,int64_t* integers,double* numbers){
            if(source.flags&NIMBY_CHARACTERISTICS_CAR_COUNT_VALID)integers[0]=source.car_count;
            if(source.flags&NIMBY_CHARACTERISTICS_CAPACITY_VALID)integers[1]=source.passenger_capacity;
            integers[2]=(source.flags&NIMBY_CHARACTERISTICS_COMPOSITION_VALID)?1:0;
            const double values[]{source.maximum_speed_mps,source.length_m,source.empty_mass_kg,source.maximum_acceleration_mps2,source.power_w,source.tractive_force_n};
            constexpr uint32_t flags[]{NIMBY_CHARACTERISTICS_MAX_SPEED_VALID,NIMBY_CHARACTERISTICS_LENGTH_VALID,NIMBY_CHARACTERISTICS_EMPTY_MASS_VALID,NIMBY_CHARACTERISTICS_ACCELERATION_VALID,NIMBY_CHARACTERISTICS_POWER_VALID,NIMBY_CHARACTERISTICS_TRACTIVE_FORCE_VALID};
            for(size_t n=0;n<6;++n)if((source.flags&flags[n])&&std::isfinite(values[n]))numbers[n]=values[n];
        };
        copy(metadata->getConfiguredCharacteristics(),out,values);copy(metadata->getCurrentCharacteristics(),out+3,values+6);
    }
    return NIMBY_OK;
}
inline int compositions(const Snapshot& snapshot,int op,int64_t* ints,int size,int numCount,char* text,int textSize){
    if(numCount||!ints)return NIMBY_INVALID_ARGUMENT;
    if(op==29){
        if(size!=2||textSize)return NIMBY_INVALID_ARGUMENT;
        const auto vehicles=snapshot.getAllTrainVehicles();const auto models=snapshot.getAllVehicleModels();
        ints[0]=vehicles?int64_t(vehicles->size()):-1;ints[1]=models?int64_t(models->size()):-1;return NIMBY_OK;
    }
    if((op!=30&&op!=31)||size!=(op==30?2+pageSize*4:2+pageSize)||textSize!=(op==30?0:pageSize*3*nameBytes)||(!text&&textSize)||ints[0]<0||ints[1]<1||ints[1]>pageSize)return NIMBY_INVALID_ARGUMENT;
    const auto offset=size_t(ints[0]),count=size_t(ints[1]);
    if(op==30){
        const auto vehicles=snapshot.getAllTrainVehicles();if(!vehicles)return NIMBY_DATA_UNAVAILABLE;
        if(offset>vehicles->size()||count>vehicles->size()-offset)return NIMBY_INVALID_ARGUMENT;
        for(size_t i=0;i<count;++i){
            const auto& row=(*vehicles)[offset+i];auto* out=ints+2+i*4;
            out[0]=int64_t(row.getTrainId());out[1]=int64_t(row.getModelId());out[2]=row.getIndex();out[3]=row.getComposition();
        }
    }else{
        const auto models=snapshot.getAllVehicleModels();if(!models)return NIMBY_DATA_UNAVAILABLE;
        if(offset>models->size()||count>models->size()-offset)return NIMBY_INVALID_ARGUMENT;
        for(size_t i=0;i<count;++i){
            const auto& row=(*models)[offset+i];ints[2+i]=int64_t(row.getId());auto* names=text+i*3*nameBytes;
            name(names,row.getCode());name(names+nameBytes,row.getNameEnglish());name(names+2*nameBytes,row.getSourceName());
        }
    }
    return NIMBY_OK;
}
}
