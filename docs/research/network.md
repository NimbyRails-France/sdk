# Réseau : voies, gares, signaux et balises — 14 septembre 2026

Suite : la version 0.2 expose désormais ces lectures par une API publique
expérimentale de snapshots dans la DLL. Voir [le guide développeur](../developing.md).
Le rapport ci-dessous conserve les preuves de l'étape de recherche initiale.

## Résultat concret

Le moniteur externe affiche maintenant, pour chaque train : vitesse, ID de voie,
position relative, sens, limite de la voie, gare associée et signaux sur cette voie.
L'adaptateur réutilisable est `src/engine/network.cpp`, avec ses déclarations dans
`include/engine/network.h`. Il reste **expérimental et en lecture seule**, hors API
publique et hors DLL chargée dans le jeu. Aucun nouveau hook ni appel de fonction
du jeu n'a été installé. Aucun essai d'écriture de vitesse pendant cette étape.

Partie observée : **379 enregistrements de voie, 9 gares, 27 signaux, dont 2 balises**.
Ces nombres incluent les objets à ID actif de la base ; leur statut de construction
(projet, construit, etc.) n'est pas encore filtré.

Le moniteur montre les signaux du **même segment** : il ne prétend pas donner le
prochain signal sur l'itinéraire. Les quais et les états rouge/vert ne sont pas
encore décodés. `Hors gare` signifie seulement `Track.station_id == 0`.

## Binaire et méthode

- Version affichée à l'écran de démarrage : `1.19.10.5bfaea3`.
- SHA-256 : `FFF49AC21720ABFC824C2B4F68B862727630EB0DB71CFE1F9EA8F685D0DB10AE`.
- PE AMD64, base préférée Ghidra `0x140000000`, taille fichier 12 583 424 octets.
- 18 914 fonctions identifiées automatiquement dans l'analyse initiale.
- Projet Ghidra ouvert avec `-readOnly -noanalysis` : aucun changement persistant.
- Les petits ponts de réflexion non définis comme fonctions ont été désassemblés
  et créés temporairement aux adresses expressément référencées par l'enregistrement.
  Leurs prototypes décompilés ne sont pas des signatures de hook validées.
- Lecture externe par `ReadProcessMemory`, sans suspension ni appel de code cible.

Toutes les adresses de code ci-dessous sont des **RVA**, à ajouter à la base du
module de cette session. Les offsets de données s'appliquent seulement au profil
de cette empreinte. Les adresses de tas des rapports sont des observations, jamais
des valeurs codées en dur.

## Preuves statiques et observations en mémoire

| Élément | Disposition utilisée | Preuves et niveau de validation |
|---|---|---|
| Base du modèle | chaîne décrite dans `stable-resolution.md` | Racine module + `0xB81998`, base de données racine + `0x540` |
| Collection Track | database + `0`, tag ID `1`, pas `0x4E8` | Taille enregistrée à RVA `0x429D14` ; 379 IDs complets vérifiés dans deux processus |
| Station d'une voie | Track + `0xD0`, ID 64 bits | Nom `station_id` à `0x429E1F`, offset à `0x429E3A` ; jointures exactes vers les gares |
| Limites voie | float + `0x80` physique, + `0x84` réglage | Pont `Track_M_max_speed` à `0x4282F0`, référencé à `0x429E6D` |
| Collection Station | database + `0x80`, tag `2`, pas `0x3E8` | Itération dans `0x4A1710`, export `Station` dans `0x4A3450` ; 9 IDs vérifiés |
| Nom manuel de gare | chaîne MSVC à Station + `0x20`, utilisée si octet + `0x40` vaut 0 | Export `0x4A3450` ; noms `Circuit V200 gare 2`, `Circuit V160 gare 1/2` concordent avec l'écran |
| Collection Signal | database + `0x380`, tag `8`, pas `0xC8` | Taille enregistrée à `0x42AA09` ; collection identifiée par lecture bornée de la base et validation de 27 IDs et de toutes leurs voies |
| Type de signal | int32 Signal + `0x30` | Pont `Signal_M_kind`, RVA `0x427F60` ; corroboré par l'interface à `0x3A7830` |
| Position du signal | ID voie + `0x40`, double + `0x48`, int8 sens + `0x50` | Ponts `0x427EF0` / `0x427F20`, renvoyant 24 octets ; IDs et valeurs cohérents dans deux processus |
| Voie du train | Motion + `0x3A8`, ID complet | Observation dynamique : tous les IDs rejoignent un Track actif ; cinq trains, deux processus |
| Position relative du train | double Motion + `0x3B0` | Valeurs dans [0,1], évolution et changement de segment dans les séries ; interprétation normalisée expérimentale |
| Sens du train | int8 Motion + `0x3B8` | Disposition Pos corroborée par les ponts ; +1 observé pour les cinq trains, sens inverse d'un train restant à tester |
| Présence / vitesse train | octet + `0x4B0` / double + `0x3C8` | Lectures UI RVA `0x7F3600` ; recherche précédente et séries réelles |

Le seuil exact du pont de limite n'est pas zéro : float à RVA `0xAAB904`, octets
`DE 38 8E 3E`, soit `0.27777761220932007` m/s. En dessous, le pont sélectionne la
limite physique ; sinon il sélectionne le minimum physique/réglage. L'adaptateur
reproduit cette sélection sur les valeurs finies. Il ne s'agit pas de la consigne
effective du train, qui dépend aussi de sa dynamique, de la ligne et des signaux.

