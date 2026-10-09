#pragma once
#include <nimby/detail/mod_options_bridge.h>
#include <cstdint>
#include <string>

namespace nimby::platform::windows::mod_options {
// Bounded owned data from the SDK launch thread. The UI receives preformatted
// immutable messages; it never reads a log file or queries system resources.
struct LoaderStatus {
    uint32_t code{},requested{},started{};
    uint64_t physical{},commit{},budget{},required{};
    bool operator==(const LoaderStatus&)const=default;
};
inline bool validLoaderStatus(const LoaderStatus& value)noexcept {
    // Admission observes at most the first rejected mod beyond its limit of 32.
    constexpr uint64_t maximumMemory=uint64_t{1}<<60;
    return value.code<=NIMBY_OPTIONS_LOADER_START_FAILED&&value.requested<=33&&
        value.started<=value.requested&&
        (value.code!=NIMBY_OPTIONS_LOADER_NO_MODS||(!value.requested&&!value.started))&&
        (value.code!=NIMBY_OPTIONS_LOADER_RESOURCE_REFUSED||!value.started)&&
        value.physical<=maximumMemory&&value.commit<=maximumMemory&&
        value.budget<=maximumMemory&&value.required<=maximumMemory;
}
struct LoaderDiagnostic {
    LoaderStatus status;
    std::string french,english;
};
inline LoaderDiagnostic loaderDiagnostic(const LoaderStatus& value) {
    LoaderDiagnostic result{value,{},{}};
    if(value.code==NIMBY_OPTIONS_LOADER_READY)return result;
    if(value.code==NIMBY_OPTIONS_LOADER_NO_MODS){
        result.french="Aucun mod n’a été activé. Activez vos mods dans NRF Hub, puis redémarrez le jeu.";
        result.english="No mods are enabled. Enable your mods in NRF Hub, then restart the game.";
        return result;
    }
    if(value.code==NIMBY_OPTIONS_LOADER_START_FAILED){
        if(value.started){
            result.french="Certains mods n’ont pas pu démarrer ("+std::to_string(value.started)+"/"+std::to_string(value.requested)+" actifs). ";
            result.english="Some mods could not start ("+std::to_string(value.started)+"/"+std::to_string(value.requested)+" active). ";
        }else{
            result.french="Les mods n’ont pas pu démarrer. ";
            result.english="The mods could not start. ";
        }
        result.french+="Consultez les journaux dans NRF Hub, puis redémarrez le jeu.";
        result.english+="Check the logs in NRF Hub, then restart the game.";
        return result;
    }
    if(value.requested>32){
        result.french="Le démarrage des mods a été refusé : la limite de 32 mods actifs est dépassée. Désactivez des mods dans NRF Hub, puis redémarrez le jeu.";
        result.english="Mod startup was refused because the limit of 32 active mods was exceeded. Disable mods in NRF Hub, then restart the game.";
        return result;
    }
    const bool memoryKnown=value.physical||value.commit;
    if(memoryKnown&&value.required&&value.budget<value.required){
        result.french="Les mods n’ont pas pu démarrer : la mémoire disponible pour les mods est insuffisante. Fermez des applications ou désactivez des mods dans NRF Hub, puis redémarrez le jeu.";
        result.english="The mods could not start because the memory available for mods is insufficient. Close other applications or disable mods in NRF Hub, then restart the game.";
    }else{
        result.french="Le lancement des mods n’a pas pu être préparé. Consultez les journaux dans NRF Hub, puis redémarrez le jeu.";
        result.english="Mod startup could not be prepared. Check the logs in NRF Hub, then restart the game.";
    }
    const auto mib=[](uint64_t bytes){return std::to_string((bytes+1024*1024-1)/(1024*1024));};
    if(memoryKnown&&value.required){
        result.french+="\nMémoire disponible pour les mods : "+mib(value.budget)+" Mio ; minimum requis : "+mib(value.required)+" Mio.";
        result.english+="\nMemory available for mods: "+mib(value.budget)+" MiB; minimum required: "+mib(value.required)+" MiB.";
    }
    if(memoryKnown){
        result.french+="\nDisponible sur le système : "+mib(value.physical)+" Mio ; mémoire virtuelle disponible : "+mib(value.commit)+" Mio.";
        result.english+="\nAvailable system memory: "+mib(value.physical)+" MiB; available virtual memory: "+mib(value.commit)+" MiB.";
    }
    return result;
}
}
