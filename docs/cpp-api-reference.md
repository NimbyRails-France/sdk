# Référence C++ : objets et types de retour

[Documentation](README.md) · [Tutoriel C++](tutorial-cpp-client.md) · [Migration 0.7](migration-0.7.md)

Inclure `<nimby/client.hpp>` et compiler en **C++20 / Windows x64**.
Les helpers sont définis dans ce header et utilisent la DLL **NimbyRailsFranceSDK 0.7.x / pont interne 2**.
Ils sont compilés dans votre application ; aucun objet C++ ne traverse l'ABI de la DLL.

## Horloge globale

Depuis 0.7.1, `snapshot->getSimulationClock()` renvoie une
`std::optional<SimulationClock>` avec date UTC et durée écoulée.
`client.setSimulationDateTime(std::chrono::sys_seconds)` modifie la date
de façon expérimentale et renvoie l'horloge écrite.
`client.setSimulationDateTimeAndRecalculateTrains(std::chrono::sys_seconds)`
applique ensuite une intervention native à tous les trains éligibles. Il renvoie
`SimulationTimeChange`, avec `clock` et `interventions`. Les services et attentes
sont recalculés ; les voyageurs et positions suivent le comportement natif du
bouton « All trains intervention », avec son coût en jeu.
Voir les [exemples, limites et tests réels](simulation-clock.md).

## Service et horaires d'un train

`snapshot->getAllTrainServices()` renvoie `std::span<const TrainService>`.
`snapshot->getTrainServiceById(id)` renvoie `std::optional<TrainService>`.
Les objets sont des copies ; les vues conservent les règles de durée de vie
des autres tables du snapshot.

```cpp
if (auto service = snapshot->getTrainServiceById(train.getId())) {
    std::cout << service->getStatusName();
    if (auto seconds = service->getDepartureRemainingSeconds())
        std::cout << " | départ dans " << *seconds << " secondes de jeu";
    if (auto stationId = service->getLocationStationId()) {
        if (auto station = snapshot->getStationById(*stationId))
            std::cout << " | " << station->getName().value_or("nom inconnu");
    }
}
```

Les getters `getStatus`, `getMotionFlags`, `getAlert`, `getLineId`,
`getLineName`, `getStopIndex`, `getStopTrackId`, `getStopStationId`,
`getLocationTrackId` et `getLocationStationId` renvoient des `optional`.
`getStatusName()` fournit un libellé, dont `unknown` si non observé.
L'arrêt est la cible pendant la conduite et l'arrêt actuel pendant RunStop.

`getGameTimeUs`, `getArrivalTimeUs`, `getDepartureTimeUs` renvoient les
microsecondes de simulation. `getGameCalendarSeconds`,
`getArrivalCalendarSeconds`, `getDepartureCalendarSeconds` renvoient les
secondes du calendrier du jeu, sans fuseau local de l'ordinateur.
Les trois getters `getArrivalRemainingSeconds`, `getDepartureRemainingSeconds`
et `getDispatchRemainingSeconds` renvoient des `optional<double>`.
Ils exigent une horloge valide ; ne pas décrémenter ces valeurs avec l'horloge
réelle quand le jeu est en pause ou accéléré. Recapturer un snapshot.

Le départ est disponible pendant un arrêt temporisé ; les départs futurs
de toute la grille horaire ne sont pas exposés. Voir les
[limites de lecture](research/train-service.md).
`nimby::getVersion()` vérifie la DLL sans ouvrir le jeu et renvoie `Version`.
`TrainService::isOnNetwork()` et `isHidden()` renvoient `optional<bool>` :
inconnu reste distinct de faux. La compatibilité avec les headers et DLL 0.6 est retirée.
Les constructeurs utilisant des structures natives et les fichiers `detail/`
sont des détails d'implémentation, pas des points d'entrée consommateurs.

## Conventions de types

| Type | Signification |
|---|---|
| `nimby::Id` | Alias de `std::uint64_t` ; conserver les 64 bits des IDs |
| `nimby::Milliseconds` | Alias de `std::chrono::milliseconds` |
| `std::optional<T>` | Valeur présente ou `std::nullopt` ; tester avant de déréférencer |
| `std::vector<T>` | Tableau possédé par l'appelant ; la copie reste valide indépendamment du snapshot |
| `std::span<const T>` | Vue en lecture seule, sans copie ; valide tant que le snapshot propriétaire existe |
| `nimby::Snapshot::Ptr` | Alias de `std::shared_ptr<const nimby::Snapshot>` ; ownership partagé d'une capture immuable |
| `std::string` | Copie de chaîne UTF-8, possédée par l'appelant |

