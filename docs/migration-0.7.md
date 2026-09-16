# Migration vers NimbyRailsFranceSDK 0.7.0

Cette version retire l'API C publique de NimbyRailsSDK 0.6. Les fonctionnalités
d'observation sont conservées dans l'API C++20. Le TCO 0.4 existant n'est pas
compatible et sa migration est un travail séparé.

## Paquet et nom

| Ancien | Nouveau |
|---|---|
| NimbyRailsSDK.dll | NimbyRailsFranceSDK.dll |
| NimbyRailsSDK (paquet CMake) | NimbyRailsFranceSDK |
| NimbyRailsSDK::SDK | NimbyRailsFranceSDK::SDK |
| NimbyRailsSDK_RUNTIME_FILES | NimbyRailsFranceSDK_RUNTIME_FILES |
| NimbyRailsLoader.exe | NimbyRailsFranceLoader.exe |
| nimby/sdk.h, nimby/observation.h | nimby/client.hpp |

Utiliser un dossier d'installation neuf, puis reconfigurer et recompiler le
consommateur avec les headers et DLL du même kit 0.7. Les presets utilisent
`install/0.7.0/Release` pour les exemples, y compris en Debug.

## Remplacer les appels

| Ancien appel C | API C++ |
|---|---|
| NimbySdk_GetVersion | nimby::getVersion() |
| OpenProcess / CloseSession | nimby::Client::connect(pid), destruction du client |
| CaptureSnapshot / ReleaseSnapshot | client.capture(), shared_ptr<const Snapshot> |
| CopyTrains / CopyTracks / CopyStations / CopySignals | getAllTrains / getAllTracks / getAllStations / getAllSignals |
| CopyTrainServices / CopyTrainDetails | getAllTrainServices / getTrainServiceById / getTrainDetailsById |
| CopyTrainLineStops | getLineStopsForTrain |
| CopyPlatforms | getPlatformOccupationsForStation |
| CopySignalStates / CopySignalTextures | getAllSignalStates / getAllSignalTextures |
| CopyTrackNodes / CopyTrainPathTracks | getAllTrackNodes / getPathTrackIdsForTrain |
| CopyTrackReservations / CopyTrackOccupations | getAllReservations / getAllOccupations |
| StatusString | nimby::Exception::what(), code(), error() |
| GetSnapshotInfo | getProcessId / getCapturedAt / getAge / getGameSha256 |

Les snapshots sont immuables et peuvent survivre au client. Conserver leur
`shared_ptr` tant qu'une vue `span` est utilisée. Une collection vide et une
collection indisponible (`nullopt`) sont deux états différents.

Pour les erreurs, utiliser `nimby::ErrorCode` et `Exception::code()`.
`nimby::getVersion()` vérifie la DLL sans ouvrir le jeu. Il rejette 0.6 et
les versions du pont interne incompatibles.

## Compatibilité retirée

Les anciens exports `NimbySdk_*` ne sont plus dans la DLL. Il n'existe pas
d'alias de paquet CMake ni de header de compatibilité. L'ancien indicateur
`NIMBY_TRAIN_PRESENT` est retiré ; la vitesse exige une validité explicite.
Utiliser `TrainService::isOnNetwork()` et `isHidden()` pour la présence native,
et `Train::getSpeedKmh()` / `isSpeedDefaulted()` pour la vitesse.

Les déclarations sous `nimby/detail/` et les exports `NimbyInternal_*` restent
installés pour permettre la compilation et la liaison des helpers C++.
Ils sont privés, sans garantie de compatibilité pour un appel direct. Le pont
interne est en version 2 ; aucun objet STL ne traverse cette frontière.

Les fonctions de diagnostic d'hôte et de cycle de vie du chargeur sont aussi
internes. Un outil externe utilise simplement `Client::connect()`.

Pour une installation drop-in 0.6, employer son ancien script de désinstallation
avant le nouvel installateur. La mise à jour automatique du TCO et du catalogue
Hub n'est pas effectuée par cette migration.