Enum SignalKind enregistré : `OneWay=0`, `PlatformStop=1`, `Balise=3`, `Path=4`,
`NoWay=5`, `Marker=6`. Aucun nom n'est inventé pour la valeur 2. Les deux balises
observées ont les IDs complets `0x8000000120003` et `0x8000000140002`.
Une balise appartient donc ici à la collection Signal ; aucune collection distincte
de balises n'est nécessaire pour ces deux objets.

### Piège de l'enregistrement par réflexion

**Ne pas transposer aveuglément les offsets de réflexion à Motion vivant.**
L'enregistrement nomme `presence` à Motion + `0xB8`, `drive` à + `0x290`, puis
`speed` à Presence + `0x18`. Or le champ de vitesse réellement utilisé par l'UI
et observé est Motion + `0x3C8`, et le Pos observé commence à + `0x3A8`.
La déduction antérieure « Presence commence à +0x3B0 puisque speed est à +0x18 »
est donc **invalide** : +0x3B0 contient la coordonnée relative, pas un ID de voie.
L'origine de cette divergence (représentation, description obsolète ou autre)
reste à établir ; les champs concordants sont corroborés individuellement.

`waiting_signal_id` est enregistré à Drive + `0x1C8`, mais ce fait ne valide
ni son emplacement dans Motion vivant, ni le sens « prochain signal ».
Ce champ n'est pas lu par le nouvel adaptateur.

## Essai réel de redémarrage

Fermeture par **Quit to desktop**, puis lancement normal du même exécutable et
**Continue game**. Aucune modification directe de l'exécutable ni des sauvegardes.
La fermeture a été normale ; la reprise utilise l'état enregistré par le jeu.

| Observation | Avant | Après |
|---|---|---|
| PID | 64116 | 53392 |
| Base module | `0x7FF79AA50000` | `0x7FF79AA50000` |
| Racine | `0x2827FD6A080` | `0x26741AD2140` |
| Base de données | `0x2829DBF35B0` | `0x267D67572C0` |
| Simulation | `0x282A64A2FC0` | `0x2679C8E4D00` |

La base de module n'a pas changé dans cet essai ; la relocalisation du module est
couverte par un test synthétique, pas présentée comme observée ici.
Les **415 enregistrements réseau exportés sont identiques** avant/après : IDs,
limites, associations aux gares, noms manuels, types et positions des signaux.
Les nouvelles séries retrouvent les cinq trains et leurs positions évolutives.
Dans la série après redémarrage, B81500 atteint une vitesse nulle à la position
`0.184356158` de la voie `0x1000000630001`, qui coïncide avec le signal Path
`0x80000001A0001` exporté sur cette voie. Cette corrélation étaye le décodage du
Pos ; elle ne valide pas un algorithme général de prochain signal.
L'ancien moniteur a vidé ses lignes et désactivé son bouton après la fermeture.
Pendant le chargement, la nouvelle détection a refusé une racine absente/changeante,
puis a réussi une fois la partie disponible. Le moniteur doit encore être rouvert
après redémarrage ; ceci valide la résolution, pas une reconnexion automatique.

Preuves sous `reports/network/` :

- `registration.txt`, décompilations `140*.c`, `strings.json` : statique.
- `live-discovery*.txt` : premières lectures brutes et découverte bornée.
- `network-before-restart.txt`, `network-after-restart.txt` : inventaires réels.
- `restart-comparison.json` : comparaison des 415 enregistrements.
- `monitor-before-restart.txt`, `monitor-after-restart.txt` : séries temporelles.
- `monitor-ui-after-restart.txt` : arbre d'accessibilité de l'affichage réel.

## Gardes, tests et suite

`read_network` refuse les versions non reconnues avant toute lecture, revalide la
racine avant/après, borne les tailles et pointeurs, contrôle tags/indices/IDs
complets, les références intercollections, les valeurs finies et les descripteurs
de blocs. En cas d'échec, le résultat est vide ; aucune ancienne donnée n'est
présentée comme actuelle. Les noms automatiques de gare restent affichés par ID.

**8/8 tests passent en Debug et Release.** Le nouveau test réseau emploie une
mémoire entièrement simulée et couvre notamment ID de génération différent,
descripteur remplacé, lecture impossible, racine changée, mauvais indice, NaN,
position absente et sens invalide. Il ne prouve pas la disposition réelle.

Les instantanés externes restent optimistes : un champ peut changer sans changer
le descripteur. Aucun verrou ou thread de simulation n'a été validé. Le même ID
doit toujours être résolu à nouveau ; ne conserver aucun pointeur d'objet côté mod.

Prochaines vérifications nécessaires :

1. Retrouver les chemins de code natifs qui lisent Motion +0x3A8 et établissent
   la topologie des voies ; vérifier un train de sens -1 et une inversion.
2. Corréler la progression relative avec les distances géométriques et l'UI ;
   distinguer les portions construites, les projets et les quais.
3. Résoudre l'itinéraire effectif et l'état des signaux pour calculer le prochain
   signal traversé, avec gestion des aiguillages et du sens.
4. Identifier le thread/protocole de publication des snapshots avant d'exposer
   une API publique ou toute commande d'écriture du réseau.

Reproduire la lecture intégrée, après chargement d'une partie :

```powershell
build/Release/NimbyTrainMonitor.exe <PID> --sample
```

Le probe brut `tools/network_probe.cpp` est uniquement un outil de recherche,
compilé avec les sources du moniteur via inclusion et les adaptateurs engine.
Il ouvre seulement `PROCESS_QUERY_INFORMATION | PROCESS_VM_READ` et vérifie le SHA.
