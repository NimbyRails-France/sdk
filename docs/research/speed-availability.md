# Disponibilité de la vitesse — 2026-09-16

## Mise à jour 0.6.1 : règle native confirmée

L'assembleur du binaire reconnu confirme le comportement de l'affichage des
informations du train : à RVA `0x7F5BE3`, `xorps xmm6,xmm6` initialise zéro ;
`cmp byte [r12+0x4B0],0` puis `je` contournent la lecture de vitesse si Drive
est absent. Sinon `movsd xmm6,[r12+0x3C8]` lit la vitesse. La décompilation
`reports/trains/1407f3600.c:1292` relie cette valeur à `train_info_speed`.

Le SDK 0.6.1 reproduit cette **convention d'affichage**, sans prétendre mesurer
une vitesse dans un bloc absent. Un Motion sans Drive fournit zéro avec
`SPEED_VALID | SPEED_DEFAULTED`, après relecture de l'ID complet et de l'absence
de Drive. Un Motion manquant reste inconnu. `PRESENT` garde son ancien sens.
Les exemples annotent les zéros par défaut ; `isSpeedDefaulted()` les distingue
en C++. Les octets résiduels de Drive ne sont jamais utilisés dans ce cas.

Validation : tests synthétiques (Drive actif, zéro, NaN, absence de Drive,
absence de Motion, changement de Drive entre lectures), helpers avec anciens
et nouveaux flags, et capture réelle de 924 trains. Les 13 tests CTest passent.

## Diagnostic initial (avant cette validation)

Diagnostic en lecture seule sur le jeu ouvert (PID 31988), binaire reconnu
par `identify`. Aucun changement de la logique de vitesse.

## Cause dans le SDK

`Train::getSpeedKmh()` renvoie une valeur seulement si `NIMBY_TRAIN_PRESENT`
est positionné. `NimbySdk_CaptureSnapshot` produit ce bit depuis `Train::present`.
Dans `read_trains`, celui-ci correspond à l'optionnel **Drive** de Motion
(`+0x4B0`), et non à toutes les formes de présence d'un train sur le réseau.
La vitesse utilisée par le SDK est le double à `Motion+0x3C8`, lu seulement
quand Drive est présent. Voir aussi `train-paths.md` et `network.md`.

Un autre optionnel, **Presence**, peut fournir une position (`+0x1D0`,
position à `+0xB8`) sans Drive. Le SDK expose alors la position, mais aucune
vitesse. Le nom `NIMBY_TRAIN_PRESENT` ne doit donc pas être interprété comme
un indicateur général « train physiquement présent ».

## Éléments observés

Au débogueur, dans `examples/first-observer/main.cpp:79` :

- `T z55558`, ID `1407374883749894` : flags=0, track_id=0.
- `UM 23510`, ID `1407374883684357` : flags=3, speed_mps=0 : un zéro valide.
- `TER X 73502`, ID `1407374884012035` : flags=3,
  speed_mps=11.117799308945257. Une exécution précédente le montrait sans
  vitesse mais avec une position : la disponibilité peut changer.

Une lecture native supplémentaire a compté 924 trains : 364 avec Drive,
25 avec Presence seule, 535 sans les deux ; tous avaient un Motion.
Ces comptes décrivent cette lecture, pas une répartition permanente ou atomique.
Les ID et les deux indicateurs ont été relus pour chaque échantillon.

Exemple Presence seule : `z55515`, ID `1407374888337410`, Drive=0,
Presence=1, position valide. Les octets du champ de vitesse de Drive
contenaient encore 0.373369 m/s alors que ce bloc était absent : les lire
sans vérifier l'optionnel risquerait d'exposer une ancienne valeur.

## Limites et suite

Sans inspection de l'état métier correspondant dans le jeu, ce diagnostic ne
permet pas d'affirmer « au dépôt », « hors service » ou « à quai » pour chaque
train. Une valeur indisponible ne doit pas être remplacée arbitrairement par 0.
Pour couvrir Presence seule, il faut valider une source de vitesse propre à
cet état (ou une règle native prouvant l'arrêt), puis représenter sa validité
indépendamment de Drive. La position seule ne suffit pas à cette preuve.

Le test asynchrone à 2 secondes a expiré sous le débogueur ; l'exemple synchrone
a permis de capturer les valeurs. Les points d'arrêt ajoutés ont été retirés
et les sessions de diagnostic arrêtées.
