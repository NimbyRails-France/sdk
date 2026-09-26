#include <nimby/detail/observation_session.hpp>
#include <cstdio>
#define CHECK(x) do {if(!(x)){std::fprintf(stderr,"FAIL %d: %s\n",__LINE__,#x);return 1;}}while(false)
int main() {
    NimbyDrivingObservation data{};
    CHECK(NimbyInternal_ReadTrainDriving(0,0,nullptr)==NIMBY_INVALID_ARGUMENT);
    CHECK(NimbyInternal_ReadTrainDriving(0,0,&data)==NIMBY_INVALID_ARGUMENT);
    data.struct_size=sizeof data;data.flags=0xffffffff;
    CHECK(NimbyInternal_ReadTrainDriving(0,0,&data)==NIMBY_INVALID_ARGUMENT&&data.flags==0);
    data.train_id=123;
    CHECK(NimbyInternal_ReadTrainDriving(0,0x5000000000001ULL,&data)==NIMBY_INVALID_HANDLE&&data.train_id==0);
    nimby::DrivingObservation missing{data};
    CHECK(!missing.getCurrentDynamics()&&!missing.getPurchasedDynamics()&&!missing.getSpeedMps()&&!missing.getPosition());
    data.flags=NIMBY_DRIVING_CURRENT_VALID;data.current.max_speed_mps=40;data.current.length_m=200;
    data.train.flags=NIMBY_TRAIN_SPEED_VALID;data.train.speed_mps=0;
    nimby::DrivingObservation stopped{data};
    CHECK(stopped.getCurrentDynamics()->lengthM==200&&!stopped.getPurchasedDynamics());
    CHECK(stopped.getSpeedMps()==0&&!stopped.isSpeedDefaulted());
    std::puts("PASS driving ABI validation and optional C++ values");
}
