# Référence de l'API — SDK 0.6.x / ABI 1

Cette page décrit l'API C bas niveau. Pour les objets, getters et captures
automatiques C++ : [référence C++ et types de retour](cpp-api-reference.md).

## États de service (depuis 0.6.3)

`NimbySdk_CopyTrainServices(snapshot, records, capacity, required)` suit le
même contrat de copie que `CopyTrains` : comptage avec `NULL, 0`, aucune
copie partielle, données immuables appartenant au snapshot. Jointure par
`train_id` complet. Il y a un enregistrement par train, même si son état
est inconnu (`STATE_VALID` absent).

`NimbyTrainService` fournit le statut, les `motion_flags` indépendants,
l'alerte native, la localisation (y compris masquée), la ligne et l'arrêt
actif. Un arrêt actif peut être la cible pendant la conduite ; ne pas le
présenter systématiquement comme la gare où le train est arrêté.

Les bits `NIMBY_SERVICE_*_VALID` indiquent quels champs sont disponibles.
Les échéances `*_time_us` sont en microsecondes de simulation. Les délais
nécessitent aussi `CLOCK_VALID`. `CALENDAR_VALID` permet de convertir une
échéance vers le calendrier du jeu : `game_epoch_seconds + time_us / 1000000`.
Afficher cette heure sans conversion vers le fuseau horaire de l'ordinateur.
Le départ est lu pendant TimedStop, l'arrivée pendant Drive. Le délai de
dispatch signifie « prochaine tentative », pas « prochain départ ».

Les statuts distinguent conduite active, arrêt en gare confirmé, arrêt
temporisé, dépôt masqué confirmé, attente de dispatch, attente au signal,
train remisé (mothballed), absence des voies et autre état. Une valeur nulle
de vitesse ne sert jamais de preuve pour ces statuts.

Alertes : 0 aucune, 1 ligne fermée, 3 trajet introuvable, 4 ordres incorrects,
5 collision, 6 signal, 7 horaire fermé, 8 voies de dispatch occupées,
9 aucun service activé, 10 services déjà affectés.

[Sources natives, validation et limites](research/train-service.md).

[Documentation](README.md) · [Tutoriel](tutorial-first-tool.md) · [Dépannage](troubleshooting.md)

Les déclarations de référence sont `include/nimby/sdk.h` et
`include/nimby/observation.h`. Inclure `<nimby/observation.h>` suffit aussi à
inclure `sdk.h`. Les fonctions sont exportées en C, avec `__cdecl`, sur Windows x64.
Les exemples de cette page sont des extraits à intégrer dans votre programme.

## Règles communes

- Les fonctions renvoient un statut `uint32_t`, sauf `NimbySdk_StatusString`.
  `NIMBY_OK` vaut zéro. Toujours examiner le statut avant d'utiliser le résultat.
- `NimbySession` et `NimbySnapshot` sont des handles opaques sur 64 bits. Zéro
  est invalide. Ils ne sont ni des pointeurs ni des IDs d'objets du jeu.
- Les IDs de trains, voies et signaux sont opaques sur 64 bits. Conserver tous
  leurs bits, relier les collections d'une même capture et oublier les anciennes
  associations quand la session ou la partie change. Zéro indique une référence absente.
- Les buffers appartiennent à l'appelant, doivent être valides et correctement
  alignés. Les chaînes copiées sont UTF-8 et terminées par zéro ; ne pas les libérer
  séparément. Ignorer les champs `reserved`.
- Initialiser `struct_size` à `sizeof(...)` pour `NimbySdkVersion`,
  `NimbySnapshotInfo` et `NimbyBinaryInfo`. Les autres structures n'ont pas ce champ.
- Les appels d'observation sont sérialisés en interne. L'appelant reste responsable
  de la durée de vie des handles et de la synchronisation de ses propres buffers.

## Version et sessions