Toutes les méthodes de lecture des objets sont `const`. Les objets `Train`,
`Track`, etc. possèdent une copie de leur structure C ; ils n'empruntent pas
la mémoire du jeu ou de la DLL. Les résultats `optional<Train>`, `vector<Signal>`
et les chaînes peuvent être conservés après destruction du snapshot.

Une **vue** `span`, ou une référence obtenue pendant son parcours, nécessite
en revanche de conserver le `Snapshot::Ptr` propriétaire :

```cpp
auto snapshot = client.latest();
if (snapshot) {
    auto trains = snapshot->getAllTrains();
    // snapshot reste vivant pendant toute l'utilisation de trains.
}
```

Ne pas conserver seulement le résultat de `client.latest()->getAllTrains()` :
le rafraîchissement pourrait remplacer le dernier propriétaire de cette capture.

## Erreurs : modèle retenu

La syntaxe directe `auto client = Client::connect()` utilise des **exceptions
pour les opérations synchrones**, plutôt qu'un objet `Result<Client>`.

- Erreur du SDK, argument incorrect ou détection impossible : `nimby::Exception`.
- Erreurs C++ usuelles (allocation, création de thread) : exceptions standard.
- Pendant le rafraîchissement, aucune exception ne sort du worker ; consulter
  `getLastError()`. Une capture réussie efface cette erreur.
- Une valeur absente attendue, comme une vitesse inconnue, utilise `optional`
  et ne déclenche pas d'exception.

`nimby::Exception` dérive de `std::runtime_error`. Sa méthode
`error() const noexcept → const nimby::Error&` expose :

| Champ de `Error` | Type | Contenu |
|---|---|---|
| `code` | `std::uint32_t` | Valeur de `nimby::ErrorCode` ; `Exception::code()` fournit cet enum |
| `operation` | `std::string` | Opération concernée |
| `message` | `std::string` | Diagnostic anglais |

`what()` donne un `const char*` combinant opération et message, valable tant que
l'exception existe. `getLastError()` renvoie une copie indépendante.

## Client

Un client possède une session native et au maximum un worker. Il n'est ni
copiable ni déplaçable ; `connect()` le construit directement grâce à C++20.
Sa destruction arrête et joint le worker puis ferme la session.

| Signature | Retour | Comportement |
|---|---|---|
| `static connect()` | `Client` | Recherche par nom de processus NIMBYRails.exe ; erreur si zéro ou plusieurs candidats ; le runtime valide ensuite le binaire |
| `static connect(std::uint32_t pid)` | `Client` | Ouvre le PID choisi ; zéro est refusé par les helpers |
| `getSdkVersion()` | `Version` | Copie de la version vérifiée à la connexion |
| `getProcessId()` | `std::uint32_t` | PID associé à la session |
| `getConnectionState()` | `ConnectionState` | État connu lors de l'ouverture ou de la dernière capture |
| `getLastError()` | `std::optional<Error>` | Dernière erreur de capture, manuelle ou automatique ; vide après un succès |
| `getRefreshInterval()` | `Milliseconds` | Intervalle configuré ; initialement 250 ms |
| `isAutoRefreshRunning()` | `bool` | Worker démarré et non arrêté |
| `startAutoRefresh()` | `void` | Démarre avec l'intervalle configuré ; sans effet si déjà démarré |
| `startAutoRefresh(Milliseconds interval)` | `void` | Configure l'intervalle puis démarre ; si actif, met à jour sa cadence |
| `stopAutoRefresh()` | `void` | Réveille l'attente et joint le worker ; attend la fin d'une capture déjà en cours |
| `setRefreshInterval(Milliseconds interval)` | `void` | Accepte de 1 ms à 24 h ; la capacité à tenir cet intervalle n'est pas garantie |
| `waitForSnapshot(Milliseconds timeout)` | `bool` | Attend qu'une capture existe, jusqu'au timeout ; true si disponible ; timeout négatif refusé |
| `latest()` | `Snapshot::Ptr` | Dernière capture réussie, ou pointeur nul avant le premier succès |
| `capture()` | `Snapshot::Ptr` | Capture synchrone, met aussi à jour latest ; lève une exception en cas d'échec |

