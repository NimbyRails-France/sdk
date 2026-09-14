# Réservations et occupation — SDK 0.5.0

Recherche du 14 septembre 2026, observation externe en lecture seule. Binaire Windows
x64 1.19.10.5bfaea3, 12 583 424 octets, SHA-256 :
`FFF49AC21720ABFC824C2B4F68B862727630EB0DB71CFE1F9EA8F685D0DB10AE`.
Toutes les adresses de fonctions ci-dessous sont des **RVA relatives au module**.
Les exports Ghidra conservent leurs VA avec base d'analyse `0x140000000`.

## Faits vérifiés par les consommateurs natifs

| RVA | Preuve / rôle observé |
| --- | --- |
| `0x452F40` | Enregistrement Extrapolator : reservation_probe, get_one_occupation, get_one_reservation |
| `0x452480`, `0x452250` | Itérateur qui distingue explicitement occupation et réservation (`is_occ`) |
| `0x4526B0`, `0x452860` | Recherche du train occupant / réservant une position |
| `0x47A5F0` | Accès à la collection globale de réservations à `Sim + 0xD98` |
| `0x45F860` | Recherche dans la table répartie en 16 partitions, avec mutex natifs |
| `0x6EB6D0`, `0x459EA0` | Inspecteur et itérateur d'occupation : collection à `Sim + 0x2C8` |
| `0x6EC380`, `0x63C9E0` | Affichage distinct « réservé par » / « occupé par » |

Les [décompilations et captures](reports/reservations/) permettent de remonter chaque
champ à son consommateur. Ce sont des fonctions identifiées, **pas des cibles de hooks**.
Les prototypes Ghidra restent supposés. Aucun appel natif n'est effectué ; la convention
Windows x64, les prototypes complets, le thread d'appel et la durée de vie d'arguments
devraient être validés avant tout futur appel ou hook.

## Layout adapté à ce binaire seulement

La racine est résolue à chaque capture depuis module + `0xB81998`, puis simulation à
racine + `0x680` (voir la recherche sur le changement de sauvegarde).

Réservations : 16 partitions à `Sim + 0xD98`, stride `0x88`. Dans chacune : contrôles
`+0x50`, slots `+0x58`, taille `+0x60`, masque de capacité `+0x68`. Un contrôle inférieur
à 128 indique un slot occupé. Les slots font 32 octets : ID voie puis les trois pointeurs
d'un vecteur. Une entrée du vecteur contient ID train `+0`, champ auxiliaire `+8` non
exposé, bornes doubles `+0x10/+0x18`. L'API normalise ces bornes dans `[0,1]`, sans
prétendre restituer le sens ou l'ordre d'un itinéraire.

Occupation : en-tête à `Sim + 0x2C8`, vecteur de clés aux offsets `0/8/0x10`, vecteur
de valeurs à `0x18/0x20/0x28`. Clé nulle = vide. Valeur de 32 octets : ID train `+0`,
bornes doubles `+8/+0x10`, auxiliaire non exposé `+0x18`. Débordement : table de buckets
`+0x38`, nombre de buckets uint32 `+0x40`. Nœud : ID voie `+0`, vecteur `+8/+0x10/+0x18`,
suivant `+0x20`. Plusieurs enregistrements peuvent couvrir la même voie pour un même
train (occupation par voitures) : aucun dédoublonnage sémantique n'est supposé.

## Validation et garde-fous

`src/engine/track_usage.cpp` vérifie les limites d'allocation, pointeurs, masques,
types d'ID, nombres finis, bornes, cycles et stabilité des buffers relus. Trois essais
maximum par composant. Les racines sont revérifiées ; chaque ID train et voie doit
appartenir au snapshot public. Une incohérence rend **ce composant indisponible**.
Les fonctions de copie travaillent ensuite uniquement sur le snapshot possédé par le SDK.

Sur la sauvegarde ouverte, `reports/reservations/sdk-live.txt` conserve 20 captures SDK
avec réservations disponibles, environ 13 200 portions pour environ 330 trains et
1 200 portions d'occupation. Le comparateur observe 607 ajouts et 445 retraits
d'intervalles, y compris les variations de bornes : ce ne sont pas 607 itinéraires
complets nouvellement établis. Capture complète mesurée autour de 304–363 ms.
`tools/usage_observer.cpp` permet de reproduire cette lecture avec le PID du jeu.

Les fixtures de `tests/track_usage.cpp` sont **synthétiques**, indépendantes du jeu :
vecteurs vides, bornes inversées, données instables, pointeurs invalides, NaN, cycles,
changement de racine et refus d'un binaire non reconnu. Le test public contrôle aussi
buffers, IDs, bornes et immutabilité avec un PID fourni explicitement.

## Limites et prochaines vérifications

- Lecture externe non atomique : aucun mutex du jeu n'est acquis. Les relectures
  détectent certaines mutations, sans garantir une vue d'un unique tick de simulation.
- Aucune écriture, aucun nouveau hook, aucune modification de sauvegarde ou du jeu.
- Les réservations virtuelles de scripts via `event_signal_marker_reserved` ne sont
  pas interrogées. La collection native n'est donc pas une synthèse de tous les scripts.
- Réservation, occupation, Path calculé, aspect d'un signal et position d'une aiguille
  sont des données différentes. Une absence de données ne prouve jamais une voie libre.
- Valider encore pause/reprise, nouvelles sauvegardes et transitions de simulation,
  puis l'interprétation des courbes géométriques avant de dessiner les bornes exactes.
  Les essais de changement de racine actuels sont synthétiques ; ils ne remplacent pas
  une nouvelle campagne de changements de sauvegarde dans le jeu pour cette API.

Pistes écartées : le vecteur inspecteur à `Sim + 0x2200 -> +0x410` était vide, sans
signifier absence de réservations globales. Le `+0x880` d'un SimSnapshotBase trouvé
dans l'Extrapolator ne doit pas être appliqué directement à Sim. Les premiers rapports
de sonde conservent ces hypothèses, contredites par les consommateurs natifs suivants.

Références primaires : [API NimbyScript](https://wiki.nimbyrails.com/NimbyScript),
[architecture de réservation, mai 2023](https://carloscarrasco.com/nimby-rails-may-2023/),
[itérateur de réservations, septembre 2025](https://carloscarrasco.com/nimby-rails-september-2025/).