| Fonction | Entrées / résultat | Contrat |
|---|---|---|
| `NimbySdk_GetVersion(out)` | `NimbySdkVersion*` initialisé | Renvoie `abi_version`, `major`, `minor`, `patch` sans toucher au jeu ; disponible depuis 0.6.0 |
| `NimbySdk_OpenProcess(abi_version, pid, out)` | ABI d'observation, PID, `NimbySession*` | Ouvre le processus en lecture et valide son binaire ; met `*out` à zéro en cas d'échec si le pointeur est fourni |
| `NimbySdk_CloseSession(session)` | Handle vivant | Ferme l'accès au processus ; les snapshots déjà créés restent valides |
| `NimbySdk_CaptureSnapshot(session, out)` | Session, `NimbySnapshot*` | Produit une copie immuable ; sortie zéro sur échec ; peut échouer temporairement pendant les chargements |
| `NimbySdk_GetSnapshotInfo(snapshot, out)` | `NimbySnapshotInfo*` initialisé | Copie les métadonnées de la capture |
| `NimbySdk_ReleaseSnapshot(snapshot)` | Handle vivant | Libère la capture ; toute réutilisation du handle est invalide |
| `NimbySdk_StatusString(status)` | Statut | Texte anglais statique ; ne pas libérer ; valable jusqu'au déchargement de la DLL |

Passer `NIMBY_OBSERVATION_ABI_VERSION` à `OpenProcess`. `pid=0` désigne le
processus appelant, **pas une recherche automatique du jeu**. Un outil externe
doit fournir le PID du jeu. Limites de cette implémentation : 8 sessions et
16 snapshots vivants dans une instance de la DLL.

`CloseSession` et `ReleaseSnapshot` renvoient `NIMBY_INVALID_HANDLE` si le handle
est déjà fermé. `Initialize` et `Shutdown` ne créent ni ne libèrent ces ressources.

### Métadonnées : `NimbySnapshotInfo`

| Champs | Signification |
|---|---|
| `struct_size`, `abi_version` | Taille exacte et version ABI |
| `process_id` | PID observé |
| `flags` | `NIMBY_SNAPSHOT_EXPERIMENTAL` et `NIMBY_SNAPSHOT_NON_ATOMIC` dans cette version |
| `captured_unix_ms` | Heure UTC de fin de capture, millisecondes depuis l'époque Unix ; pas un numéro de tick |
| `train_count`, `track_count`, `station_count`, `signal_count` | Tailles des quatre collections principales |
| `game_sha256[65]` | Empreinte hexadécimale du binaire identifié à l'ouverture |

Le snapshot reste consultable après la fermeture de sa session ou du jeu. Sa
durée de vie ne garantit pas sa fraîcheur : à l'écran, il peut déjà être ancien.

## Lire les collections : protocole `Copy*`

Toutes les fonctions ci-dessous reçoivent `(snapshot, records, capacity, required)`.
`CopyTrainPathTracks` prend en plus `train_id` juste après `snapshot`.

1. Appeler avec `records=nullptr`, `capacity=0` et un pointeur `required` valide.
2. Si le statut est `NIMBY_OK`, allouer `required` éléments du bon type.
3. Copier avec cette capacité. Un snapshot étant immuable, sa taille ne change pas.
4. Conserver le contenu copié dans votre application ou libérer vos buffers.

`capacity` et `required` comptent des **éléments**, jamais des octets. Un buffer
trop petit reçoit `NIMBY_BUFFER_TOO_SMALL`, la taille nécessaire et aucune copie
partielle. `required` est obligatoire ; un buffer nul avec une capacité non nulle
est invalide. Avec zéro élément, le second appel peut être omis.

```cpp
uint32_t count = 0;
auto status = NimbySdk_CopyTrains(snapshot, nullptr, 0, &count);
if (status == NIMBY_OK) {
    std::vector<NimbyTrain> trains(count); // nécessite <vector>
    if (count != 0) {
        status = NimbySdk_CopyTrains(snapshot, trains.data(), count, &count);
    }
    if (status == NIMBY_OK) {
        // Utiliser trains ici, en vérifiant les flags de chaque train.
    }
}
```

### Fonctions disponibles

| Fonction | Type des éléments | Contenu / disponibilité |
|---|---|---|
| `NimbySdk_CopyTrains` | `NimbyTrain` | Trains et validité individuelle de vitesse/position |
| `NimbySdk_CopyTracks` | `NimbyTrack` | Voies, limites de vitesse et références de gare |
| `NimbySdk_CopyStations` | `NimbyStation` | Gares ; nom personnalisé ou automatique sélectionné par le jeu |
| `NimbySdk_CopySignals` | `NimbySignal` | Implantation et type des signaux |
| `NimbySdk_CopySignalStates` | `NimbySignalState` | Une ligne par signal ; vérifier les bits de validité |
| `NimbySdk_CopySignalTextures` | `NimbySignalTexture` | Une ligne par signal ; référence et fichier valides indépendamment |
| `NimbySdk_CopyTrackNodes` | `NimbyTrackNode` | Graphe principal partiel et coordonnées natives |
| `NimbySdk_CopyTrackReservations` | `NimbyTrackUsage` | Intervalles de réservation natifs ; `DATA_UNAVAILABLE` si composant inconnu |
| `NimbySdk_CopyTrackOccupations` | `NimbyTrackUsage` | Intervalles d'occupation natifs ; disponibilité indépendante des réservations |
| `NimbySdk_CopyTrainPathTracks` | `uint64_t` | IDs de voies du Path stocké pour un train ; `DATA_UNAVAILABLE` si absent ou instable |

