#include <engine/driving_command.h>
#include <cassert>
#include <limits>
int main(){
    nimby::engine::DrivingCommand command;
    constexpr uint64_t train=0x5000000000001;
    assert(command.accept(train,1000,1100,20,0.8,101));
    assert(command.remaining==11&&command.active(train,1099,20));
    assert(!command.active(train+1,1000,20)&&!command.active(train,1100,20));
    assert(!command.active(train,1000,0)&&!command.active(train,1000,120001));
    double ceiling{},braking{};
    assert(command.parameters(100,100,2,30,3,ceiling,braking)&&ceiling==20&&braking==0.8);
    assert(command.parameters(0,100,2,10,0.5,ceiling,braking)&&ceiling==10&&braking==0.5);
    assert(!command.parameters(-1,100,2,10,0.5,ceiling,braking));
    assert(!command.parameters(0,0,2,10,0.5,ceiling,braking));
    assert(!command.accept(train,1000,1100,std::numeric_limits<double>::quiet_NaN(),0.8,100));
    assert(!command.accept(train,1000,1000,20,0.8,100));
    assert(!command.accept(train,1000,31001,20,0.8,100));
    assert(command.remaining==11); // Rejected input never replaces a live command.
    assert(!command.consume(21,20)&&command.remaining==11);
    assert(!command.consume(-1,20)&&command.remaining==11);
    assert(command.consume(10,20)&&command.remaining==1);
    assert(command.consume(1,20)&&!command.active(train,1001,20));
    // A consumer ceiling is not a national speed limit. Native constraints
    // still cap the effective speed even when the requested ceiling is higher.
    assert(command.accept(train,1000,1100,250,0.8,100));
    assert(command.parameters(0,100,2,40,1,ceiling,braking)&&ceiling==40);
}