`Version` possède quatre champs `std::uint32_t` : `major`, `minor`, `patch`,
`abi`. `ConnectionState` possède `Connected` et `ProcessExited`.

**Connected est un état connu, pas une surveillance indépendante du processus.**
La fermeture du jeu est constatée à la prochaine capture. Les autres erreurs,
comme des données indisponibles, restent dans `getLastError()`. Il n'y a pas de
reconnexion automatique ; après redémarrage du jeu, créer un nouveau client.
Le worker continue ses tentatives jusqu'à `stopAutoRefresh()` ou destruction.

`waitForSnapshot()` ne démarre pas le worker et n'attend pas nécessairement une
**nouvelle** capture : s'il en existe déjà une, même ancienne, il renvoie true.
Il peut terminer plus tôt si le processus est connu comme fermé.
Examiner `getAge()` ou comparer les `Snapshot::Ptr` pour suivre les nouveautés.

### Cadence et threads

La première capture automatique commence immédiatement. Ensuite, la cadence cible
est de 250 ms entre débuts de capture. Les échéances dépassées sont sautées.
Une modification d'intervalle pendant l'attente recalcule la prochaine échéance.

Les captures manuelles et automatiques du même client sont sérialisées.
Les getters du client et les contrôles du worker sont synchronisés. Une capture
manuelle peut retarder le worker ; éviter de la lancer périodiquement en plus
du mode automatique. Les appels du pont interne sont aussi sérialisés dans la DLL.

Avant de détruire le client, arrêter vos propres threads qui l'utilisent.
Les snapshots déjà conservés restent consultables après sa destruction.

## Snapshot : métadonnées

| Méthode | Retour | Unité / sens |
|---|---|---|
| `getProcessId()` | `std::uint32_t` | PID observé |
| `getCapturedAt()` | `std::chrono::system_clock::time_point` | Heure UTC de fin de capture native |
| `getAge()` | `Milliseconds` | Durée monotone depuis le retour de la capture native ; indépendante d'une modification de l'horloge Windows |
| `isOlderThan(Milliseconds age)` | `bool` | Âge strictement supérieur au seuil |
| `getGameSha256()` | `std::string` | Empreinte hexadécimale du jeu |

La capture est immuable, mais **n'est pas un tick atomique du moteur**.
Elle contient une copie des collections et des Paths. Le handle C temporaire est
libéré avant de publier le snapshot C++ ; garder plus de 16 captures C++ ne retient
donc pas 16 handles natifs, mais consomme de la mémoire dans votre application.

## Snapshot : collections et recherches

- `getAllTrains() → std::span<const Train>`
- `getTrainById(Id id) → std::optional<Train>`
- `getAllTracks() → std::span<const Track>`
- `getTrackById(Id id) → std::optional<Track>`
- `getAllStations() → std::span<const Station>`
- `getStationById(Id id) → std::optional<Station>`
- `getAllSignals() → std::span<const Signal>`
- `getSignalById(Id id) → std::optional<Signal>`
- `getAllTrackNodes() → std::span<const TrackNode>`
- `getTrackNodeById(Id id) → std::optional<TrackNode>`
- `getAllSignalStates() → std::span<const SignalState>`
- `getSignalStateById(Id id) → std::optional<SignalState>`
- `getAllSignalTextures() → std::span<const SignalTexture>`
- `getSignalTextureById(Id id) → std::optional<SignalTexture>`

Les recherches individuelles sont indexées par ID complet, de coût moyen O(1).
Un ID absent renvoie `nullopt`, jamais un objet factice. Les listes peuvent être vides.
Les getters n'appellent pas la DLL et ne déclenchent aucune lecture du jeu.

### Relations

| Méthode | Retour |
|---|---|
| `getTrackForTrain(Id trainId)` | `std::optional<Track>` |
| `getStationForTrack(Id trackId)` | `std::optional<Station>` |
| `getSignalsForTrack(Id trackId)` | `std::vector<Signal>` |
| `getTrainsOnTrack(Id trackId)` | `std::vector<Train>` |
| `getTracksForStation(Id stationId)` | `std::vector<Track>` |
| `getPathTrackIdsForTrain(Id trainId)` | `std::optional<std::vector<Id>>` |

Les relations individuelles sont absentes si l'objet, la référence ou la position
nécessaire manque. Les listes filtrées parcourent la collection en O(n) et
retournent une copie ; pour un ID inconnu, elles sont vides. Les trains dont la
position est inconnue ne figurent pas dans `getTrainsOnTrack()`.

