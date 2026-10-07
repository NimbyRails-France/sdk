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

## Cible franchie et validation de mouvement manquée — 3 octobre 2026

Un test reproduit un arrêt mémorisé à la position 1000 : après une réouverture
autorisant le mouvement de 999 à 1001, une validation finale manquée laissait
cette cible derrière le train avec un plafond nul. La capture suivante du parcours
pouvait déjà avoir retiré le signal franchi.

Le moteur compare maintenant deux observations valides de la tête pour retirer
une cible dont le franchissement est démontré. Il recharge d'abord la position du
même signal dans le parcours courant. Une première observation seule, une tête
immobile au signal ou une cible encore devant ne suffit pas. Cette récupération
ne reconstitue aucune nouvelle instruction qui aurait été reçue pendant le
mouvement manqué ; les autres arrêts et limites mémorisés restent applicables.

Le même test échoue avec le moteur précédent et passe avec la correction. Les
tests purs `automatic_driving` et `automatic_controller` passent également.
Cela établit le défaut et sa correction dans le moteur, sans attribuer à lui seul
les ralentissements des trains Z55520 et Z55521 observés dans la partie.

## Contention affectant les trains hors mod — 4 octobre 2026

Une reproduction de l'ancien adaptateur maintient son verrou global depuis un
autre thread. Le train testé ne possède ni état de conduite SDK ni signal géré.
Sans contention, sa permission native vaut 1 et ses paramètres d'intégration
restent inchangés. Avec contention, la vérification retourne 0 sans appeler la
permission native. L'intégration reçoit un plafond, une distance et une cible
nuls, au lieu de 55,5 m/s, 1234 m et 7,25 m/s. Les deux assertions de progression
échouent avec l'ancien code. Ce défaut pouvait donc affecter un train sans aucun
signal de mod sur son parcours.

L'adaptateur sépare désormais le remplacement des publications de l'état propre
à chaque train. Un signal non géré consulte directement la permission native.
Un train sans intérêt SDK conserve son intégration native, indépendamment des
verrous des autres trains ou des producteurs. L'option explicite
`maximumLineSpeed` conserve sa portée existante et est comptée séparément dans
les diagnostics. Une contention sur le même train géré reste restrictive.

Le diagnostic V5 compte les appels, les contentions par opération, les états
indisponibles et les validations de mouvement. Ses échantillons incluent les
paramètres natifs et choisis ainsi qu'un horodatage monotone. Les enregistrements
sont bornés et ajoutés sans réutilisation : après saturation, les compteurs
continuent mais les échantillons ne décrivent plus nécessairement le présent.
Ces reproductions locales ne suffisent pas à attribuer chaque ralentissement
de la sauvegarde au défaut ; la validation sur la carte reste distincte.

Les tests natifs `automatic_driving_permission_bridge` et
`automatic_driving_state_isolation` passent avec le pont reconstruit. Ils
vérifient un producteur bloqué, deux trains indépendants, la conservation d'une
nouvelle annonce réellement franchie, le retrait puis retour du même
propriétaire, le changement de monde et le passage d'un parcours non géré à un
parcours nouvellement géré. Le test du registre couvre aussi sa saturation ;
quatre lecteurs concurrents vérifient la cohérence de 4000 publications pendant
leur remplacement et leur retrait. Ces callbacks natifs sont simulés dans un
processus de test, sans modifier une partie.

## Isolation des publications — 3 octobre 2026

Les consignes de signaux et de trains sont maintenant publiées par propriétaire.
Deux lots portant sur des IDs distincts coexistent ; une collision est refusée
sans modifier les lots précédents. Un arrêt ne retire que les consignes et les
instructions mémorisées de son propriétaire. Les baux restent indépendants :
le renouvellement d'un mod ne renouvelle pas les données périmées d'un autre.
Un propriétaire expiré conserve ses IDs jusqu'à sa libération explicite.

Le transport natif V3 ajoute le jeton de propriétaire. Les anciens appels SDK
V1/V2 sont associés au module appelant. Le transport de processus isolé remplace
tout jeton du mod par l'identité du canal attribuée par le SDK. Un ancien pont
résident sans V3 est refusé ; il faut redémarrer le jeu avec le SDK reconstruit.

Le plafond de vitesse maximale reste global lorsqu'un seul propriétaire publie.
Avec plusieurs propriétaires, les options sont prises sur les signaux du
parcours du train. Les plafonds physiques et contraintes natives restent actifs.
Les tables sont bornées à 64 propriétaires, 32768 signaux et 8192 trains.

La régression de remplacement d'un lot A par un lot B a été reproduite dans le
vrai adaptateur natif avec callbacks simulés avant correction. Les tests couvrent
maintenant coexistence, collision, libération étrangère, expiration indépendante,
libération pendant un mouvement natif et nettoyage des seules instructions du
propriétaire. Les tests RPC couvrent les tailles incohérentes, les comptes hors
limites et la substitution du propriétaire. Ces essais simulés ne mesurent pas
la latence de rendu de la partie.