`NIMBY_OK` avec zéro entrée signifie une collection observée vide.
`NIMBY_DATA_UNAVAILABLE` avec zéro entrée signifie inconnue. Pour les états et
textures, `NIMBY_OK` peut accompagner des lignes dont `flags=0`.

## Trains, voies et gares

### `NimbyTrain`

| Champ | Lecture correcte |
|---|---|
| `id` | ID complet du train |
| `name_utf8[257]` | Nom UTF-8, jusqu'à 256 octets utiles |
| `flags` | Tester les bits ci-dessous indépendamment |
| `speed_mps` | Vitesse d'affichage en m/s si `NIMBY_TRAIN_SPEED_VALID` (ou `NIMBY_TRAIN_PRESENT` pour SDK 0.6.0) ; multiplier par 3,6 pour les km/h |
| `track_id` | Voie référencée si `NIMBY_TRAIN_POSITION_VALID` |
| `track_fraction` | Position normalisée dans `[0,1]` si position valide |
| `direction` | `+1` ou `-1` selon l'orientation de la voie, si position valide |

L'absence de `NIMBY_TRAIN_PRESENT` ne permet pas de conclure à une vitesse nulle.
Depuis 0.6.1, `NIMBY_TRAIN_SPEED_VALID` valide la vitesse indépendamment de ce
bit historique, qui indique toujours un Drive actif. Si
`NIMBY_TRAIN_SPEED_DEFAULTED` est aussi présent, la valeur est le zéro utilisé
par l'affichage natif pour un Motion sans Drive : ce n'est pas une mesure.
Un train sans Motion correspondant conserve une vitesse indisponible.
Les anciens clients restent compatibles (ABI 1, structure inchangée), mais
doivent utiliser le nouveau bit pour afficher ces zéros par défaut.
L'absence du bit de position ne permet pas de tracer le train à la position zéro.

### `NimbyTrack` et `NimbyStation`

Une voie contient `id`, `station_id` et `speed_limit_mps`. Sa limite de vitesse
est une propriété de la voie, pas la consigne actuelle d'un train.
Une gare contient `id` et `name_utf8[257]`. Depuis 0.6.2, les noms automatiques
proviennent du cache natif, joint par l'ID complet (génération comprise).
Un nom personnalisé reste prioritaire lorsque le mode automatique est désactivé.
Un cache absent, instable ou invalide laisse le nom indisponible sans faire
échouer la capture du réseau. Un nom vide peut aussi être volontairement vide.
Une référence `station_id=0` indique l'absence de référence de gare.

Liaison : `train.track_id → track.id`, puis `track.station_id → station.id`.
Elle ne donne ni l'horaire, ni la destination, ni le prochain arrêt du train.

## Signaux et textures

`NimbySignal` contient `id`, `track_id`, `track_fraction`, `direction` et `kind`.
Sa position est normalisée sur la voie. Son type est une catégorie d'objet :

| Constante | Valeur |
|---|---:|
| `NIMBY_SIGNAL_ONE_WAY` | 0 |
| `NIMBY_SIGNAL_PLATFORM_STOP` | 1 |
| `NIMBY_SIGNAL_BALISE` | 3 |
| `NIMBY_SIGNAL_PATH` | 4 |
| `NIMBY_SIGNAL_NO_WAY` | 5 |
| `NIMBY_SIGNAL_MARKER` | 6 |

Ce type n'est pas un état rouge/vert. Traiter aussi les valeurs inconnues.
Relier états et textures par `signal_id → signal.id`, dans la même capture.

### `NimbySignalState`

| Champ | Bit requis |
|---|---|
| `aspect` | `NIMBY_SIGNAL_ASPECT_VALID` |
| `system_utf8[32]`, `specific_state_utf8[64]` | `NIMBY_SIGNAL_SPECIFIC_STATE_VALID` |
| `texture_state` | `NIMBY_SIGNAL_TEXTURE_STATE_VALID` |

