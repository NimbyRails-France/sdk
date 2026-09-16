# Tutoriel : un client C++ avec captures automatiques

[Documentation](README.md) · [Tous les types et getters](cpp-api-reference.md)

Ce parcours est l'entrée recommandée pour créer un outil C++.
Il utilise `nimby::Client`, la détection du jeu et un rafraîchissement automatique
à **4 Hz**, soit une échéance toutes les **250 ms**.
Pour gérer vous-même les buffers et handles C, voir le
[tutoriel bas niveau](tutorial-first-tool.md).

## 1. Préparer le kit

Prévoir Windows x64, CMake 3.24+, Ninja et MinGW x64 avec C++20.
La chaîne testée est MinGW GCC 15.2 fournie avec CLion.

Les helpers nécessitent :

- `include/nimby/client.hpp` de cette révision du SDK ;
- les headers C et les DLL du kit SDK **0.6.x / ABI 1** ;
- l'exemple complet `examples/auto-observer`.

Le ZIP 0.6.0 publié avant ces helpers ne contient pas encore `client.hpp`.
Pour préparer un kit complet depuis les sources, exécuter à la racine du SDK :

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools/package.ps1
```

Le résultat est dans `dist/NimbyRailsSDK-0.6.0/`, avec la documentation sous
`share/doc/NimbyRailsSDK/` et les exemples sous
`share/NimbyRailsSDK/examples/`. Ajouter `-ClionHome 'C:/chemin/CLion'` si nécessaire.
Cette opération construit un paquet local ; elle ne met pas à jour les anciennes
releases et ne modifie pas le jeu.

Vous pouvez aussi ajouter le nouveau `client.hpp` dans `include/nimby/` d'une
**copie de travail** du kit 0.6.0 et utiliser le nouvel exemple : les helpers
n'exigent pas de nouvel export dans sa DLL. Conserver les fichiers gérés par
le Hub dans leur installation d'origine.

## 2. Compiler l'exemple complet

Copier les deux fichiers du dossier `examples/auto-observer` dans votre projet :

```text
MyNimbyClient/
├── CMakeLists.txt
└── main.cpp
```

Ils sont aussi disponibles dans les
[sources de l'exemple](https://github.com/NimbyRails-France/sdk/tree/main/examples/auto-observer)
lorsque cette révision est publiée.

Depuis ce nouveau dossier, avec le compilateur, CMake et Ninja dans `PATH` :

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=C:/SDK/NimbyRailsSDK-0.6.0
cmake --build build
./build/MyNimbyClient.exe --check-sdk
```

Remplacer `C:/SDK/NimbyRailsSDK-0.6.0` par la racine de votre kit préparé à
l'étape 1. Le résultat attendu du contrôle est :

```text
C++ helpers ready | SDK 0.6.0 | ABI 1
```

