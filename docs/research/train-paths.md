# Chemins de trains et futur panneau TCO — 2026-09-14

Ce rapport décrit l'étape 0.3. L'évolution 0.4 remplace la vue locale par toute la
carte et inclut les liens primaires non réciproques validés : voir
[carte globale et actualisation](global-tco.md).

## Objectif visuel

La référence fournie montre un panneau noir continu : voies raccordées, diagonales
d'aiguilles, repères de signaux et indications lumineuses. Le TCO actuel est un
observateur par segments indépendants ; il ne représente pas encore cette topologie.
Un panneau doit couvrir une gare ou un secteur lisible, pas 248418 segments simultanés.

Séparer dans les données et dans la légende : chemin calculé du train, itinéraire
réservé/établi, position du train, occupation effective et aspect du signal.
Une liste de voies du chemin ne prouve ni leur réservation ni leur occupation.
La documentation [SNCF OpenTCO](https://numerique.sncf.com/actualites/opentco-une-ia-pour-pour-fluidifier-le-traffic-en-temps-reel/)
décrit la superposition des positions et itinéraires. Le
[TCO Digital du Rail Open Lab](https://www.railopenlab.com/realisations/le-tableau-de-controle-optique-tco-digital)
distingue occupation des voies et position des aiguilles.

## Binaire et preuves statiques

SHA-256 : FFF49AC21720ABFC824C2B4F68B862727630EB0DB71CFE1F9EA8F685D0DB10AE.
Exports Ghidra en lecture seule dans `reports/paths/`. Les prototypes sont inférés,
pas des signatures de hooks validées. Aucune fonction native appelée par le SDK.

| RVA | Preuve |
|---|---|
| 0x4C2A90 | Sérialise des Motion de stride 0x638, appelle 0x4B1E10 |
| 0x4B1E10 | Optionnel Drive dans Motion+0x290, présence à +0x4B0 |
| 0x4B1660 | Drive commence par `nimby::model::Path`, sérialisé par 0x4AFE30 |
| 0x4AFE30 | Path : indicateur +0x90 ; positions +0x60/+0x78 ; vecteur +0xA8 |
| 0x31AAD0 | Ce vecteur contient des entiers 64 bits, stride 8 |
| 0x428790 | Enregistrement nomme les positions `found_start`, `found_goal`, et le booléen `found` |
| 0x3856E0 | TapeTrace contient des éléments de 0x20 avec ID voie, bornes et orientation ; ce n'est pas le vecteur d'IDs du Path |

Les descriptions natives distinguent un TapeTrace simplifié sans branches et un
Path complexe contenant branches, raccordements et rebroussements. Ne pas appliquer
la disposition de TapeTrace à Path. Le vecteur de Path ne doit pas être réduit à
une simple droite sans valider les opérations de parcours.

## Première lecture en jeu

Outil `tools/path_probe.cpp` : processus ouvert en lecture seule, empreinte reconnue,
racines résolues à nouveau, en-tête Path relu après copie, IDs comparés aux voies
actives. Au plus 24 trains présents sont échantillonnés. Aucune injection ni écriture.
Rapport : `reports/paths/live-candidates.txt` ; PID 69952, réseau de 248418 voies.

Exemples : train 0x5000000000005, found=1, 147 entrées et 147 IDs valides ;
train 0x5000000010005, found=1, 260 entrées et 260 IDs valides.
Les adresses absolues du rapport ne sont pas réutilisables. Les données restent
des candidats : la concordance des IDs ne prouve pas leur ordre, le chemin restant,
le choix aux embranchements ou une réservation.

Compilation reproductible avec le MinGW CLion, depuis la racine :

```powershell
& "$env:LOCALAPPDATA/Programs/CLion/bin/mingw/bin/g++.exe" -std=c++20 -O2 -DUNICODE -D_UNICODE -Iinclude tools/path_probe.cpp src/engine/binary_identity.cpp src/engine/live_state.cpp src/engine/network.cpp -lbcrypt -lcomctl32 -lgdi32 -o build/Release/path_probe.exe
./build/Release/path_probe.exe <PID>
```

## Validation requise avant affichage réel

1. Suivre un train identifié : confronter sa voie actuelle, les extrémités du Path
   et la liste à l'itinéraire visible dans le jeu sur plusieurs instants.
2. Vérifier embranchement, terminus et rebroussement ; déterminer si le vecteur
   contient une succession, une empreinte ou des instructions encodées.
3. Décoder les connexions physiques indépendamment des itinéraires ; les listes
   Track+0xE0/+0xF8/+0x110/+0x128 sont des vecteurs Conflict d'après 0x327BB0,
   et ne peuvent pas être proclamées listes de voisins.
4. Définir une API additive indépendante des offsets : train, disponibilité,
   nature du chemin, segments ordonnés avec bornes/orientation seulement si validées.
5. Construire un panneau QML par secteur, avec placement schématique horizontal
   et diagonal, zoom, sélection du train et superposition du chemin observé.
   Garder réservation, occupation et texture courante inconnues tant que non prouvées.

## Intégration expérimentale SDK 0.3

`CopyTrackNodes` expose les coordonnées natives +0x30/+0x38 et uniquement les
liens +0x08/+0x10 réciproques entre IDs complets existants. Les points non finis
sont exclus. `live-links.txt` documente des chaînes réciproques et leurs ruptures.
Un lien absent n'est pas une preuve de cul-de-sac ; les raccordements spéciaux
restent incomplets. Aucune déduction de voisinage par proximité géométrique.

`CopyTrainPathTracks(snapshot, train_id, ...)` copie le vecteur du Path dans son
ordre de stockage. En-tête, ID du Motion et optionnel Drive sont relus ; le vecteur
est lu deux fois. Les entrées doivent toutes correspondre à des voies actives du
snapshot. Ces vérifications bornent les erreurs de lecture sans rendre la capture
atomique. Un changement entre captures reste possible.

Le contrat expose l'appartenance au Path, pas des segments de parcours orientés
ou un itinéraire réservé. Une absence/instabilité retourne DATA_UNAVAILABLE sans
faire disparaître les autres trains. Bornes : 16384 entrées par train, 1048576
entrées totales par capture. Au-delà, les chemins concernés sont indisponibles.
La lecture réelle a fourni 248418 points et 335 Paths sur le PID 69952.

Le panneau QML affiche les liens réciproques d'un secteur autour du train choisi,
limité à 800 points dans un rayon de 1500 unités natives. Les coordonnées sont
tournées et déformées pour un rendu schématique, puis reliées par traits horizontaux
et diagonaux. Les croisements dessinés ne créent aucun lien. Les marques dorées
indiquent seulement les voies du Path. Les symboles train/signal sont associés
au point de leur voie ; leur fraction exacte n'est pas interpolée sans connaissance
validée de la géométrie du segment. La liste conserve les fractions originales.

Tests ajoutés : lecture de Path synthétique, IDs incorrects, vector mal aligné,
échec de lecture, handles publics invalides, copies réelles de graphe et Paths.
Les exports sont additifs : structures ABI v1 précédentes inchangées. Le TCO exige
le paquet 0.3 pour ne pas charger une DLL 0.2 dépourvue des nouveaux exports.
Les fonctions natives étudiées ne sont toujours pas des hooks approuvés.