La gare de la voie n'est pas la destination du train. Un Path inconnu ou un train
absent donne `nullopt` ; un Path observé vide donne un vector vide. Son ordre
reste celui du vecteur natif, sans garantie sur l'itinéraire restant.

### Réservations et occupations

| Méthode | Retour |
|---|---|
| `getAllReservations()` | `std::optional<std::span<const TrackUsage>>` |
| `getAllOccupations()` | `std::optional<std::span<const TrackUsage>>` |
| `getReservationsForTrain(Id trainId)` | `std::optional<std::vector<TrackUsage>>` |
| `getReservationsForTrack(Id trackId)` | `std::optional<std::vector<TrackUsage>>` |
| `getOccupationsForTrain(Id trainId)` | `std::optional<std::vector<TrackUsage>>` |
| `getOccupationsForTrack(Id trackId)` | `std::optional<std::vector<TrackUsage>>` |

`nullopt` signifie **composant inconnu** ; un tableau présent mais vide signifie
**aucune portion observée correspondant à la requête**. Pour vérifier l'existence
du train ou de la voie, utiliser séparément sa recherche par ID.
Les deux composants sont disponibles indépendamment ; leurs intervalles peuvent
se recouvrir. Ils ne prouvent ni une autorisation de passage ni un aspect de signal.
Les réservations virtuelles des scripts ne sont pas exposées.

## Objets : signatures exactes

Dans les tableaux suivants, tous les getters retournent des valeurs possédées,
pas des références empruntées.

### Position

| Méthode | Retour |
|---|---|
| `getTrackId()` | `Id` |
| `getFraction()` | `double` |
| `getDirection()` | `std::int32_t` |

La fraction est normalisée dans [0,1]. Le sens +1/-1 est relatif à l'orientation
de la voie, pas une direction géographique.

### Train

| Méthode | Retour |
|---|---|
| `getId()` | `Id` |
| `getName()` | `std::string` |
| `getSpeedMps()` | `std::optional<double>` |
| `getSpeedKmh()` | `std::optional<double>` |
| `isSpeedDefaulted()` | `bool` : zéro d'affichage natif sans Drive, depuis SDK 0.6.1 |
| `getPosition()` | `std::optional<Position>` |

La vitesse est optionnelle indépendamment de la position. Une vitesse inconnue n'est pas zéro. `getName()` conserve le nom UTF-8 du SDK ; il peut être vide.

### Track

| Méthode | Retour |
|---|---|
| `getId()` | `Id` |
| `getStationId()` | `std::optional<Id>` |
| `getSpeedLimitMps()` | `double` |
| `getSpeedLimitKmh()` | `double` |

La limite de vitesse est une propriété de voie, pas une consigne de train. `getStationId()` est absent lorsque la référence native vaut zéro.

### Station

| Méthode | Retour |
|---|---|
| `getId()` | `Id` |
| `getName()` | `std::optional<std::string>` |

`getName()` renvoie le nom personnalisé ou automatique sélectionné par le jeu
(résolution des noms automatiques depuis SDK 0.6.2). Il renvoie `nullopt` si le nom
est vide ou si le cache automatique n'a pas pu être lu de manière cohérente.

### Signal

| Méthode | Retour |
|---|---|
| `getId()` | `Id` |
| `getTrackId()` | `Id` |
| `getFraction()` | `double` |
| `getDirection()` | `std::int32_t` |
| `getKind()` | `std::int32_t` |

Le type est une valeur native (0 : sens unique, 1 : arrêt quai, 3 : balise, 4 : path, 5 : interdit, 6 : marqueur) ; il ne représente pas la couleur ou la permission de passage.

### SignalState

| Méthode | Retour |
|---|---|
| `getSignalId()` | `Id` |
| `getAspect()` | `std::optional<std::uint32_t>` |
| `getSpecificState()` | `std::optional<SpecificState>` |
| `getTextureSelector()` | `std::optional<std::int32_t>` |
| `usesDefaultTextureSelector()` | `bool` : zero choisi par le rendu pour un ID absent d'une table valide |

Chaque optional est commandé par son propre bit de validité. Un sélecteur de texture égal à zéro peut être valide. `SpecificState` possède deux champs `std::string` : `system` et `state`. L'aspect général reste inconnu dans l'adaptateur actuel ; les valeurs générales sont 0 inconnu, 1 arrêt, 2 prudence, 3 passage, 4 éteint.

