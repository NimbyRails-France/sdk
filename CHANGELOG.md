# Changelog

## [Unreleased]

- Politique de canaux alpha / bêta / stable contrôlée en CI ; publication directe par release GitHub.

- Les scripts de packaging utilisent la version du projet au lieu de numéros écrits en dur.

- Validation Woodpecker et contrôle de cohérence des versions et du changelog.

## [0.7.2] - 2026-09-18

SDK 0.7.2 : adaptateur C++ pour les mods, chargement automatique par NRF Loader, commandes de textures par chemin utilisables dans le jeu, tutoriel de creation de mod et outils de topologie. ABI interne 2 conservee. Utiliser le Hub 0.2.3 pour installer les mods C++.

Validation : 17 tests CTest, consommateurs du kit installe, proxy SDL et installation/retrait du pont sur des copies des binaires. Les commandes visuelles restent experimentales ; leur rendu en jeu et les regles de signalisation ne sont pas annonces comme valides par ces tests.

## [0.7.1] - 2026-09-16

Lecture et modification expÃ©rimentale de la date de simulation, GUI auto-observer, quais centralisÃ©s et textures natives des signaux.

- Horloge UTC native, dates avant 1970, translation calendaire et commande explicite de recalage par l'intervention native des trains.
- Auto-observer graphique avec modification de date et vues trains / gares / quais.
- Regroupement des tronÃ§ons d'un mÃªme quai ; occupations et rÃ©servations dÃ©doublonnÃ©es, dÃ©tail conservÃ©.
- SÃ©lecteur de texture zÃ©ro lorsque le moteur utilise sa valeur par dÃ©faut, sans masquer les donnÃ©es indisponibles. Les sÃ©lecteurs explicites et les changements de texture sont conservÃ©s.
- SDK C++20, ABI interne 2 ; les nouveaux clients demandent une DLL 0.7.1 compatible.

Validation : 14 tests SDK, trois exemples compilÃ©s depuis le paquet installÃ©, changement de sauvegarde, 16 312 signaux sur la grande partie et alternance native de textures sur la petite partie. Voir docs/release-0.7.1-validation.md.

Pour les textures de mods lisibles et les signaux superposÃ©s sÃ©parÃ©s Ã  l'Ã©cran, utiliser aussi TCO 0.5.2.

La modification de date reste expÃ©rimentale : le recalage utilise les interventions du moteur, avec leurs coÃ»ts et dÃ©placements. Le multijoueur et tous les systÃ¨mes calendaires ne sont pas validÃ©s. Voir docs/simulation-clock.md avant usage.

## [0.7.0] - 2026-09-16

NimbyRailsSDK devient **NimbyRailsFranceSDK**, avec une API publique **C++20 uniquement**.

### Rupture de compatibilité

- Nouvelle DLL : `NimbyRailsFranceSDK.dll`.
- Nouveau paquet CMake : `find_package(NimbyRailsFranceSDK 0.7 CONFIG REQUIRED)` ; cible `NimbyRailsFranceSDK::SDK`.
- Les anciens exports `NimbySdk_*` et les headers publics `nimby/sdk.h` et `nimby/observation.h` sont retirés.
- Utiliser `<nimby/client.hpp>`, `nimby::Client`, les snapshots et les getters. Les fonctions `NimbyInternal_*` et les headers `detail/` restent un pont privé, version 2, nécessaire aux helpers.
- Le TCO existant n'est pas compatible : sa migration est séparée. Remplacer seulement sa DLL ne suffit pas.

