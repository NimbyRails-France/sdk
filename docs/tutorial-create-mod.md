# Créer son premier mod C++, fichier par fichier

Ce tutoriel part d'un dossier vide. Vous créez les fichiers et écrivez le code
vous-même. Il n'y a pas de projet préfabriqué à télécharger ni de textures SFR
à recopier. Le nom fictif utilisé ici est **MonPremierMod** : remplacez-le par
celui de votre projet.

À la fin, vous aurez une DLL que le NRF Loader peut charger et deux actions C++ :
afficher une texture sur un signal et restaurer sa texture native. Le tutoriel
ne construit pas un moteur de signalisation automatique.

## 1. Préparer votre environnement

Il faut Windows x64, CMake 3.24 ou plus, Ninja et un compilateur MinGW compatible
avec celui du kit SDK. Dans CLion, choisissez la chaîne MinGW correspondante.
Les commandes ci-dessous supposent que `cmake`, `ninja` et `g++` sont dans `PATH`.

Utilisez le **kit de développement mis à jour** qui contient :

- `include/nimby/mod.hpp` et `include/nimby/signal_textures.hpp` ;
- la cible CMake `NimbyRailsFranceSDK::Mod` ;
- `share/NimbyRailsFranceSDK/mod/entry.cpp`.

Ces ajouts sont ceux de la version de développement actuelle. Le numéro
`0.7.1` seul ne garantit pas leur présence dans une ancienne archive publiée.
Le NRF Loader installé dans le jeu doit également être celui qui prend en charge
les mods C++ et leur manifeste `nrf-mod.ini`.

Dans les commandes, remplacez `C:/Dev/NRFSDK` par le dossier du **kit installé**,
contenant `include/`, `bin/` et `lib/cmake/`. Ce n'est pas le dossier des sources
du SDK. Si vous travaillez dans les dépôts locaux, il peut s'agir de
`C:/Users/<vous>/CLionProjects/sdk/install/development`.

## 2. Créer les dossiers

Créez un dossier `MonPremierMod`, puis les sous-dossiers `src` et `assets`.
Vous allez y écrire trois premiers fichiers :

```text
MonPremierMod/
  CMakeLists.txt
  src/
    mod.cpp
  assets/
    nrf-mod.ini
```

| Fichier | Votre travail |
|---|---|
| `src/mod.cpp` | Écrire les actions de votre mod |
| `CMakeLists.txt` | Dire à CMake comment construire la DLL avec le SDK |
| `assets/nrf-mod.ini` | Indiquer au NRF Loader le nom exact de cette DLL |

Il n'y a pas de `main.cpp` : vous construisez une DLL chargée dans le jeu,
pas un programme à lancer séparément.

## 3. Écrire un premier `mod.cpp` minimal

Créez `src/mod.cpp` et saisissez :

```cpp
#include <nimby/mod.hpp>

nimby::Mod nimby::createMod() {
    return {};
}
```

`createMod()` décrit les actions disponibles. Pour l'instant, vous n'en déclarez
aucune : le module pourra démarrer, mais ne changera rien dans la partie.

Vous n'avez pas à écrire `extern "C"`, `DllMain` ou des fonctions Windows.
L'adaptateur du SDK les ajoute à votre DLL lors de sa compilation.

## 4. Écrire `CMakeLists.txt`

À la racine du projet, créez ce fichier :

```cmake
cmake_minimum_required(VERSION 3.24)
project(MonPremierMod VERSION 0.1.0 LANGUAGES CXX)

find_package(NimbyRailsFranceSDK 0.7.1 CONFIG REQUIRED)
if(NOT TARGET NimbyRailsFranceSDK::Mod)
    message(FATAL_ERROR "Ce kit SDK est trop ancien : adaptateur de mod absent")
endif()

add_library(MonPremierMod SHARED src/mod.cpp)
target_link_libraries(MonPremierMod PRIVATE NimbyRailsFranceSDK::Mod)
set_target_properties(MonPremierMod PROPERTIES
    PREFIX ""
    OUTPUT_NAME "MonPremierMod")

if(MINGW)
    target_link_options(MonPremierMod PRIVATE -static-libgcc -static-libstdc++)
endif()
if(MSVC)
    target_compile_options(MonPremierMod PRIVATE /utf-8)
endif()

install(TARGETS MonPremierMod RUNTIME DESTINATION .)
install(FILES assets/nrf-mod.ini DESTINATION .)
```

