# Diagnostics, proxy et ancien chargeur

Pour développer un outil externe avec le SDK 0.6.x, commencer par le
[tutoriel](tutorial-first-tool.md), le [guide développeur](developing.md) et la
[référence API](api-reference.md). Cette page conserve les procédures des
diagnostics et du chargement dans le jeu ; elles ne sont pas nécessaires au tutoriel.

SDK natif C++20, DLL Windows AMD64. MinHook 1.3.4 est compilé et lié statiquement
(sources x64 et licence dans `third_party/minhook/`). Le socle de diagnostic fonctionne dans un
programme autonome et peut maintenant être chargé au démarrage par le proxy SDL3.
Aucun hook du jeu n'est validé ou installé. Depuis la version 0.2, une API publique
de lecture est disponible : voir le [guide développeur](developing.md) pour
`find_package`, l'exemple autonome, les instantanés et leurs limites expérimentales.
Les commandes de simulation et setters restent à concevoir après validation.

## Construire et tester

Dans PowerShell, depuis ce dossier :

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File ./tools/build.ps1
```

Les commandes ci-dessus se lancent depuis la racine du projet. Le script utilise les outils livrés avec CLion et vérifie chaque code de sortie.
L'option ExecutionPolicy est limitée à ce processus ; aucune politique système
n'est modifiée (l'exécution des scripts est désactivée par défaut sur ce poste).
Adapter `-ClionHome` si nécessaire. `-Configuration Release` est également accepté.
Dans CLion : recharger CMake, sélectionner `NimbyRailsSDK`, puis lancer les tests CTest.
Avec une chaîne Windows x64 déjà dans PATH :

```powershell
cmake -S . -B build/Debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build/Debug
ctest --test-dir build/Debug --output-on-failure
```

Le chargeur et les tests MinGW utilisent leurs DLL de runtime, copiées à côté des exécutables.
La DLL du SDK 0.2 utilise bcrypt/KERNEL32/msvcrt et `libwinpthread-1.dll` avec la
chaîne MinGW testée. Les runtimes requis sont fournis dans le paquet ; voir le
guide développeur pour leur copie près du programme consommateur.
MinHook ne nécessite aucune DLL séparée. Conserver les fichiers de `bin/` ensemble
lors du déplacement. MSVC est prévu par CMake, mais non testé à ce
jalon. Aucune bibliothèque STL, exception ou allocation ne traverse l'ABI publique.

## Cycle de vie explicite

`tests/host.cpp` charge uniquement notre DLL dans son propre processus et résout
les quatre exports de `include/nimby/sdk.h` avec `GetProcAddress`.

1. Après le retour de `LoadLibrary`, appeler `NimbySdk_Initialize(1, 0)`.
2. Initialiser `NimbyBinaryInfo.struct_size` à `sizeof(NimbyBinaryInfo)` avant lecture.
3. Lire le diagnostic avec `NimbySdk_GetHostInfo`.
4. Arrêter et joindre tous les utilisateurs de l'API, appeler `NimbySdk_Shutdown`,
   puis `FreeLibrary`. Ne pas rappeler l'API après déchargement.

Ne jamais appeler ces fonctions depuis un DllMain ou callback TLS. DllMain retourne
simplement TRUE. Pas de worker SDK, callback de mod, hook ou objet moteur conservé.
L'état est protégé par SRWLOCK ; l'arrêt est idempotent. Les buffers appartiennent
à l'appelant et doivent être valides, correctement alignés et non partagés en écriture.
Les chemins d'inspection sont UTF-16. L'ABI v1 est expérimentale, avec taille exacte
des structures exigée et sans garantie de compatibilité avec une future API de mods.

La journalisation utilise `OutputDebugStringA` : observable avec un débogueur ou un
collecteur Windows, sans fichier global ni callback réentrant. Elle signale le début,
l'empreinte de l'hôte, les refus et l'arrêt. L'hôte écrit le résultat des tests sur stdout.

L'identification utilise SHA-256 via Windows BCrypt et vérifie les en-têtes PE AMD64.
L'empreinte connue identifie une version de **recherche**, pas une version compatible
avec des hooks. Toute demande `NIMBY_REQUEST_HOOKS` échoue sans changer l'état.
Un hôte inconnu peut démarrer les diagnostics. Inspecter un autre fichier ne change
jamais l'identité de l'hôte. Aucun contenu en mémoire n'est validé par ce mécanisme.

Inspection facultative de l'original, sans chargement ni exécution du jeu :

```powershell
./build/Debug/nimby_sdk_host.exe ./build/Debug/NimbyRailsSDK.dll 'C:/Program Files (x86)/Steam/steamapps/common/NIMBY Rails/NimbyRails.exe'
```

Le test `minhook_observation` détourne uniquement une fonction de test dont nous
possédons le code : création, trampoline, activation, observation concurrente,
préservation du résultat, drainage, désactivation et retrait sur trois cycles.
Cela valide l'intégration de MinHook, pas une cible du jeu. MinHook est initialisé
après identification de l'hôte et arrêté explicitement, toujours hors DllMain.

## Installation par copie de DLL (méthode retenue)

`build/Release/drop-in/` contient notre proxy `SDL3.dll` et `NimbyRailsSDK.dll`.
Le jeu possède déjà sa propre SDL3.dll : elle doit être conservée sous le nom
`NimbyRailsSDL3Original.dll`, jamais écrasée sans sauvegarde. L'installateur effectue
cette opération et vérifie les empreintes. Depuis la racine, jeu fermé :

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools/install-proxy.ps1
# Pour une autre bibliothèque Steam : ajouter -GameDirectory 'D:/SteamLibrary/steamapps/common/NIMBY Rails'
# Pour restaurer les fichiers d'origine :
powershell -NoProfile -ExecutionPolicy Bypass -File tools/install-proxy.ps1 -Action Remove
```

