# Cantons et liens de signaux : API haut niveau du SDK

## Construction depuis la topologie observée

`<nimby/block_topology.hpp>` fournit `BlockTopology(boundaries, nodes, junctions)`.
L'appelant fournit les signaux qui délimitent les cantons, sans y mélanger les
balises ou marqueurs. `read(signalId)` cherche le premier signal rencontré dans
le sens de circulation et restitue les portions exactes en fractions de voie.
Les raccords inversés changent le sens de parcours ; les signaux opposés ne
ferment pas le canton. Aucune distance métrique n'est inventée.

`SignalBlock.hasBoundary()` distingue une limite trouvée d'une trace partielle.
Une bifurcation de face reste indéterminée, de même qu'un lien absent, une boucle
sans signal aval, un budget de parcours atteint ou des limites superposées.
`SignalBlock.occupation(reader)` renvoie `Unknown` pour une trace incomplète,
même si le lecteur fourni connaît une couverture complète. Ne pas utiliser ses
portions partielles pour conclure que tout le canton est libre.

Cette API décrit la géométrie ; elle ne prouve ni la réservation d'un itinéraire,
ni la couverture complète des observations d'occupation. Le test `block_topology`
couvre les limites, les directions, les bifurcations, les boucles et ces refus.

Le SDK décrit l'occupation (`Unknown`, `Clear`, `Occupied`), compare les emprises
des trains aux limites des cantons et parcourt les liens des signaux. Il ne
connaît aucune indication française : les annonces, couleurs et textures restent
dans le mod. Les limites et l'itinéraire choisi doivent être configurés ; les
cantons ne sont pas inférés depuis une distance à vol d'oiseau.

## Lecture dans un mod

```cpp
#include <nimby/mod.hpp>

auto blocks = nimby::readBlocks(); // Une capture SDK, pas une capture par canton.
auto state = blocks.read(blockSections);
auto otherState = blocks.read(otherBlockSections);
```

`blockSections` est une liste de `nimby::BlockSection{trackId, begin, end}`,
fractions normalisées d'une même voie. Le lecteur possède ses observations,
indexées par voie ; on le réutilise pour tous les cantons de ce cycle de lecture.
Une nouvelle lecture crée un nouveau lecteur : aucun ancien état n'est réinjecté
en cas d'échec. La connexion est gérée comme celle de `nimby::readTrain` et libérée
par l'adaptateur à l'arrêt. La fonction lance une capture réseau complète : elle
n'est pas une boucle temps réel, contrairement à la lecture ciblée d'un train.

Si une capture existe déjà, utiliser `nimby::observeBlocks(*snapshot)` depuis
`<nimby/block_observation.hpp>` ; aucune deuxième lecture du jeu n'est effectuée.
Le délai de fraîcheur par défaut est 1000 ms, réglable via son second argument.

**Couverture expérimentale de la lecture réelle :** `observeBlockCoverage`
recoupe la collection native complète (table principale et chaînes secondaires)
avec la présence de chaque train. Une présence inconnue, un train présent sans
emprise, ou des emprises appartenant à un train absent/inconnu invalident cette
couverture. `checkBlockCoverage` ne transforme jamais un échantillon d'emprises
en collection complète : cette garantie doit venir de l'extracteur natif.
Une emprise fraîche correspondante donne `Occupied`. Sans correspondance,
`Clear` exige cette couverture vérifiée ; sinon le résultat reste `Unknown`.
L'absence de table ou des données périmées donnent également `Unknown`.
Ce recoupement ne prouve pas l'atomicité avec un tick du moteur : les lectures
pendant les mouvements et changements de session restent à valider.

## Observations fournies, scénarios et tests

```cpp
nimby::BlockReader blocks(footprints, coverageVerified, fresh);
auto state = blocks.read(blockSections);
```

Les `TrainFootprint{trainId, trackId, begin, end}` sont des emprises occupées,
jamais des réservations. `coverageVerified=true` exige la preuve que toutes les
portions et tous les trains sont couverts. Ce drapeau est approprié aux scénarios
complets ; il ne doit pas être activé arbitrairement pour un snapshot du jeu.

Le SDK tient compte de tous les trains et de toutes les portions. Un train
chevauchant deux cantons occupe les deux. Une queue touchant encore une limite
reste occupante. Les intervalles invalides, cantons vides et observations périmées
restent inconnus. Une observation complète sans recouvrement donne `Clear`.

## Parcours générique des signaux

Pour un mod, utiliser l'entrée haut niveau :

```cpp
auto decisions = nimby::evaluateSignals(signals, decideSignal, unknownDecision);
```

Chaque élément de `signals` porte `id` et `nextSignal`, plus les données métier
du mod. `decideSignal(signal, optionalNextDecision)` retourne une décision pour
ce seul signal, ou `nullopt` lorsqu'il faut connaître l'aval. Le SDK construit
les liens, valide les IDs, choisit l'ordre de calcul et associe chaque résultat
à son ID. Le mod ne manipule ni indices ni callbacks de parcours.

Une boucle sans référence ou un lien manquant utilise `unknownDecision`.
Une règle peut décider localement sans attendre l'aval (par exemple une fermeture).
Le quatrième argument optionnel borne le nombre de signaux, dans la limite SDK
de 4096. `SignalNetwork` ci-dessous demeure l'implémentation générique du SDK.

`nimby::SignalNetwork` dans `<nimby/signal_network.hpp>` reçoit les
`SignalLink{id, next}` de l'itinéraire sélectionné. `resolve<Result>` reçoit
trois fonctions métier : décision locale ou frontière connue, décision à partir
du résultat aval, décision en cas de lien absent/boucle sans référence.

Le SDK valide les IDs, détecte les liens manquants et cycles, puis traite l'aval
avant l'amont sans récursion. Les résultats conservent l'ordre d'entrée. Un
résultat local peut couper la dépendance à l'aval (par exemple un signal fermé).
Les nœuds sont limités à 4096. Le SDK ne choisit ni branche ni couleur ni vitesse.

Tests : `blocks_and_signal_network` couvre emprises, queues, plusieurs trains,
fraîcheur, couverture, copie des données, ordre d'entrée, liens absents, cycles
et IDs dupliqués. Les scénarios d'indications nationales restent dans leurs mods.