`SHARED` crée une DLL. `PREFIX ""` évite un nom comme `libMonPremierMod.dll`.
`OUTPUT_NAME` choisit le nom du fichier sans son extension.
La cible `NimbyRailsFranceSDK::Mod` apporte C++20, le SDK et son adaptateur.
Les lignes `install` prépareront un dossier contenant les fichiers à déployer.

## 5. Écrire `nrf-mod.ini`

Créez `assets/nrf-mod.ini` :

```ini
[NRFMod]
library=MonPremierMod.dll
```

La valeur de `library` doit correspondre **exactement au nom de la DLL produite**.
Indiquez uniquement le nom du fichier, sans chemin ni sous-dossier.
Si vous changez `OUTPUT_NAME` dans CMake, changez aussi cette valeur.

Ce fichier dit au loader **quoi charger**. Il ne dit pas quelle texture afficher
et ne contient pas les règles de fonctionnement du mod.

## 6. Compiler ce premier module

Ouvrez un terminal dans le dossier `MonPremierMod` :

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++ -DCMAKE_PREFIX_PATH="C:/Dev/NRFSDK"
cmake --build build
cmake --install build --prefix package/MonPremierMod
```

Le dossier `package/MonPremierMod` doit contenir `MonPremierMod.dll` et
`nrf-mod.ini`. Vous avez construit un module vide, pas encore un mod de textures.
Ne cherchez pas un bouton pour exécuter la DLL comme un EXE : elle sera chargée
par le NRF Loader.

## 7. Créer vos deux textures et leur déclaration

Dans `assets`, créez le dossier `imgs`. Avec votre éditeur SVG, dessinez **deux
images personnelles de test**, par exemple deux formes géométriques différentes.
Enregistrez-les sous `premiere.svg` et `seconde.svg`. Les noms sont des exemples,
ils n'ont aucune signification de signalisation réelle.

Créez ensuite `assets/mod.txt` :

```ini
[ModMeta]
schema=1
name=Mon premier mod de textures
author=Votre nom
desc=Deux images personnelles pour apprendre le SDK.
version=0.1.0

[SignalTextures]
id=mon_premier_mod_textures
name_en=My first texture set
name_loc=mon_premier_mod_textures_name
state=imgs/premiere.svg
state=imgs/seconde.svg

[SignalTemplate]
name_en=My first test signal
name_loc=mon_premier_mod_signal_name
kind=path
textures=mon_premier_mod_textures
```

Choisissez un identifiant de textures propre à votre projet pour éviter les
collisions avec d'autres mods. Le même identifiant sera utilisé dans le C++.
Les chemins sont relatifs au dossier du paquet, où se trouve `mod.txt`.

Ajoutez ces deux lignes à la fin de `CMakeLists.txt` :

```cmake
install(FILES assets/mod.txt DESTINATION .)
install(DIRECTORY assets/imgs DESTINATION .)
```

Les deux manifestes ont des lecteurs différents : `mod.txt` déclare les
ressources au **jeu**, tandis que `nrf-mod.ini` déclare la DLL au **NRF Loader**.

## 8. Écrire les actions du mod

Remplacez maintenant le contenu de `src/mod.cpp` par :

```cpp
#include <nimby/mod.hpp>

nimby::Mod nimby::createMod() {
    return {
        .showTexture = [](Id signal, const char* path) {
            SignalTextures::inGame().show(
                signal, {"mon_premier_mod_textures", path});
        },
        .restoreTexture = [](Id signal) {
            SignalTextures::inGame().restore(signal);
        }
    };
}
```

Une fonction associée à une action est appelée ici un *callback*. Vous écrivez
ce qu'elle fait ; l'adaptateur reçoit la commande et appelle votre fonction.

- `signal` est l'identifiant 64 bits du signal à modifier. Ce n'est ni un numéro
  de ligne ni un index de texture.
- `path` est le chemin demandé, par exemple `imgs/premiere.svg`.
- `inGame()` utilise le processus où le mod est chargé ; aucun PID à saisir.
- `show()` retrouve l'image dans le catalogue du jeu et gère le pont de rendu.
- `restore()` rend la main à la texture native du signal.

L'adaptateur vérifie les arguments et convertit les exceptions en erreurs de
commande. Un fichier absent du catalogue ne sera pas remplacé arbitrairement
par une autre image. Les ressources doivent déjà être activées et chargées.

**Ces callbacks ne sont pas des événements automatiques du jeu.** Le loader
appelle le démarrage et l'arrêt ; il ne choisit ni signal ni texture et n'appelle
pas `showTexture` à chaque image. Votre logique devra décider quand afficher
une image et sur quel signal, ou un outil de test devra envoyer cette commande.

Dans une fonction de votre logique disposant déjà d'un `signalId` valide et
d'une partie chargée, l'appel direct équivalent est :

```cpp
nimby::SignalTextures::inGame().show(
    signalId, {"mon_premier_mod_textures", "imgs/premiere.svg"});