Voir le [guide de migration 0.6 vers 0.7](https://github.com/NimbyRails-France/sdk/blob/v0.7.0/docs/migration-0.7.md).

### API et outils

- `nimby::getVersion()` vérifie la DLL sans ouvrir le jeu.
- `Exception::code()` et `nimby::ErrorCode` exposent les erreurs en C++.
- `TrainService::isOnNetwork()` et `isHidden()` distinguent présence native, position masquée et valeur inconnue.
- La vitesse exige désormais un indicateur de validité explicite ; le repli historique sur `NIMBY_TRAIN_PRESENT` est retiré.
- Les trois exemples utilisent uniquement l'API C++ et le SDK installé. Configurations CLion, chargeur, paquets et documentation sont renommés.
- Les données de la 0.6.6 restent disponibles : services des trains, localisation, quais, occupations, réservations, plan de ligne et voyageurs embarqués.

### Paquets

- `NimbyRailsFranceSDK-0.7.0-windows-x64-mingw.zip` : kit de développement, DLL, en-têtes, bibliothèque d'import, documentation et exemples.
- `NimbyRailsFranceSDK-0.7.0-drop-in-windows-x64.zip` : proxy SDL et installateur.
- `NimbyRailsFranceSDK-0.7.0-hub.zip` et `project.json` : paquet Hub et métadonnées. Le catalogue Hub et le TCO doivent être migrés séparément.
- `SHA256SUMS.txt` : empreintes des trois archives et des métadonnées.

Pour une ancienne installation drop-in 0.6, utiliser son ancien script de désinstallation avant d'installer 0.7.

### Validation et limites

13/13 tests réussis en Debug et Release, dont le rejet de l'ancienne interface et l'absence des anciens exports. Les trois exemples compilent indépendamment depuis le paquet installé et passent leur contrôle de version sans jeu. Aucun nouvel essai en partie réelle pour cette migration : le jeu était fermé.

Windows x64 / C++20, kit MinGW. Les snapshots restent non atomiques et les valeurs indisponibles optionnelles. Les horaires du plan de ligne sont relatifs, sans garantie sur le parcours futur effectif.

Profil du jeu compatible (SHA-256) : `fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae`.

Les archives ne contiennent ni exécutable du jeu, ni SDL originale, ni sauvegarde.

## [0.6.6] - 2026-09-16

Cette release rassemble les évolutions du SDK depuis la 0.6.0, avec une API C (ABI 1), les helpers C++20 et les trois exemples autonomes.

### Nouveautés et corrections

- États de service des trains : circulation, arrêt en gare, arrêt temporisé, dépôt confirmé, attente de mise en circulation et attente au signal.
- Localisation par voie, gare et quai lorsqu'elle est disponible ; distinction entre position inconnue et train non placé dans la simulation.
- Noms automatiques des gares et noms des quais, avec occupations et réservations séparées et identification des trains concernés.
- Arrêt actif, échéances d'arrivée et de départ, attente avant une nouvelle tentative de mise en circulation, identifiants de service et plan des arrêts de ligne.
- Compteur de voyageurs embarqués corrigé : les versions de développement 0.6.4/0.6.5 lisaient la capacité du train.
- Vitesse mesurée distinguée du zéro affiché par défaut par le jeu.
- Configurations CLion, exemples liés au SDK installé et documentation des API C/C++ enrichies.

### Paquets

- `NimbyRailsSDK-0.6.6-windows-x64-mingw.zip` : kit de développement Windows x64, DLL, en-têtes, bibliothèque d'import, documentation et exemples.
- `NimbyRailsSDK-0.6.6-drop-in-windows-x64.zip` : chargeur SDL et script d'installation.
- `NimbyRailsSDK-0.6.6-hub.zip` et `project.json` : paquet et métadonnées pour NRF Hub.
- `SHA256SUMS.txt` : empreintes des trois ZIP et des métadonnées.

### Validation et limites

13 tests natifs réussis en Release ; les trois exemples compilent indépendamment à partir du paquet installé. Contrôle de version : SDK 0.6.6, ABI 1. Les essais en jeu ont aussi vérifié les états, les localisations et la correction du compteur de voyageurs.

Les nouveaux exemples requièrent la DLL 0.6.6. Les structures existantes conservent leur taille. Les captures restent non atomiques ; les indicateurs de validité et les valeurs optionnelles signalent les données indisponibles. Les horaires du plan de ligne sont relatifs, sans garantie sur le futur parcours effectif. Les sections de voie portant le même nom de quai restent distinctes.

Profil du jeu compatible (SHA-256) : `fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae`.

Les paquets ne contiennent ni l'exécutable du jeu, ni sa SDL originale, ni de sauvegarde.

## [0.6.0] - 2026-09-15

SDK 0.6.0 ajoute les états et textures natifs des signaux, une API de version utilisable avant initialisation et un paquet géré par NRF Hub.

- SDK C / Windows x64, ABI 1 ; TCO 0.4 requiert SDK 0.6.x.
- Paquet hub : kit de développement et chargeur SDL dans loader/.
- Paquet drop-in : installation dans le jeu avec sauvegarde de la SDL originale.
- 13 tests natifs réussis ; observation en lecture seule, hooks internes désactivés.

Jeu compatible : SHA-256 FFF49AC21720ABFC824C2B4F68B862727630EB0DB71CFE1F9EA8F685D0DB10AE.
Les assets ne contiennent pas l'exécutable du jeu, la SDL originale ni de sauvegardes.

## [0.5.0] - 2026-09-14

## Installation dans le jeu

TÃ©lÃ©charger **NimbyRailsSDK-0.5.0-drop-in-windows-x64.zip** dans Assets et extraire tout le ZIP. Il regroupe SDL3.dll, NimbyRailsSDK.dll et libwinpthread-1.dll. Jeu fermÃ©, suivre le README et lancer install-proxy.ps1 : la SDL originale est conservÃ©e. Ne pas Ã©craser directement la SDL du jeu. Pour mettre Ã  jour une ancienne installation, suivre la procÃ©dure du [guide](https://github.com/NimbyRails-France/sdk/blob/v0.5.0/docs/install-drop-in.md).

Le ZIP windows-x64-mingw est le kit de dÃ©veloppement pour les autres projets C/C++. [Guide dÃ©veloppeur](https://github.com/NimbyRails-France/sdk/blob/v0.5.0/docs/developing.md). Le [TCO 0.2.0](https://github.com/NimbyRails-France/tco/releases/tag/v0.2.0) inclut dÃ©jÃ  le runtime du SDK.

## NouveautÃ©s

- API publique NimbySdk_CopyTrackReservations et NimbySdk_CopyTrackOccupations : identifiant du train, voie et bornes normalisÃ©es.
- Collections indÃ©pendantes dans un snapshot immuable ; indisponibilitÃ© distincte d'une collection vide.
- ContrÃ´les de version, IDs, bornes, structures et relectures de stabilitÃ©. Aucun nouveau hook ni Ã©criture dans le jeu.
- Recherche documentÃ©e avec preuves statiques, captures rÃ©elles et exemple consommateur autonome.

Validation : 11 tests SDK rÃ©ussis, compilation de l'exemple depuis le kit installÃ©, 20 captures rÃ©elles de rÃ©servations et contrÃ´le des buffers/IDs/immutabilitÃ©. Les archives et les DLL contenues ont Ã©tÃ© vÃ©rifiÃ©es par SHA-256.

Version expÃ©rimentale, Windows x64, uniquement le binaire SHA-256 FFF49AC21720ABFC824C2B4F68B862727630EB0DB71CFE1F9EA8F685D0DB10AE. Les intervalles ne constituent pas un itinÃ©raire ordonnÃ© ; occupation, rÃ©servation et Path sont distincts. Lecture non atomique ; rÃ©servations virtuelles de scripts non interrogÃ©es. [Preuves et limites](https://github.com/NimbyRails-France/sdk/blob/v0.5.0/docs/research/reservations.md).

## [0.4.0] - 2026-09-14

## DLL à installer dans le dossier du jeu

**Télécharger `NimbyRailsSDK-0.4.0-drop-in-windows-x64.zip`.**
Il contient **SDL3.dll**, **NimbyRailsSDK.dll** et leur dépendance **libwinpthread-1.dll**,
avec installateur, README et licences. Les DLL sont distribuées ensemble dans ce ZIP.

Jeu fermé, extraire le ZIP puis lancer `install-proxy.ps1` depuis le dossier extrait.
Il conserve la SDL originale sous **NimbyRailsSDL3Original.dll**, installe les DLL
et permet leur retrait avec `-Action Remove`. Ne pas écraser la SDL originale.
[Guide de copie manuelle, installation et désinstallation](https://github.com/NimbyRails-France/sdk/blob/main/docs/install-drop-in.md).

Le SDK se charge après SDL_Init à chaque lancement normal du jeu. Ses hooks internes
restent désactivés. La SDL originale du jeu n'est pas incluse dans le téléchargement.
Validation : 10/10 tests, dont proxy_forwarding ; installation/retrait sur copie isolée
avec restauration de la SDL originale. Empreintes : **SHA256SUMS-drop-in.txt**.

Le ZIP `windows-x64-mingw.zip` ci-dessous reste le kit destiné au développement.

---

## Installer le SDK

Télécharger **NimbyRailsSDK-0.4.0-windows-x64-mingw.zip**, puis extraire tout le dossier.
Dans CLion / CMake, utiliser `-DCMAKE_PREFIX_PATH=C:/SDK/NimbyRailsSDK-0.4.0`,
`find_package(NimbyRailsSDK 0.4 CONFIG REQUIRED)` et `NimbyRailsSDK::SDK`.
Le ZIP contient les headers publics, bibliothèques d'import MinGW, DLL et runtimes,
configuration CMake, documentation, notices et un exemple autonome.
**Instructions complètes : [README](https://github.com/NimbyRails-France/sdk#installer-depuis-une-release).**

API d'observation en lecture seule : trains, vitesses, voies, gares, signaux,
coordonnées natives et Paths expérimentaux. MinHook est intégré dans la DLL.
Aucune installation dans le jeu requise pour cette API externe.

Windows x64 / MinGW GCC 15.2. Pour MSVC, reconstruire depuis les sources.
Version expérimentale : positions parfois indisponibles, graphe partiel, aspects
des signaux inconnus ; Path ne signifie pas réservation. Binaire reconnu :
`FFF49AC21720ABFC824C2B4F68B862727630EB0DB71CFE1F9EA8F685D0DB10AE`.

Validation : compilation Release propre, 9/9 tests autonomes réussis,
exemple consommateur compilé et chargé depuis le ZIP extrait et déplacé.
Le test optionnel de proxy avec la SDL du jeu ne fait pas partie de ces 9 tests.
Empreinte du ZIP dans **SHA256SUMS.txt**.

Pour l'application graphique prête à lancer : [Nimby TCO](https://github.com/NimbyRails-France/tco/releases/tag/v0.1.0).