Aspects définis : `NIMBY_SIGNAL_ASPECT_UNKNOWN` (0), `STOP` (1), `CAUTION` (2),
`PROCEED` (3), `DARK` (4), tous préfixés `NIMBY_SIGNAL_ASPECT_`.
**L'adaptateur actuel laisse l'aspect général inconnu.** Les identifiants
spécifiques observés sont du type `nimby:<hash>:kind.<type>.state.<valeur>`.
Les noms français comme `fr:carre` sont des possibilités du contrat, pas des
états que cette version calcule.

`texture_state=0` peut être valide ; seul son bit de validité permet de le savoir.
Ce sélecteur brut est distinct de l'index finalement sélectionné dans le catalogue.

### `NimbySignalTexture`

| Champs | Signification |
|---|---|
| `signal_id` | Signal concerné |
| `textures_hash`, `file_hash` | Identifiants natifs, pas des empreintes SHA-256 de validation |
| `selected_index`, `state_count` | Index sélectionné et nombre d'entrées du jeu de textures |
| `textures_id_utf8[128]` | Identifiant du jeu de textures |
| `mod_id_utf8[256]` | Identifiant du mod |
| `relative_path_utf8[512]` | Chemin de la ressource dans le mod |
| `file_path_utf8[1024]` | Chemin local résolu si `FILE_VALID` |
| `source` | 0 : intégré au jeu ; 1 : mod local ; 2 : Steam Workshop |

Bits de `flags`, avec le préfixe `NIMBY_SIGNAL_TEXTURE_` :

- `REFERENCE_VALID` : entrée du catalogue résolue.
- `FILE_VALID` : fichier local résolu à la capture ; gérer quand même un échec de lecture d'image.
- `DEFAULT_SET` : jeu de textures par défaut utilisé.
- `CLAMPED` : sélecteur limité aux index disponibles.

Sans référence valide, ne pas interpréter un index zéro comme une texture connue.
Le SDK n'atteste pas le succès du chargement GPU du jeu. Les détails et preuves
sont dans [États des signaux](signal-states.md).

## Graphe, Paths, réservations et occupation

### `NimbyTrackNode`

`id` identifie une voie ayant une géométrie ; `link_a` et `link_b` donnent des
liaisons principales ; `x` et `y` sont des coordonnées projetées natives.
Ce ne sont pas des degrés de latitude/longitude. Les liaisons peuvent être
non réciproques et les jonctions sont incomplètes. Zéro ne prouve pas un butoir.
Ces données ne donnent pas la position d'une aiguille.

### Path d'un train

```cpp
uint32_t count = 0;
auto status = NimbySdk_CopyTrainPathTracks(snapshot, trainId, nullptr, 0, &count);
// Si OK : allouer count uint64_t puis rappeler avec le même trainId et snapshot.
// Si DATA_UNAVAILABLE : afficher « Path inconnu », même si count vaut zéro.
```

L'ordre est celui du vecteur natif stocké. Le sens, la signification des
embranchements et le caractère complet ou restant du trajet ne sont pas établis.
Une voie du Path n'est pas nécessairement réservée ou occupée.

### `NimbyTrackUsage`

`train_id` et `track_id` désignent le train et la voie. `fraction_begin` et
`fraction_end` sont des bornes finies dans `[0,1]`, avec début ≤ fin.
Plusieurs intervalles peuvent se recouvrir, y compris pour un même train.

Lire séparément `CopyTrackReservations` et `CopyTrackOccupations`, puis filtrer
par `train_id`. Ces collections ne sont pas des itinéraires ordonnés et
n'incluent pas les réservations virtuelles définies par les scripts.
Ne pas en déduire une permission de passage ou un état de signal.

À chaque capture, remplacer la collection affichée. Si elle est indisponible,
afficher « inconnu » et retirer les anciennes portions de l'affichage courant.

## Codes de retour