Les copies sont préparées et vérifiées avant le remplacement. Le fichier
`NimbyRailsSDK-install.json` enregistre leurs empreintes pour éviter de supprimer
des fichiers modifiés ou étrangers lors de la restauration. L'exécutable original
et les sauvegardes ne sont pas modifiés par l'installateur. Garder la sauvegarde
SDL et ce manifeste jusqu'à désinstallation.

Le proxy transmet les 1 271 exports d'origine, avec leurs ordinaux. Il enveloppe
uniquement SDL_Init et SDL_Quit, dont l'ABI publique est documentée : après une
initialisation SDL réussie il démarre le SDK ; avant SDL_Quit il arrête le SDK s'il
en possède le cycle de vie. DllMain conserve seulement le handle du proxy.
Il n'y a ni injection distante ni superviseur externe dans ce mode.

L'empreinte de l'exécutable et celle de SDL sont contrôlées avant activation du SDK.
Un binaire inconnu ou un SDK absent est journalisé ; les appels SDL continuent.
Journal : `%LOCALAPPDATA%/NimbyRailsSDK/proxy.log`. Les modules sont conservés jusqu'à
la fermeture du processus, sans déchargement à chaud. Les hooks du jeu restent refusés.

Après une mise à jour du jeu, ne pas forcer l'installation : il faut réexaminer
les nouvelles versions. Une vérification des fichiers Steam peut remettre SDL3.dll
d'origine et donc retirer le proxy ; la présence de la sauvegarde ne garantit pas
que le proxy est encore installé. La désinstallation refuse un fichier d'empreinte
différente plutôt que de l'écraser.

## Ancien chargeur externe (diagnostic uniquement)

Le chargement a été autorisé par l'utilisateur et est maintenant implémenté.
Ce mode est remplacé pour l'usage normal par le proxy décrit ci-dessus. Ne pas
lancer le superviseur simultanément avec le proxy.
`NimbyRailsLoader.exe` surveille les processus toutes les secondes. Il contrôle le
chemin canonique, l'architecture AMD64 et le SHA-256 reconnu, charge la DLL voisine
avec LoadLibraryW puis appelle `NimbySdk_Bootstrap` hors DllMain. Le couple PID/date
de création évite les doublons et détecte les nouvelles instances du jeu. Un mutex
empêche deux superviseurs de fonctionner simultanément dans la même session Windows.
La fonction Bootstrap est un export interne supplémentaire, distinct des quatre
fonctions de l'API de mods ; elle demande toujours le mode diagnostic sans hooks.

Depuis la racine du projet :

```powershell
# Surveillance continue (laisser la fenêtre ouverte).
./build/package/bin/NimbyRailsLoader.exe
# Un seul passage sur les processus actuellement ouverts.
./build/package/bin/NimbyRailsLoader.exe --once
# Autre emplacement Steam ; l'empreinte doit toujours être reconnue.
./build/package/bin/NimbyRailsLoader.exe --game 'D:/SteamLibrary/steamapps/common/NIMBY Rails/NimbyRails.exe'
# Arrêt propre du superviseur, y compris s'il fonctionne en arrière-plan.
./build/package/bin/NimbyRailsLoader.exe --stop
```

Le chargeur ne démarre ni ne ferme le jeu. Son arrêt ne décharge pas le SDK déjà
présent : celui-ci reste chargé jusqu'à la fermeture du jeu. Aucun service, tâche
planifiée ou démarrage Windows automatique n'est installé. Pour reprendre la
surveillance après un redémarrage du PC, relancer le chargeur.

L'instance lancée pendant le développement écrit dans
`build/package/bin/loader.log` et `loader-error.log` via redirection de stdout/stderr.
Un lancement manuel affiche ces messages dans la console. Les diagnostics internes
du SDK restent disponibles par OutputDebugString ; le chargeur confirme le code
de retour de l'initialisation, pas l'exécution d'un hook de simulation.

Une nouvelle version inconnue du jeu est refusée. Une erreur après tentative de
chargement n'est pas réessayée sur cette même instance. En cas de timeout de thread
(15 secondes), le chargeur ne tue pas ce thread et conserve son éventuel buffer de
chemin jusqu'à la fin du processus afin d'éviter un accès à de la mémoire libérée.
L'exécution requiert Windows x64 avec IsWow64Process2 (Windows 10 ou ultérieur).
Le chargeur et le jeu doivent être accessibles au même utilisateur/niveau de droits.

## Mettre à jour les binaires de développement

Avec le proxy : fermer le jeu, désinstaller avec `-Action Remove`, reconstruire
en Release, puis réinstaller. Les paragraphes suivants concernent l'ancien chargeur.

Arrêter le superviseur puis fermer le jeu avant de remplacer une DLL chargée.
Les builds restent possibles sous `build/Debug` ou `build/Release` pendant qu'une
version installée dans `build/package/bin` est utilisée. Après compilation Release :

```powershell
cmake --install build/Release --prefix build/package
```

Puis relancer le superviseur et le jeu. Ne pas déplacer le dossier `bin/` pendant
la surveillance. Le fichier du SDK est verrouillé en lecture pendant celle-ci.

Lire [le premier rapport historique](research/first-milestone.md) pour les
découvertes et limites de ce jalon. Pour l'installation actuelle dans le jeu,
suivre [la procédure du proxy](install-drop-in.md). Le binaire original et le projet Ghidra ne sont pas modifiés ; le
chargeur n'accède pas aux sauvegardes. Les validations du chargeur sont décrites
dans [le rapport](research/loader-validation.md).
