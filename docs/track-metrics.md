# Longueurs natives des voies — développement 0.8.x

Le client Kotlin expose `Observation.trackMetrics`, une liste optionnelle de
`TrackMetric(trackId, lengthM)`. `offsetM(fraction)`, `fraction(offsetM)` et
`distanceM(fromFraction, toFraction)` convertissent les positions normalisées
suivant la métrique du parcours natif. Les entrées non finies ou hors voie sont
refusées. La longueur n'est pas reconstruite à partir des coordonnées des nœuds.

Cette capacité est **non publiée**. Une DLL 0.8.x antérieure sans le nouvel
export retourne `null` côté Kotlin, tout en conservant les autres observations.
Une ligne absente indique une longueur indisponible, jamais une longueur nulle.
Les captures de signalisation limitées ne calculent pas ces métriques.

## Preuve et portée

Le pont Windows existant `src/platform/windows/runtime/automatic_driving_bridge.cpp`
lit `Track + 0x88` comme un double et vérifie, dans `permissionRange` et `scan`,
`nativeRange.length == abs(to-from) * metric` à une tolérance de 0,01 m.
Cette extension réutilise cette interprétation. Elle ne prétend pas avoir
effectué une nouvelle campagne de mesure dans le jeu.

L'offset est déclaré uniquement dans `include/platform/windows/game_layout.h`.
Le profil Linux reste non qualifié pour cette nouvelle lecture. Le moteur commun
lit deux fois le préfixe du record comprenant identité, liens et métrique ;
tout changement observé, erreur de lecture ou valeur non positive/non finie
supprime la métrique de cette voie. Le contrôle d'empreinte du jeu et les gardes
existantes de pool/racine restent applicables. La capture n'est pas atomique.

Le nouvel export privé `NimbyInternal_CopyTrackMetrics` utilise un record de
16 octets, sans modifier les structures ABI existantes. Aucun client C++ public
n'est ajouté et aucune commande d'écriture n'est autorisée par cette lecture.

## Limites pour la construction

Une longueur disponible ne garantit pas la complétude des raccordements, le
statut construit/projet, une révision stable du réseau ou le droit de placer un
signal. Un aperçu calculé depuis une capture est informatif. Le futur système
de commande devra vérifier ces propriétés dans le jeu avant toute pose.

Les fixtures couvrent les métriques valides, les données invalides/instables,
les tailles ABI et la compatibilité avec une bibliothèque sans nouvel export.