```

Ce fragment se place **dans cette fonction**, pas tout seul à la racine du
fichier. Ne remplacez pas `signalId` par une adresse mémoire ni un nombre choisi
au hasard. La sélection du signal et le déclencheur sont la partie du mod que
vous devez développer selon votre besoin.

Les callbacks optionnels `.start` et `.stop` servent à initialiser et libérer
vos ressources. `start` arrive après SDL, mais **avant qu'une partie soit
forcément disponible** : n'y forcez pas une texture. Si vous lancez des tâches,
`stop` doit les arrêter et les rejoindre avant de rendre la main.

## 9. Préparer et enregistrer le paquet

Relancez la compilation et l'installation de l'étape 6. Vous devez obtenir :

```text
package/MonPremierMod/
  MonPremierMod.dll
  nrf-mod.ini
  mod.txt
  imgs/
    premiere.svg
    seconde.svg
```

Conservez ce dossier comme une unité. Ne recopiez pas une seconde DLL SDK à
l'intérieur : le loader du jeu fournit le SDK et son pont de textures compatibles.

Le Hub installe le paquet dans votre bibliothèque et enregistre deux liens :
les ressources dans les mods du jeu, et le module dans
`<dossier du jeu>/NRFMods/<identifiant du projet>`. Un paquet Hub de ce type
déclare notamment :

```json
{
  "id": "mon-premier-mod",
  "kind": "native-mod",
  "modId": "MonPremierMod",
  "loaderApi": 1,
  "module": "MonPremierMod.dll",
  "sdkMin": "0.7.1",
  "sdkMaxExclusive": "0.8.0"
}
```

C'est un **extrait explicatif**, pas un manifeste publiable : il manque entre
autres le nom, la version, le dossier racine, l'URL du ZIP, sa taille, son SHA-256
et les empreintes des versions du jeu effectivement validées. Le Hub écrit
`nrf-mod.ini` à partir du champ `module`. Compiler votre DLL ne publie pas le
projet et ne l'ajoute pas automatiquement au catalogue public.

Pour un essai local hors Hub, jeu fermé, vous pouvez copier votre paquet dans
`<dossier du jeu>/NRFMods/MonPremierMod`, puis placer une copie de `mod.txt` et
de `imgs` dans un dossier `MonPremierMod` sous
`%USERPROFILE%/Saved Games/Weird and Wry/NIMBY Rails/mods/`.
N'écrasez pas une installation ou une jonction déjà gérée par le Hub. Les
copies manuelles seront à mettre à jour ou à retirer manuellement ; ce n'est
pas un mécanisme d'import du Hub.

## 10. Vérifier ce que vous avez construit

Lancez le jeu normalement, avec le NRF Loader compatible installé. Consultez
`%LOCALAPPDATA%/NimbyRailsFranceSDK/proxy.log` pour vérifier le démarrage du
module. Activez vos ressources dans le jeu et chargez une partie pour les utiliser.

Un message `OK: NRF mod started` confirme le démarrage d'un module ; s'il y en
a plusieurs, il ne permet pas à lui seul d'identifier lequel. Il ne prouve pas
non plus qu'une texture a été affichée. Pour cette dernière vérification,
déclenchez votre action sur le signal choisi après chargement de la partie,
observez le résultat dans le jeu puis essayez la restauration.

| Symptôme | Vérification |
|---|---|
| CMake ne trouve pas le SDK | `CMAKE_PREFIX_PATH` pointe vers le kit installé |
| La cible `::Mod` manque | Le kit contient bien l'adaptateur récent |
| Le loader ne trouve pas la DLL | `library` correspond au fichier présent près de `nrf-mod.ini` |
| Le module démarre sans rien changer | Aucune commande d'affichage n'a encore été déclenchée |
| La texture est indisponible | La partie, le paquet, l'identifiant et le chemin sont corrects |

Le forçage est visuel et lié à la session. Il ne change pas les permissions des
trains et n'ajoute pas de règles de signalisation à lui seul.

Pour aller plus loin : [contrat du loader](mod-loader.md) et
[commandes de textures](texture-commands.md).
