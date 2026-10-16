# Fins de voie dirigées — 9 octobre 2026

Qualification statique en lecture seule du profil Windows119, SHA-256
`fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae`.
Base de l’image : `0x140000000`. La qualification des racines Linux ne qualifie
pas ces champs de Track. Aucun jeu n’a été lancé pour cette recherche.

Les exports Ghidra headless sont conservés localement dans
`.validation/buffer-stops-20261009/exports/`, avec le script
`InspectBufferStops.java` et les journaux du mode `-readOnly -noanalysis`.
Le projet analysé est `build/sdk-data-ghidra/NimbyRailsResearch`, programme
`NimbyRails.exe`. Les adresses ci-dessous sont des RVA, sauf indication VA.

## Producteur exact du diagnostic natif

Le code UI `0x7a0a40` affiche le warning 4 via
`signal_warn_ignored_plat_desc` : « One way signals at platform dead ends are
ignored ». L’éditeur `0x79f4d0` obtient ce résultat du producteur `0x762660`
(appel à VA `0x14079fbad`) et transmet le tableau de résultats à l’UI.

Le producteur résout l’ID complet de la voie via `0x32c420`, puis exécute
exactement ce prédicat :

```cpp
signal.kind == 0 && track.station_id != 0 &&
    (track.link_a == 0 || track.link_b == 0)
// => warning 4
```

Les offsets et branches sont confirmés dans son désassemblage :

```text
1407627a3 MOV EAX,dword ptr [RDI + 0x30]  ; Signal.kind
1407627a6 TEST EAX,EAX
1407627a8 JNZ 0x1407627cc
1407627aa CMP qword ptr [RBX + 0xd0],0x0 ; Track.station_id
1407627b2 JZ 0x1407627cc
1407627b4 CMP qword ptr [RBX + 0x8],0x0  ; Track.link_a
1407627b9 JZ 0x1407627c2
1407627bb CMP qword ptr [RBX + 0x10],0x0 ; Track.link_b
1407627c0 JNZ 0x1407627cc
1407627c2 MOV EAX,0x4
```

Ce warning est une règle UI grossière : il ne vérifie ni le sens du signal,
ni le parent d’une branche, ni les enfants. Il ne suffit donc pas à identifier
une fin de parcours dirigée. Aucun objet ou drapeau distinct de butoir
graphique n’a été qualifié dans cette recherche.

## Connectivité native à prendre en compte

Le parcours de sections `0x379d10`, confirmé par `0x37cf30`, utilise trois
formes de continuation ordinaires. Les offsets de raccordement sont également
documentés dans [track-junctions.md](track-junctions.md).

| Donnée | Offset Track | Rôle |
| --- | --- | --- |
| Identité complète | `+0x00` | Incarnation de la voie |
| Primaire A / B | `+0x08 / +0x10` | Continuation en sens -1 / +1 |
| Station | `+0xd0` | Association de station, indépendante de la terminaison |
| Parent | `+0x3f0` | Voie principale d’une branche |
| Fraction sur le parent | `+0x3f8` | Position du raccordement |
| Sens d’approche sur le parent | `+0x400` | ±1 pour entrer dans la branche |
| Enfants begin/end/cap | `+0x408 / +0x410 / +0x418` | Liste inverse d’IDs complets |

`0x379d10` parcourt les enfants situés devant la position courante, avec le
sens d’approche correspondant. Il émet ensuite le primaire B en sens +1 ou A
en sens -1, s’il existe. Quand ce primaire est nul et qu’un parent existe, il
émet le parent à `+0x3f8` avec le sens opposé à `+0x400`. Sa variante de
retournement, contrôlée par le premier octet de `param_1+1`, ajoute également
le sens positif du parent ; elle est volontairement exclue du décodage.
Les conflits géométriques à `+0xf8` sont une option distincte du parcours et
ne sont pas des liens ordinaires de continuation.

