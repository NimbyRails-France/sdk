#include "loader/mods.h"
#include <nimby/detail/native_library.hpp>
#include <cassert>
#include <fstream>
#include <iostream>
#include <unistd.h>
namespace { unsigned errors=0; void log(const char* message){if(std::string(message).starts_with("ERROR:"))++errors;} }
int main(int argc,char** argv) {
    assert(argc==3);
    namespace fs=std::filesystem;
    namespace native=nimby::detail::native;
    auto temporary=(fs::temp_directory_path()/"nrf-mods-XXXXXX").string();
    assert(mkdtemp(temporary.data()));
    const fs::path root(temporary);
    auto manifest=[&](const char* directory,const char* text) {
        fs::create_directory(root/directory);
        std::ofstream(root/directory/"nrf-mod.ini")<<text;
    };
    manifest("valid","[NRFMod]\r\nlibrary = Fixture.so\r\n");
    fs::copy_file(argv[1],root/"valid/Fixture.so");
    manifest("escape","[NRFMod]\nlibrary=../Fixture.so\n");
    manifest("duplicate","[NRFMod]\nlibrary=A.so\nlibrary=B.so\n");
    manifest("windows","[NRFMod]\nlibrary=Fixture.dll\n");
    manifest("exports","[NRFMod]\nlibrary=Other.so\n");
    fs::copy_file(argv[2],root/"exports/Other.so");
    fs::create_directory(root/"ordinary-assets"); // Not a native mod.
    auto fixture=native::load(root/"valid/Fixture.so");
    auto starts=reinterpret_cast<uint32_t(*)()>(native::symbol(fixture,"Fixture_Starts"));
    auto stops=reinterpret_cast<uint32_t(*)()>(native::symbol(fixture,"Fixture_Stops"));
    assert(starts&&stops);
    nimby::loader::Mods mods;
    mods.start(root,log);assert(starts()==1&&stops()==0&&errors==4);
    mods.start(root,log);assert(starts()==1&&errors==4);
    assert(mods.stop(log));assert(stops()==1);
    assert(mods.stop(log));assert(stops()==1);
    mods.start(root,log);assert(starts()==2);
    assert(mods.stop(log));assert(stops()==2);
    native::unload(fixture);
    fs::remove_all(root);
    std::cout<<"Linux mod manifest validation, start, stop and restart passed\n";
}