Dans **CLion**, ouvrir le dossier comme projet, sélectionner MinGW x64,
ajouter `-DCMAKE_PREFIX_PATH=...` aux options CMake, puis compiler et lancer
la cible **MyNimbyClient**. Les détails pour mettre les outils CLion dans
`PATH` sont dans le [premier tutoriel](tutorial-first-tool.md#3-compiler).

## 3. Lire la partie ouverte

Lancer NIMBY Rails et charger une partie, puis :

```powershell
./build/MyNimbyClient.exe
```

Sans argument, l'exemple détecte le jeu. Si plusieurs instances existent :

```powershell
Get-Process -Name NIMBYRails | Select-Object Id, Path
./build/MyNimbyClient.exe 12345
```

Remplacer `12345` par le PID choisi. Le PID est un entier 32 bits Windows ;
il est différent des IDs 64 bits des trains et voies.

L'exemple affiche cinq fois la dernière capture, puis arrête son worker.
Il signale si deux affichages utilisent la même capture. Une attente de 250 ms
dans l'affichage ne garantit pas qu'une nouvelle capture a réussi entre-temps.

Sortie illustrative :

```text
PID 12345 | age=4 ms | new snapshot
TER 01 | id=1407374883553283 | 72 km/h | track=281474976710657 | reservations=3
TER 02 | id=1407374883618819 | speed unavailable | track=281474976710658 | reservations unknown
Stopped; retained trains=2
```

Les nombres dépendent de votre partie. Les captures sont expérimentales et
en lecture seule ; une donnée inconnue est affichée comme telle.

## 4. Le minimum pour commencer

Voici un programme autonome plus court. Vous pouvez remplacer le `main.cpp`
de l'exemple par ce code en gardant son `CMakeLists.txt` :

```cpp
#include <nimby/client.hpp>
#include <iostream>

using namespace std::chrono_literals;

int main() {
    try {
        auto client = nimby::Client::connect();
        client.startAutoRefresh(); // 250 ms par défaut

        if (!client.waitForSnapshot(2s)) {
            if (auto error = client.getLastError())
                std::cerr << error->operation << ": " << error->message << '\n';
            return 1;
        }

        auto snapshot = client.latest(); // shared_ptr<const Snapshot>
        for (const auto& train : snapshot->getAllTrains()) {
            std::cout << train.getName(); // std::string UTF-8
            if (auto speed = train.getSpeedKmh()) // optional<double>
                std::cout << " : " << *speed << " km/h";
            else
                std::cout << " : vitesse inconnue";
            std::cout << '\n';
        }
        // Le destructeur du client arrête le worker et ferme la session.
        return 0;
    } catch (const nimby::Exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
```

`startAutoRefresh()` lance une première capture immédiatement.
`waitForSnapshot()` attend qu'il y en ait une ; il ne garantit pas sa fraîcheur
lorsqu'une ancienne capture existe déjà.

## 5. Trouver un train par ID

Les extraits suivants s'insèrent après l'obtention d'un `snapshot` non nul.
Choisir un ID présent dans `getAllTrains()`, ou conserver celui sélectionné
dans votre interface :

```cpp
nimby::Id trainId = 1407374883553283ULL; // Exemple : remplacer par un ID observé
std::optional<nimby::Train> train = snapshot->getTrainById(trainId);

if (train) {
    std::cout << train->getName() << '\n';
} else {
    std::cout << "Train absent de cette capture\n";
}
```

L'objet retourné est une **copie possédée**, donc il peut survivre au snapshot.
Il représente toujours l'état de cette ancienne capture ; le conserver ne le
met pas automatiquement à jour. Les getters ne font pas de lecture du jeu.

## 6. Retrouver la voie et sa gare

```cpp
if (auto track = snapshot->getTrackForTrain(trainId)) {
    std::cout << "Voie " << track->getId() << '\n';
    if (auto station = snapshot->getStationForTrack(track->getId())) {
        std::cout << station->getName().value_or("Nom automatique inconnu") << '\n';
    }
}
```

La relation utilise les données de la même capture. Elle retourne `nullopt`
si la position, la référence ou l'objet nécessaire manque. La gare d'une voie
ne désigne pas le prochain arrêt ou la destination du train.

## 7. Réservations et occupation

```cpp
auto portions = snapshot->getReservationsForTrain(trainId);
// Type : optional<vector<nimby::TrackUsage>>
if (!portions) {
    std::cout << "Réservations inconnues\n";
} else if (portions->empty()) {
    std::cout << "Aucune réservation observée pour cet ID\n";
} else {
    for (const auto& portion : *portions) {
        std::cout << portion.getTrackId()
                  << " : " << portion.getBeginFraction()
                  << " -> " << portion.getEndFraction() << '\n';
    }
}
```

L'occupation s'obtient avec `getOccupationsForTrain()`. Les bornes sont
normalisées entre 0 et 1. Ces portions ne constituent pas un itinéraire ordonné,
et une réservation ne donne pas la permission d'un signal.

## 8. Signaux : vérifier chaque donnée

```cpp
for (const auto& signal : snapshot->getAllSignals()) {
    if (auto state = snapshot->getSignalStateById(signal.getId())) {
        if (auto selector = state->getTextureSelector())
            std::cout << "Sélecteur : " << *selector << '\n';
        if (auto specific = state->getSpecificState())
            std::cout << specific->system << ':' << specific->state << '\n';
    }
    if (auto texture = snapshot->getSignalTextureById(signal.getId())) {
        if (auto path = texture->getFilePath())
            std::cout << "Fichier UTF-8 : " << *path << '\n';
    }
}
```

Un sélecteur zéro peut être valide ; les `optional` évitent de le confondre
avec une donnée inconnue. L'aspect ferroviaire général reste inconnu dans
l'adaptateur actuel. Le fichier peut disparaître après la capture : gérer
l'erreur lors de son chargement par votre interface.

## 9. Fréquence, erreurs et vieillissement

```cpp
client.setRefreshInterval(500ms); // Cible 2 Hz
client.stopAutoRefresh();        // Attend la fin d'une capture en cours
client.startAutoRefresh(250ms);  // Redémarre à 4 Hz

auto current = client.latest();
if (current && current->isOlderThan(1s))
    std::cout << "Affichage ancien\n";

if (auto error = client.getLastError())
    std::cerr << error->operation << ": " << error->message << '\n';
```

Le dernier snapshot réussi reste disponible si la lecture suivante échoue.
Il faut donc afficher son âge et retirer ou griser les informations devenues
anciennes selon les besoins de l'application. L'erreur disparaît au prochain
succès. La fermeture du jeu est signalée par `ConnectionState::ProcessExited`
lorsqu'une capture la constate ; créer un nouveau client après redémarrage.

La cadence est une cible : si une capture dépasse l'intervalle, les échéances
manquées sont sautées. Il n'y a pas d'accumulation de captures automatiques.

## 10. Capture manuelle

Sans démarrer le mode automatique :

```cpp
auto client = nimby::Client::connect();
nimby::Snapshot::Ptr snapshot = client.capture();
```

`capture()` peut lever une exception. Elle actualise également `latest()`.
On peut techniquement l'appeler pendant le mode automatique, mais elle attend
son tour et peut retarder le worker ; choisir habituellement un seul mode.

## Distribution et limites

Conserver les DLL du kit près de l'exécutable ; la commande CMake de copie
le fait pour l'exemple. Le header C++ est compilé dans votre outil, la DLL
continue d'exposer seulement l'ABI C. Voir le [guide développeur](developing.md)
pour les dépendances et licences.

Les helpers couvrent l'observation et les réglages du client. Les commandes
d'écriture dans le jeu ne sont pas implémentées.

La [référence C++](cpp-api-reference.md) liste **toutes les signatures et tous
les types de retour**. Le [dépannage](troubleshooting.md) traite aussi les erreurs
de DLL, le jeu non reconnu et les captures indisponibles.