### TextureReference

| Méthode | Retour |
|---|---|
| `getTexturesId()` | `std::string` |
| `getTexturesHash()` | `Id` |
| `getFileHash()` | `Id` |
| `getSelectedIndex()` | `std::int32_t` |
| `getStateCount()` | `std::uint32_t` |
| `getSource()` | `std::uint32_t` |
| `getModId()` | `std::string` |
| `getRelativePath()` | `std::string` |
| `isDefaultSet()` | `bool` |
| `isClamped()` | `bool` |

Les hashes sont des identifiants natifs, pas des SHA-256. `getSource()` vaut 0 (intégré), 1 (local), 2 (Workshop), ou une valeur future à traiter. Les booléens indiquent le repli par défaut et la limitation du sélecteur.

### SignalTexture

| Méthode | Retour |
|---|---|
| `getSignalId()` | `Id` |
| `getReference()` | `std::optional<TextureReference>` |
| `getFilePath()` | `std::optional<std::string>` |

Référence du catalogue et chemin local sont optionnels séparément. Même avec un chemin valide à la capture, gérer l'échec ultérieur du chargement du fichier.

### TrackNode

| Méthode | Retour |
|---|---|
| `getId()` | `Id` |
| `getCoordinates()` | `Coordinates` |
| `getLinkAId()` | `std::optional<Id>` |
| `getLinkBId()` | `std::optional<Id>` |

`Coordinates` possède deux champs `double`, `x` et `y`, en unités projetées natives. Les liens absents valent `nullopt` ; ils ne prouvent pas un butoir. Le graphe et ses jonctions restent partiels.

### TrackUsage

| Méthode | Retour |
|---|---|
| `getTrainId()` | `Id` |
| `getTrackId()` | `Id` |
| `getBeginFraction()` | `double` |
| `getEndFraction()` | `double` |

Les deux fractions sont des `double` normalisés dans [0,1], début ≤ fin. Une portion concerne un train et une voie ; la collection n'est pas un itinéraire ordonné.

## Réglages et commandes

Les réglages du client sont disponibles via `setRefreshInterval()`, le démarrage
et l'arrêt du rafraîchissement. Les snapshots et leurs objets n'ont pas de setters.

**Cette couche n'ajoute aucune écriture dans le jeu.** Les commandes envisagées
de renommage, pause ou simulation ne sont pas implémentées. Le pont sous
`detail/` est privé ; utiliser uniquement l'API C++ publique.

## Coût des helpers

Une capture C++ copie les collections du snapshot natif, construit les index,
copie les Paths disponibles puis libère le handle C. Elle relit actuellement
tout le réseau à chaque capture, comme le lecteur natif. Il n'y a pas de cache indépendant
du réseau fixe. Les vues `getAll*` ne copient pas à chaque appel ; les recherches
individuelles et filtres renvoient des copies possédées.

4 Hz est une valeur par défaut, pas une garantie de performance sur toute taille
de partie. Mesurer sur les cartes visées ; la fluidité du rendu du TCO peut être
traitée séparément par interpolation.

## Données détaillées et plan de ligne (0.6.4)

`Snapshot::getTrainDetailsById(id)` fournit `getPassengerCount()`, `getScheduleId()`,
`getShiftId()` et `getOrderIndex()` sous forme d'optionnels. Le mode brut du modèle
est accessible via `getOrderMode()`.

`getLineStopsForTrain(id)` renvoie une vue optionnelle sur le plan complet de la
ligne associée à la course active. Conserver le snapshot propriétaire de la vue.
Chaque `LineStop` expose les IDs de ligne, voie et gare, l'index à partir de zéro,
les offsets relatifs d'arrivée/départ et `getPlannedDwellSeconds()`.
Ce plan inclut les arrêts déjà passés ; une course partielle peut en desservir un
sous-ensemble. Il ne constitue pas une prédiction d'horaires absolus.

## Quais par gare (0.6.5)

`getPlatformOccupationsForStation(stationId)` regroupe les troncons de meme nom de quai dans cette gare et dedoublonne les trains occupants et reservant. `platform.getTrackIds()` et `containsTrack(id)` couvrent tout le quai ; `getTrackId()` en represente seulement le premier troncon. `getPlatformSectionOccupationsForStation(stationId)` retourne le detail par troncon. Voir [contrat, exemple et etats inconnus](platform-occupations.md).
