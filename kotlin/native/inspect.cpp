#include <nimby/kotlin_mod.hpp>
#include <nimby/detail/signal_settings_client.hpp>
#include <nimby/texture_preview.hpp>
#include <iostream>
#include <thread>
#include <iomanip>
int main(int argc,char** argv){try{
 if(argc!=3){std::cerr<<"Usage: nrf_kotlin_inspect COUNT INTERVAL_MS\n";return 1;}
 const auto count=std::stoi(argv[1]),interval=std::stoi(argv[2]);
 if(count<1||count>20000||interval<20||interval>10000)return 1;
 const auto mod=nimby::createMod();
 auto client=nimby::detail::ObservationSession(nimby::detail::discoverProcess());
 std::vector<nimby::Id> expectedSignals;
 std::string expectedWorld;
 for(int n=0;n<count;++n){try{
  const auto snapshot=client.captureSignalling(nimby::kotlin::Rules::textureSet);
  const auto game=snapshot->getGameSession();if(!game)throw std::runtime_error("No world");
  if(n==0)expectedWorld=game->worldId;
  if(expectedWorld!=game->worldId)throw std::runtime_error("World changed during validation");
  const auto saved=nimby::detail::SignalSettingsFile::load(nimby::detail::SignalSettingsClient::profilePath(game->worldId,nimby::kotlin::Rules::settingsId()));
  if(!saved)throw std::runtime_error("BAL settings unavailable");
  auto states=nimby::observeSignals(*snapshot,nimby::kotlin::Rules::textureSet);
  std::vector<nimby::Id> observedSignals;
  for(const auto& state:states)observedSignals.push_back(state.id);
  if(n==0)expectedSignals=observedSignals;
  if(states.empty()||expectedSignals!=observedSignals)throw std::runtime_error("BAL signal set missing or changed");
  nimby::kotlin::Runtime::NetworkRequest request;
  for(auto& state:states){
   state.settings.status=nimby::SettingsStatus::Present;
   for(const auto& field:nimby::kotlin::Rules::checkboxes())state.settings.booleans[std::string(field.name)]=field.defaultValue;
   if(saved)for(const auto& row:saved->signals)if(row.id==state.id)
    for(const auto& [name,value]:row.values)state.settings.booleans[name]=value;
   request.signals.push_back(nimby::kotlin::Rules::fromLive(state));
  }
  const auto result=nimby::kotlin::Runtime::network(request);
  // Une seule verification des binaires/connexion pour ces six signaux.
  // Les reponses restent sequentielles, sans pretendre etre atomiques.
  const auto displayedStates=nimby::getSignalTextureOverrideStatuses(client.getProcessId(),observedSignals);
  // Une recette s'interrompt au premier defaut : ne pas noyer une anomalie
  // dans les lignes suivantes. Le journal du mod detecte aussi un defaut
  // fugace entre deux releves. Archiver ce journal avant de relancer le jeu.
  bool defect=std::filesystem::exists(std::filesystem::temp_directory_path()/nimby::kotlin::Rules::diagnosticFile());
  bool anyActive=false;
  const auto coverage=nimby::observeBlockCoverage(*snapshot);
  const auto clock=snapshot->getSimulationClock();
  if(!clock)throw std::runtime_error("Simulation clock unavailable");
  std::cout<<"{\"sample\":"<<n<<",\"simulation_ms\":"<<clock->getElapsedTime().count()<<",\"age_ms\":"<<snapshot->getAge().count()
   <<",\"occupations_available\":"<<snapshot->getAllOccupations().has_value()
   <<",\"coverage_verified\":"<<coverage.verified
   <<",\"unknown_presence\":"<<coverage.unknownPresence
   <<",\"missing_footprints\":"<<coverage.missingFootprints
   <<",\"unexpected_footprints\":"<<coverage.unexpectedFootprints<<",\"signals\":[";
  for(size_t i=0;i<states.size();++i){
   if(i)std::cout<<',';const auto& state=states[i];const auto& decision=result.signals.values()[i].result.decision;
   std::cout<<"{\"id\":\""<<state.id<<"\",\"name\":\""<<((state.id>>16)&0xffffffff)<<'.'<<(state.id&65535)
    <<"\",\"next\":\""<<state.nextSignal<<"\",\"occupation\":"<<static_cast<int>(state.occupation)
    <<",\"aspect\":"<<std::quoted(std::string(nimby::kotlin::Rules::aspectName(decision.aspect)))
    <<",\"reason\":"<<std::quoted(std::string(nimby::kotlin::Rules::reasonName(decision.reason)))<<",\"trains\":[";
   for(size_t j=0;j<state.trains.size();++j){if(j)std::cout<<',';std::cout<<'"'<<state.trains[j]<<'"';}
   // Lecture du remplacement publie par le mod charge. Ce releve est separe
   // de la capture precedente : une transition peut survenir entre les deux.
   const auto& displayed=displayedStates[i];
   anyActive=anyActive||nimby::kotlin::Rules::isActive(decision);
   defect=defect||nimby::kotlin::Rules::isFault(decision)||!displayed.active;
   std::cout<<"],\"override_active\":"<<displayed.active<<",\"texture_index\":"<<displayed.index<<"}";
  }
  std::cout<<"]}\n";std::cout.flush();
  if(defect||!anyActive){std::cerr<<"Recette interrompue au premier defaut, releve "<<n<<'\n';return 3;}
 }catch(const std::exception& e){std::cout<<"{\"error\":"<<std::quoted(std::string(e.what()))<<"}\n";return 2;}
 std::this_thread::sleep_for(std::chrono::milliseconds(interval));
 }
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 2;}}
