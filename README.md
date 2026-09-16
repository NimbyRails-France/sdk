# NimbyRailsSDK 0.6.6

La version 0.6.6 corrige le compteur de voyageurs : il lit les voyageurs embarqués, et non la capacité du train.

La version 0.6.5 ajoute les noms de quai et la [vue des occupations par gare](docs/platform-occupations.md), avec trains presents et reservations separes.

La version 0.6.4 ajoute les voyageurs, les identifiants de service affecté et le plan des arrêts de ligne avec temps relatifs. Voir [les données détaillées](docs/research/train-line-plan.md).

La version 0.6.3 expose les états de service des trains, les arrêts actifs,
la localisation au dépôt quand elle est confirmée, et les comptes à rebours
dans le temps du jeu. Voir [les sources et limites](docs/research/train-service.md).

Les sources 0.6.2 résolvent aussi les noms automatiques des gares depuis le
cache natif du jeu. Les noms personnalisés restent prioritaires.

Les sources 0.6.1 ajoutent la vitesse d'affichage native pour les trains sans
Drive : `0 km/h`, avec un indicateur explicite de valeur par défaut.

SDK Windows x64 pour créer des outils qui observent NIMBY Rails : trains,
vitesses, voies, gares, signaux, textures, Paths, réservations et occupation natives.
Il fournit une DLL, une API C versionnée et des exemples C++20.

## Documentation

**[Commencer par le tutoriel C++ : captures automatiques et getters](docs/tutorial-cpp-client.md)**

La couche C++20 `<nimby/client.hpp>` fournit `Client::connect()`, un rafraîchissement
automatique à **4 Hz**, `getAllTrains()`, `getTrainById()` et les relations du réseau.
Les valeurs inconnues utilisent `std::optional` ; les captures restent immuables.
Voir [tous les types de retour](docs/cpp-api-reference.md). L'API C reste accessible.

```cpp
auto client = nimby::Client::connect();
client.startAutoRefresh(); // 250 ms par défaut
if (client.waitForSnapshot(std::chrono::seconds{2})) {
    auto snapshot = client.latest();
    for (const auto& train : snapshot->getAllTrains()) {
        if (auto speed = train.getSpeedKmh()) {
            std::cout << train.getName() << " : " << *speed << " km/h\n";
        }
    }
}
```

Extrait à placer dans un programme qui inclut `<nimby/client.hpp>` et `<iostream>`
et traite les exceptions. Le [tutoriel](docs/tutorial-cpp-client.md) fournit le programme complet.

| Besoin | Guide |
|---|---|
| Découvrir les parcours et exemples | [Sommaire de la documentation](docs/README.md) |
| Compiler son premier programme avec CLion ou PowerShell | [Tutoriel C++](docs/tutorial-cpp-client.md) |
| Chercher un getter et son type de retour | [Référence des helpers C++](docs/cpp-api-reference.md) |
| Intégrer le SDK dans une application | [Guide développeur](docs/developing.md) |
| Chercher une fonction, une structure ou un code d'erreur | [Référence de l'API](docs/api-reference.md) |
| Résoudre un problème | [Dépannage](docs/troubleshooting.md) |
| Exploiter les états et textures des signaux | [Signaux](docs/signal-states.md) |
| Travailler sur le SDK dans CLion | [Configuration du dépôt](docs/clion.md) |

## Choisir son installation

| Objectif | Paquet / outil |
|---|---|
| Développer un programme externe | **NimbyRailsSDK-0.6.6-windows-x64-mingw.zip** |
| Charger le SDK au démarrage du jeu | **NimbyRailsSDK-0.6.6-drop-in-windows-x64.zip** |
| Gérer SDK et TCO ensemble | [NRF Hub](https://github.com/NimbyRails-France/hub) |
| Utiliser simplement le TCO | [TCO prêt à lancer](https://github.com/NimbyRails-France/tco/releases) |

Les deux ZIP autonomes sont dans les
[assets de la release SDK 0.6.6](https://github.com/NimbyRails-France/sdk/releases/tag/v0.6.6).
Le kit MinGW contient `include/`, `lib/`, `bin/` et `share/`.
Le paquet du Hub contient aussi un sous-dossier `loader/`.

**L'observation externe fonctionne sans proxy ni injection.** Pour le chargement
dans le jeu, suivre [l'installation du proxy](docs/install-drop-in.md) : sa SDL
originale doit être conservée, pas écrasée directement. Les installations gérées
par le Hub se mettent à jour et se désinstallent depuis le Hub.

## Exemple bas niveau

Le projet autonome [examples/first-observer](examples/first-observer) vérifie la
version du SDK puis affiche les trains, les vitesses et les voies de cinq captures.
Copier ses deux fichiers dans un dossier de travail. Avec MinGW x64, CMake et
Ninja dans `PATH`, depuis ce dossier :

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=C:/SDK/NimbyRailsSDK-0.6.6
cmake --build build
./build/MyFirstNimbyTool.exe --check-sdk
Get-Process -Name NIMBYRails | Select-Object Id, ProcessName
# Remplacer 12345 par le PID du jeu, avec une partie chargee :
./build/MyFirstNimbyTool.exe 12345
```

Le [tutoriel](docs/tutorial-first-tool.md) explique l'installation des outils,
le code, les résultats et les erreurs. Les trois exemples sont inclus dans le
paquet 0.6.6 et requièrent sa DLL. L'exemple [observer](examples/observer)
présente aussi les signaux et les portions réservées/occupées.

## Compatibilité et données disponibles

- Windows x64 ; le kit publié utilise MinGW GCC 15.2. Pour MSVC, reconstruire
  le SDK avec MSVC x64 afin d'obtenir sa bibliothèque d'import.
- API publique d'observation en lecture seule, ABI 1. SDK 0.6 ajoute
  [`NimbySdk_GetVersion`](docs/sdk-version.md).
- Binaire du jeu reconnu par SHA-256 :
  `fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae`.
- Les captures sont expérimentales et non atomiques. Une donnée indisponible
  reste inconnue ; elle ne signifie pas zéro ou absence.
- Les réservations natives ne sont pas un itinéraire ordonné ; les réservations
  virtuelles des scripts ne sont pas exposées. Les Paths sont un composant distinct.
- Les états et textures des signaux sont lus, mais leur interprétation générale
  rouge/vert ou selon les règles françaises n'est pas établie.
- Aucun setter de vitesse, commande réseau ou hook de simulation n'est exposé
  par l'API publique.

Le moniteur historique conserve un bouton expérimental d'écriture ponctuelle
de vitesse ; il est distinct de cette API. Voir les
[notes de recherche sur les vitesses](docs/research/speed-experiments.md).

## Compiler et tester le SDK

Depuis la racine du dépôt :

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools/build.ps1 -Configuration Release
```

Ajouter `-ClionHome 'chemin/CLion'` si nécessaire. Le script compile et exécute
les tests. `tools/package.ps1` construit le kit, installe la documentation et
vérifie les trois exemples comme consommateurs autonomes.

L'API publique est dans `include/nimby/`, l'implémentation dans `src/`, les
tests dans `tests/` et les preuves dans `docs/research/`. MinHook est compilé
dans la DLL ; conserver les runtimes livrés avec le kit, dont `libwinpthread-1.dll`.
Voir [le guide développeur](docs/developing.md) pour le contrat et la distribution.