| Code | Constante | Action / contexte |
|---:|---|---|
| 0 | `NIMBY_OK` | Opération réussie ; vérifier encore la validité de chaque champ |
| 1 | `NIMBY_INVALID_ARGUMENT` | Vérifier ABI, `struct_size`, pointeurs, capacités et flags |
| 2 | `NIMBY_IO_ERROR` | Vérifier PID, droits d'accès et fichier/processus disponible |
| 3 | `NIMBY_INVALID_BINARY` | Le fichier ne satisfait pas les contrôles du binaire Windows AMD64 |
| 4 | `NIMBY_ALREADY_INITIALIZED` | Diagnostics déjà initialisés ; vérifier leur propriétaire |
| 5 | `NIMBY_HOOKS_UNAVAILABLE` | Demande de hooks refusée ; cette version ne les active pas |
| 6 | `NIMBY_INTERNAL_ERROR` | Échec interne, notamment allocation ; ne pas utiliser de résultat invalide |
| 7 | `NIMBY_UNSUPPORTED_GAME` | Exécutable non reconnu ; attendre un profil compatible |
| 8 | `NIMBY_DATA_UNAVAILABLE` | Données absentes/instables ; distinguer inconnu et vide, réessayer plus tard |
| 9 | `NIMBY_INVALID_HANDLE` | Handle nul, fermé ou du mauvais type ; corriger sa durée de vie |
| 10 | `NIMBY_BUFFER_TOO_SMALL` | Allouer le nombre d'éléments demandé et recopier |
| 11 | `NIMBY_PROCESS_EXITED` | Fermer la session ; retrouver le nouveau PID et rouvrir |
| 12 | `NIMBY_RESOURCE_LIMIT` | Libérer les sessions/snapshots accumulés |

## Diagnostics de `sdk.h`

Ces fonctions sont distinctes de l'observation d'un processus externe.

| Fonction | Usage |
|---|---|
| `NimbySdk_InspectBinary(path, out)` | Inspecte un fichier désigné par un chemin UTF-16 ; `out` est un `NimbyBinaryInfo` initialisé |
| `NimbySdk_Initialize(abi_version, flags)` | Initialise les diagnostics du processus hôte ; passer `NIMBY_ABI_VERSION`, `flags=0` |
| `NimbySdk_GetHostInfo(out)` | Copie l'identité de l'hôte des diagnostics ; exige leur initialisation |
| `NimbySdk_Shutdown()` | Arrête les diagnostics ; idempotent ; n'invalide pas à lui seul les handles d'observation |

`NimbyBinaryInfo` contient `struct_size`, `recognized_research_build`,
`file_size` et `sha256[65]`. Inspecter un autre fichier ne change pas l'hôte.
Un exécutable reconnu pour la recherche ne constitue pas une cible de hook validée.
`NIMBY_REQUEST_HOOKS` reste refusé.

Tous les appels se font hors `DllMain` et hors callback TLS. Avant de décharger
la DLL : arrêter et joindre les threads appelants, libérer les snapshots, fermer
les sessions, puis arrêter les diagnostics si vous les aviez initialisés.

## Donn?es d?taill?es et plan de ligne (0.6.4)

`NimbySdk_CopyTrainDetails(snapshot, out, capacity, required)` renvoie un
`NimbyTrainDetails` par train. `NIMBY_TRAIN_PASSENGERS_VALID` valide le nombre de
voyageurs ; `NIMBY_TRAIN_ASSIGNMENT_VALID` valide les IDs opaques schedule/shift
et l'index d'ordre ? partir de z?ro. Ces IDs ne sont pas des noms de service.

`NimbySdk_CopyTrainLineStops(snapshot, train_id, out, capacity, required)` copie
le plan complet de la ligne active : IDs ligne/voie/gare et index ? partir de
z?ro. `NIMBY_LINE_STOP_TIMES_VALID` valide les offsets d'arriv?e/d?part en
secondes dans le plan de ligne. Leur diff?rence est la dur?e d'arr?t pr?vue,
pas le temps restant r?el. Courses partielles et boucles peuvent modifier les
arr?ts effectivement desservis et leurs horaires.

Les deux nouvelles structures font 40 octets (pack 8). Les structures ant?rieures
restent inchang?es. Les conventions de comptage, buffer trop petit et snapshot
immuable sont identiques aux autres exports. Un plan absent renvoie
`NIMBY_DATA_UNAVAILABLE`, pas un tableau d'arr?ts invent?s.

## Quais (0.6.5)

`NimbySdk_CopyPlatforms` expose `NimbyPlatform` (288 octets) : voie, gare, nom et validite. Voir [occupations et reservations par gare](platform-occupations.md).

Depuis 0.6.6, `NimbyTrainDetails.passenger_count` utilise la table des voyageurs embarqu?s affich?e dans la fiche du jeu. Les versions 0.6.4/0.6.5 lisaient ? tort la capacit?. Une lecture impossible reste indisponible via le bit de validit?.
