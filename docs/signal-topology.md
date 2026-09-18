# Ordre des signaux et connexions observées

API C++ ajoutée après la release 0.7.1. La prise en charge des aiguilles nécessite
la DLL et les en-têtes de développement correspondants (nouveau symbole interne
`NimbyInternal_CopyTrackJunctions`). La DLL publiée en 0.7.1 ne suffit pas.
`Snapshot::getSignalsForTrack(id)` renvoie les signaux par fraction native
croissante, avec l'identifiant comme départage à position égale. Les balises
sont conservées ; l'ordre des identifiants n'est pas un ordre spatial.

Construire une fois l'index du snapshot pour travailler sur plusieurs voies :

```cpp
auto topology = snapshot->getSignalTopology();
auto forward = topology.getSignalsForTrack(trackId, +1);
auto reverse = topology.getSignalsForTrack(trackId, -1);
auto facing = topology.getSignalsForTrack(trackId, -1, true);
auto axis = topology.getTrackAxis(trackId); // optional, axe A -> B normalisé
auto trace = topology.traceFrom(nimby::Position{trackId, 0.4, +1}, 256);
for (const auto& section : trace.sections) {
    // section.trackId, section.direction, section.orderedSignals dans l'ordre rencontré
}
// Toujours examiner trace.stop et trace.stoppedAtTrack.
```

L'index possède ses données mais n'est pas actualisé automatiquement. Le recréer
sur le nouveau snapshot. Le sens demandé vaut +1 ou -1. `facingOnly` filtre le
sens natif ; cela ne détermine pas si le signal s'applique au train ni s'il
autorise le passage. Par défaut les deux sens sont présents.

Le parcours inclut la position de départ et ignore les signaux derrière celle-ci.
Il suit B en sens +1, A en sens -1, puis adapte le sens selon l'extrémité de la
voie suivante qui référence la précédente. Les jonctions B-B et A-A peuvent
donc inverser le sens natif sans inverser l'ordre rencontré.

Les raisons d'arrêt sont : `UnobservedConnection`, `UnknownTrack`,
`InconsistentConnection`, `AmbiguousConnection`, `Cycle`, `TrackLimit`, `Junction`.
Une boucle est détectée sur la position orientée et l'étape du raccordement. Le parcours est borné à
`maxTracks` voies (256 par défaut). À la limite, `stoppedAtTrack` désigne la
prochaine voie non visitée ; sinon la voie où le parcours s'est arrêté.

## Aiguilles et prochains signaux

`Snapshot::getAllTrackJunctions()` expose les raccordements natifs validés :
voie principale, fraction exacte, branche et sens d'entrée. Le lien inverse est
vérifié dans les données du moteur ; aucune connexion n'est créée par proximité.
`traceFrom` s'arrête à une aiguille abordée en pointe (`Junction`) et retourne
les positions orientées possibles dans `continuations`. Les sections sont
limitées à `fromFraction` / `toFraction`, sans inclure les signaux après le choix.
Une branche abordée vers son raccordement rejoint la voie principale à sa
fraction native, dans le sens de sortie prévu par le moteur.

```cpp
auto signal = snapshot->getSignalById(signalId);
if (signal) {
    auto next = topology.findNextSignals(
        nimby::Position{signal->getTrackId(), signal->getFraction(), direction},
        4096, false, signal->getId()); // Exclure le signal de départ.
    for (const auto& candidate : next.nextSignals) {
        auto state = snapshot->getSignalStateById(candidate.getId());
        // État de ce candidat dans la même capture.
    }
    // next.truncated : budget atteint ; next.incomplete : connexion manquante.
}
```

Cette recherche s'arrête au premier signal de chaque chemin observé (tous ceux
à la même fraction sont conservés). Elle explore les branches et déduplique les
résultats par ID ; l'ordre entre branches n'est pas un ordre spatial. Le sens
de recherche est explicite, distinct du sens graphique propre à un signal Path.
`facingOnly` garde son filtre sur le sens natif brut. Le parcours ne fait pas de
demi-tour à l'aiguille et ne sélectionne pas l'itinéraire réservé d'un train.

## Limites

Les raccordements natifs avec une extrémité primaire ouverte sont pris en charge
quand leur référence inverse, leur génération d'ID, leur fraction et leurs sens
sont valides. Les raccordements spéciaux non décodés et les données instables
restent incomplets. Un lien absent ne prouve ni une fin de ligne ni une voie libre.
Cette API expose les possibilités topologiques, pas les prochains signaux garantis
de l'itinéraire d'un train.

L'axe est calculé à partir des nœuds voisins : c'est une approximation pour
l'affichage, pas la tangente exacte d'une courbe ni une distance le long du rail.
Le TCO utilise l'ordre et l'axe du SDK pour espacer les symboles, avec un trait
vers leur voie.

## Essai réel sur une aiguille — 16 septembre 2026

Trois signaux intégrés « Path signal » ont été posés dans le jeu sur la partie
de test Professional Freight : un en amont de la bifurcation et un sur chaque
branche. La capture SDK est passée de 26 à 29 signaux ; les trois nouveaux
objets ont le type natif 4.

| Emplacement | ID du signal | ID de voie | Fraction | Sens natif |
| --- | --- | --- | --- | --- |
| Entrée | `8000000140003` | `1000000810001` | 0.3868132947 | -1 |
| Branche directe | `8000000190002` | `1000000850001` | 0.4567953770 | -1 |
| Branche déviée | `80000001c0001` | `1000000d00001` | 0.8135875228 | +1 |

Les identifiants du tableau sont hexadécimaux. Le parcours depuis l'entrée
en sens -1 retrouve le signal de la branche directe, mais pas celui de la
branche déviée. Depuis cette dernière, le parcours en sens -1 s'arrête après
six sections sur `1000000d50001`, avec `UnobservedConnection` : son lien A
exporté vaut zéro. Le sens +1 s'arrête également sur une connexion non
observée, à l'autre extrémité (`1000000ca0001`).

Ce constat décrit le défaut avant correction. Après correction, 20 raccordements
sont lus dans cette partie. Depuis le signal d'entrée en sens -1,
`findNextSignals` retrouve exactement `8000000190002` et `80000001c0001`,
en 12 sections, sans troncature ni connexion manquante. Depuis la branche déviée
en sens -1, il retrouve `8000000140003` en 8 sections. Les détails du décodage
et la preuve au débogueur sont dans [le rapport de recherche](research/track-junctions.md).
