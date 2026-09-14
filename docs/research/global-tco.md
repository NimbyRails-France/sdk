# Carte complète et actualisation — SDK 0.4, 2026-09-14

## Constat et limites du diagnostic

Les premières captures du PID 69952 restaient identiques dans le client autonome
et dans les deux pools root+5C0 / root+680. L'horloge du jeu a ensuite progressé
à l'écran ; les échantillons suivants ont confirmé que les mêmes pools changent.
Ne pas conclure qu'un nouveau pointeur racine était nécessaire. La cause exacte
de l'intervalle initial figé n'est pas établie (pause/transition possibles).
Le choix de racine n'a pas été changé.

L'ancien panneau avait une limite certaine : il ne montrait que 800 points dans
un rayon local, recalculait l'orientation et l'échelle autour d'un train à chaque
capture, et associait les marqueurs au point de voie sans interpoler leur fraction.
Il ne pouvait pas satisfaire une navigation sur toute la carte.

## Corrections

Le panneau utilise désormais un QQuickPaintedItem C++ avec toutes les coordonnées
et tous les liens fournis par le SDK. Il n'y a plus de coupe par secteur, de rotation
automatique ni de recentrage à chaque capture. La caméra conserve centre/échelle,
avec zoom au curseur, déplacement libre, Tout voir et Centrer train.
Les voies utilisent un fond raster mis en cache, recalculé sur changement de
géométrie/caméra/taille ; les trains et le Path sont redessinés depuis chaque capture.
Pas d'extrapolation ni de faux mouvement pendant une pause. Les fractions sont
affichées, mais la position cartographique exacte le long des courbes reste à valider.

La construction de centaines de milliers de QVariantMap à chaque tick est évitée :
les points passent en bloc binaire typé local à l'app Qt (NimbyTrackNode). Ce bloc
provient exclusivement de l'API publique. Le worker reste séparé de l'interface,
avec au plus un résultat en attente et cadence adaptée au coût de capture.

Les liens primaires +08/+10 sont conservés lorsque leur cible complète existe,
même sans lien réciproque. L'ancien filtre éliminait les références de branche
unidirectionnelles. On ne fusionne aucun point par proximité. Les points dont le
graphe a plus de deux voisins sont repérés à fort zoom, sans inventer une position
de lame d'aiguille. Les raccordements spéciaux non décodés restent incomplets.

## Positions hors Drive

La sérialisation Motion (RVA 4B1E10) contient aussi un optionnel Presence à +B8,
gardé par +1D0 ; son sérialiseur 49B620 commence par Pos, via 2F2EE0.
Pos contient ID à +0, fraction double à +8, deux octets à +10/+11.
La lecture existante exploite l'octet +10 selon le contrat courant ; l'autre
orientation n'est pas exportée comme une nouvelle signification supposée.

`timing_probe.cpp` a relevé par exemple un train sans Drive :
ID 5000000190004, voie 10000E8CF0001, fraction 0.997135, octets 1/1.
Le SDK utilise désormais Presence comme repli de position lorsque Drive n'en
fournit pas. Il ne déduit pas une vitesse nulle de l'absence de Drive.
Les trains sans état de position valide restent non localisés, avec libellé explicite.

## Validation

Binaire reconnu SHA-256 FFF49AC21720ABFC824C2B4F68B862727630EB0DB71CFE1F9EA8F685D0DB10AE.
Tests SDK : 10/10 ; fixture supplémentaire Presence sans Drive et fraction NaN.
Le client SDK autonome réussit sur 924 trains, 248418 voies, 3342 gares, 26003 signaux.

`NimbyTco.exe --verify-live` teste huit secondes de réception réelle sans afficher
sa fenêtre ; code 0 exige au moins deux captures. Il compte les positions distinctes
du premier train mais n'exige pas qu'un jeu potentiellement en pause se déplace.
Essai normal (hors debugger) : 10 captures, 10 positions distinctes, 389 trains
positionnés ; coût de construction de 385 à 477 ms. Le test sous GDB a dépassé
la fenêtre de huit secondes sans capture et n'est pas utilisé comme preuve de réussite.
La vue globale de la France a été vérifiée visuellement dans le TCO.

Les scripts de paquet/client fixent désormais NimbyRailsSDK_DIR explicitement :
un cache CMake d'un ancien SDK ne doit pas détourner CMAKE_PREFIX_PATH. Le client
observer demande 0.4, cohérent avec la compatibilité SameMinorVersion du paquet.

Aucun fichier du jeu ou de sauvegarde modifié. Cette étape ne valide pas encore
les aspects des signaux, l'occupation des cantons ou les réservations.