Pour l’extrémité A, la position testée est fraction 0, sens -1. Pour B, c’est
fraction 1, sens +1. Une fin `Terminal` exige : primaire de ce côté réellement
nul, parent réellement nul et liste inverse complète/stable ne contenant
aucune branche valide exactement à cette extrémité dans ce sens.

Un parent non nul sur le côté ouvert d’une branche valide donne `Connected`,
jamais `Terminal`. Une branche enfant à l’extrémité avec le bon sens donne
également `Connected`. Une branche à une fraction intérieure ne prolonge pas
l’extrémité elle-même : elle reste une alternative de parcours en amont.
La sélection de l’itinéraire d’un train demeure une autre observation.

## Capacité préparée dans le SDK

La capture optionnelle `NimbyInternal_CaptureTrackEndpointsSnapshot` reçoit au
maximum 4096 IDs complets et partage un budget de 131072 unités de références
(résolutions et entrées des listes). Aucun balayage global, appel du jeu ou
écriture mémoire n’est ajouté aux captures existantes. La structure séparée
`NimbyTrackEndpointStates` préserve la taille de `NimbyTrackNode` et des autres
records ABI.

Chaque extrémité vaut `Unknown`, `Connected` ou `Terminal`. `Connected`
qualifie au moins une continuation ; il ne garantit pas que toutes les
alternatives soient connues. `Unknown` distingue lecture indisponible,
descripteur/attache invalide, référence introuvable, instabilité et budget.
Le champ station est une association observée, pas la preuve d’un quai avec
butoir. L’adaptateur interne expose `captureTrackEndpoints(ids)`, puis
`getTrack(id)->at(direction)` avec un type enum, sans offsets natifs.

Le lecteur utilise un descripteur et une table de pool bornés pour les seuls
IDs demandés/référencés. Il relit pour chaque record l’identité/liens
`[0,0x30)`, l’attache/enfants `[0x3f0,0x420)` et la station `+0xd0`. Les listes
sont alignées, bornées à 4096 entrées, copiées deux fois puis recontrôlées
après leurs dépendances. Les formes vides toutes nulles et vides avec une
allocation conservée sont distinguées des descripteurs invalides.

Chaque enfant doit avoir une identité complète actuelle, ce parent inverse,
une fraction finie dans [0,1], un sens ±1 et exactement un primaire ouvert.
Les listes sans identité, doublons ou références à soi restent inconnues.
Un raccordement au parent exige une occurrence inverse unique. Les records
partagés sont lus/vérifiés une seule fois par capture ; une instabilité locale
invalide les rows qui en dépendent, sans invalider les voies indépendantes.
Une table/pool ou un monde remplacé rejette la capture entière.

Les observations restent non atomiques : ces relectures détectent les
remplacements connus mais ne constituent pas une transaction de simulation.
Aucun aspect AB, ralentissement ou comportement devant un butoir ne consomme
cette capacité à ce stade.

## Validation sans jeu

`tests/track_endpoints.cpp` couvre les deux extrémités, gare terminus et gare
traversante, primaire absent mais parent valide, branche à l’extrémité ou au
milieu, sens opposé, parent inverse incorrect, générations périmées, fractions
NaN/hors bornes, doublons, listes illisibles, données modifiées, remplacement
de pool/monde, qualification de profil et épuisement du budget avant lecture.
Une voie indépendante reste observable lorsque sa voisine est invalide.

`tests/track_endpoint_observation.cpp` vérifie le transport de production,
les buffers sans copie partielle, la différence avec une capture d’un autre
scope, le typage des directions, l’immuabilité, la génération du monde et la
durée de vie indépendante de la session.

Les deux tests et `architecture_boundaries` passent dans
`.validation/signal-interlocking-20261009/endpoint-build` (3/3). Ceci qualifie
le décodage et le transport sur fixtures ; aucune validation nouvelle sur une
partie réelle et aucune détection de texture de butoir ne sont revendiquées.
