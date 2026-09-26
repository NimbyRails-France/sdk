// Offline fixture editor. Uses the SDK's validation and atomic profile writer;
// never modifies a native game save or a running UI's in-memory settings.
#include <nimby/detail/signal_settings_file.hpp>
#include <tlhelp32.h>
#include <iostream>
#include <charconv>
#include <algorithm>

int main(int argc,char** argv) { try {
 if(argc<5 || (argc-3)%2) {
  std::cerr<<"Usage: signal-settings-profile PROFILE SIGNAL_ID FIELD 0|1 [FIELD 0|1 ...]\n";return 2;
 }
 const HANDLE processes=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
 if(processes==INVALID_HANDLE_VALUE)throw std::runtime_error("Cannot verify game is closed");
 PROCESSENTRY32W entry{};entry.dwSize=sizeof(entry);bool running=false;
 for(BOOL found=Process32FirstW(processes,&entry);found;found=Process32NextW(processes,&entry))
  running|=_wcsicmp(entry.szExeFile,L"NimbyRails.exe")==0;
 CloseHandle(processes);
 if(running)throw std::runtime_error("Close NIMBY Rails before editing its SDK profile");
 uint64_t signal{};const std::string_view id=argv[2];
 const auto parsed=std::from_chars(id.data(),id.data()+id.size(),signal);
 if(parsed.ec!=std::errc{}||parsed.ptr!=id.data()+id.size()||signal>>48!=8)
  throw std::invalid_argument("Invalid full signal ID");
 const std::filesystem::path path=argv[1];
 auto data=nimby::detail::SignalSettingsFile::load(path);
 if(!data)throw std::runtime_error("Profile does not exist");
 const auto at=std::find_if(data->signals.begin(),data->signals.end(),[&](const auto& row){return row.id==signal;});
 if(at==data->signals.end())throw std::runtime_error("Signal is absent from this profile");
 for(int i=3;i<argc;i+=2){
  const std::string_view value=argv[i+1];
  if(value!="0"&&value!="1")throw std::invalid_argument("Expected boolean 0 or 1");
  at->values[argv[i]]=value=="1";
 }
 nimby::detail::SignalSettingsFile::save(path,*data);
 std::cout<<"Updated offline SDK profile for signal "<<signal<<"\n";
 return 0;
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;} }
