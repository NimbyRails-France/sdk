# Politique de conduite fournie par les mods — 23 septembre 2026

## Frontière retenue

Le SDK fournit des mécanismes de consignes numériques et leur exécution native.
Les vitesses de circulation et l'association entre aspects et consignes sont
définies par le mod. La valeur implicite de passage a été retirée de l'API Kotlin,
ainsi que les plafonds numériques arbitraires dans la validation des consignes.
Les limites physiques natives restent applicables.

`AutomaticDriving.kt` documente les fonctions de construction des consignes.
`ApproachPassable` est une permission explicite, distincte de `Clear` : le moteur
ne déduit plus une réouverture du seul fait qu'une instruction vise un panneau
en aval. Le mod SFR choisit ses vitesses dans `DrivingLimits.kt` et ses politiques
dans `DrivingInstructions.kt`.

## Vérifications effectuées

- Le test de réouverture en nouvelle annonce échouait avant la correction.
- La consigne d'approche reste mémorisée jusqu'au passage mesuré de la tête.
- Une annonce aval dépourvue de permission n'autorise pas ce passage.
- Une autre vitesse fournie par un mod est respectée : aucune valeur nationale
  n'est choisie par le moteur.
- Des données périmées, absentes ou une nouvelle fermeture restent restrictives.
- Le passage consomme l'ancienne annonce et mémorise la nouvelle cible sans
  libérer une restriction indépendante attendant un Clear.
- CTest Release : 51 tests réussis.
- CTest Debug : 51 tests réussis. Une exécution simultanée avec Release a fait
  échouer `loader_restart`, dont le moniteur a observé les processus de l'autre
  suite. Le test a ensuite réussi seul. Exécuter les deux suites successivement.
- Kit Kotlin reconstruit ; construction du mod SFR : 18 tâches réussies, tests
  Kotlin et vérification des exports, démarrage, arrêt, redémarrage et rechargement.

Les journaux locaux se trouvent dans `build/windows-validation-20260923/`, sous
les noms `policy-*`. Ce dossier est un artefact local, non une dépendance du SDK.

## Limites de cette validation

Ces résultats ne valident pas encore le nouveau comportement dans une partie.
Le processus de jeu ouvert pendant cette passe conserve les anciens binaires.
Il faut installer ensemble le SDK et le mod reconstruits, puis reproduire la
séquence avec mesures de vitesse au franchissement. Aucun correctif à chaud
n'a été appliqué à ce processus.

Le point du signal est utilisé pour le franchissement de la tête ; aucune
position d'antenne ni transmission KVB complète n'est simulée. La géométrie,
les hypothèses de visibilité du moteur et les permissions natives restent des
contraintes de l'implémentation, pas une certification d'un règlement ferroviaire.
La validation Linux reste distincte et n'a pas été exécutée ici.
