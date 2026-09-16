# Guide développeur — NimbyRailsFranceSDK 0.7

Le SDK expose `<nimby/client.hpp>` en C++20, Windows x64. Sa DLL s'appelle
`NimbyRailsFranceSDK.dll`. Le pont C sous `detail/` est privé ; il ne doit
pas être utilisé directement par les consommateurs.

## Intégrer le kit

Créer une cible exécutable, puis :

```cmake
find_package(NimbyRailsFranceSDK 0.7 CONFIG REQUIRED)
target_compile_features(mon_outil PRIVATE cxx_std_20)
target_link_libraries(mon_outil PRIVATE NimbyRailsFranceSDK::SDK)
add_custom_command(TARGET mon_outil POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${NimbyRailsFranceSDK_RUNTIME_FILES} "$<TARGET_FILE_DIR:mon_outil>"
    COMMAND_EXPAND_LISTS)
```

Configurer avec `-DCMAKE_PREFIX_PATH=C:/SDK/NimbyRailsFranceSDK-0.7.0`.
Le kit MinGW requiert un consommateur MinGW x64 ; pour MSVC, reconstruire
un kit MSVC. Les helpers sont compilés dans le consommateur et ne font
traverser aucun objet STL à la frontière de la DLL.

## Compiler le SDK

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools/build.ps1 -Configuration Release
powershell -NoProfile -ExecutionPolicy Bypass -File tools/package.ps1 -SkipBuild
```

`tools/package.ps1` installe un kit dans `dist/NimbyRailsFranceSDK-0.7.0/`
et compile les trois exemples à partir de ce kit. Le code des exemples ne
dépend pas d'une cible locale du SDK. Voir [CLion](clion.md) pour les presets.

## Contrat

`Client` possède la session et son thread de rafraîchissement ; il n'est ni
copiable ni déplaçable. Arrêter les appels externes avant sa destruction.
`Snapshot::Ptr` permet de conserver une capture après destruction du client.
Les vues `span` nécessitent de garder la capture en vie.

Les champs indisponibles sont des `optional`. Les captures restent non
atomiques. Réservations, occupations et plan de ligne ont des significations
distinctes. Voir la [référence C++](cpp-api-reference.md) et les notes de recherche.

Les tests du pont interne vérifient notamment la validité, les tailles,
les handles, les cycles de vie et l'absence des anciens exports publics.
Ils ne rendent pas ce pont utilisable comme une API publique.

## Distribution

Fournir les DLL de `bin/`, les licences du kit et les notices de vos dépendances.
Le paquet drop-in est distinct de l'observation externe et nécessite la
procédure [d'installation du proxy](install-drop-in.md).
Le TCO et le Hub doivent migrer leur intégration séparément.
