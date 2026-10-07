# Construction expérimentale depuis Kotlin

Cette API de développement Windows sert à qualifier la pose de séries dans une
**partie solo d’essai**. Les kits reconstruits localement depuis le mode développeur
du Hub incluent le pont (`NIMBY_DEVELOPMENT_CONSTRUCTION=ON`). Les builds
alpha Windows à partir de `0.8.0-alpha.2` l’incluent également pour Signal
Placement. Les distributions stables gardent cette option désactivée ; la
construction reste une fonctionnalité expérimentale réservée au jeu solo.
Le binaire reconnu est NIMBY Rails `1.19.10.5bfaea3`, SHA-256
`fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae`.

## Contrat

`NimbyClient.prepareConstruction(sourceSignal)` charge explicitement le pont et
produit un jeton. L’éditeur des voies doit être actif dans le jeu. Lire ensuite
une nouvelle capture pour calculer les positions ; une capture antérieure à la
préparation ne convient pas. La simple lecture du SDK ne charge aucun pont.
Depuis un mod isolé, `ToolContext.prepareConstruction` demande au SDK du jeu
de charger le pont. Le mod utilise les fonctions Kotlin sans injection ni
accès aux fonctions natives du jeu.

`ToolSignal.travelDirection` donne le sens des trains sur la voie : `+1` de A
vers B, `-1` de B vers A. Pour copier un signal, utiliser
`source.placementAt(trackId, fraction, travelDirection)` ; le SDK traduit
l'orientation propre au modèle. Ces positions servent à la fois à
`showSignalPreview` et `createSignals`. Le mod n'a pas à interpréter le type
numérique du signal ni à inverser lui-même sa direction.

`createSignals(token, sourceSignal, positions)` accepte 1 à 64 positions. Le mod
choisit la route, les distances, le sens et les exclusions. Le SDK valide les
identités, les fractions strictement intérieures, les directions ±1 et les
doublons exacts. Il copie le modèle via l’affectation profonde native, puis utilise
l’allocateur et l’exécuteur du jeu. Il ne choisit ni espacement ni règle ferroviaire.

Une seule commande enveloppe la série. Son delta est finalisé par le répartiteur
natif après toutes les créations. La réponse attend l’accusé de réception de
l’éditeur : une entrée de file ou un appel d’exécuteur ne constitue pas un succès.

`undoConstruction(token)` utilise le delta de **cette** opération. La correspondance
entre retour, déplacement du delta et entrée d’historique est suivie explicitement.
L’annulation est refusée si une commande intermédiaire ou une autre entrée
d’historique empêche d’attribuer la dernière opération à cette série. Préparer une
nouvelle série valide remplace le jeton courant et abandonne son accès à l’annulation SDK.
Le jeu conserve son propre historique.

Une préparation `READY` d’un autre propriétaire bloque tant que son bail est
valide dans la même partie et la même révision. Une commande déjà en exécution
reste protégée même si son bail de préparation expire. Un résultat terminé ne
réserve pas indéfiniment l’éditeur ; la source d’une nouvelle préparation est
validée avant de remplacer l’ancienne opération.

Les appels d’une session sont sérialisés. Un mutex nommé sérialise les demandes
destinées au même jeu ; un mod isolé reçoit immédiatement une erreur de ressource
occupée si un autre demandeur le détient. Une requête non commencée expire après
une seconde et ne peut démarrer après la mort de son demandeur. Le jeton préparé
reste propre à ce demandeur. Si l'exécution a déjà commencé, consulter le résultat
avec `pollConstruction` ; ne pas relancer la création. Le travail natif se fait
sur les threads habituels du jeu :
enfilage dans l’éditeur, exécution sous le verrou du simulateur, historique dans
l’éditeur. Aucun pointeur du jeu ne traverse le contrat Kotlin.

## Résultats et erreurs

| État | Signification |
|---|---|
| READY | Jeton préparé ; capturer puis calculer |
| APPLIED | Toutes les créations ont été accusées ; IDs disponibles |
| PARTIAL | Création ou copie des réglages incomplète ; examiner les IDs, la raison et `canUndo` |
| UNDONE | Annulation exécutée et accusée |
| REJECTED | Demande refusée ; raison numérique disponible |
| PENDING | Résultat incertain ; appeler `pollConstruction(token)` |

Raisons : 1 requête invalide/expirée, 2 jeton/session/révision invalide,
3 retour de création inattendu, 4 éditeur occupé ou contexte indisponible,
5 modèle/voie absent ou état incompatible, 6 historique non annulable,
7 copie des réglages NRF indisponible, 8 un autre outil vivant détient une
préparation encore valide.

Les répétitions copient les valeurs effectives des cases et réglages numériques
NRF du signal source, y compris les valeurs par défaut et les cases décochées.
Elles sont figées avant
la première création et appliquées quand les nouveaux IDs sont observés dans
la même partie. Une capture plus ancienne ne doit pas effacer cette copie.
La présence d’un ancien pont UI incompatible fait refuser la construction avant
mutation. Un échec de copie après création produit `PARTIAL`, même si tous les
signaux natifs existent ; ne pas répéter automatiquement la pose.

Un délai ou une exception de transport après une demande de création ne prouve
pas l’absence de mutation. Ne pas refaire automatiquement la commande. Le pont
conserve une réponse finale bornée. Après la fin de l’exécution, une fenêtre de
lecture de deux secondes protège cette réponse contre son remplacement par un
autre propriétaire. Une lecture valide du bon ticket et du bon propriétaire
acquitte le résultat copié ; une lecture `PENDING`, invalide ou étrangère ne
l’acquitte pas. La mort du propriétaire ou l’expiration de cette fenêtre permet
une nouvelle préparation. Un ticket devenu périmé ne prouve donc jamais que
l’ancienne commande n’a rien modifié.

Une ressource temporairement occupée autorise à réessayer une lecture, un
aperçu, son effacement ou un panneau lors d’un callback ultérieur. Elle
n’autorise pas à rejouer automatiquement `createSignals` ou `undoConstruction`.
Un refus pendant la préparation demande une nouvelle action explicite après
revalidation de l’aperçu. La première préparation peut payer le coût de
chargement du pont ; les préparations suivantes réutilisent le pont chargé.

Le pont reste épinglé jusqu’à la
fermeture du jeu : redémarrer avant de tester une nouvelle DLL. Il refuse de
cohabiter avec les sondes de recherche de construction.

Les appels/résultats Kotlin sont consignés dans les journaux communs
`logs/sdk-client` et le compagnon dans `logs/mods/signal-placement`.

## Limites de qualification

- Multijoueur non pris en charge : l’expansion locale de la commande n’est pas un
  protocole de réplication. Le pont ne sait pas encore prouver le mode solo ;
  cela bloque sa distribution générale. Le kit local de développement reste
  réservé à la qualification solo, sans certification de compatibilité réseau.
- Les observations ne garantissent pas encore l’exhaustivité des aiguilles.
  Le calcul arrête les branches connues mais ne prouve pas l’absence d’une
  branche omise. Ne pas utiliser cette version sur une partie de production.
- Le compteur surveille le répartiteur natif. Il ne couvre pas des écritures
  mémoire arbitraires provenant d’autres mods. Le contrôle des pointeurs de
  session n’est pas un identifiant persistant de sauvegarde et n’exclut pas ABA.
- Une création partielle reste possible ; ne pas annoncer une transaction
  atomique avec retour arrière garanti.
- La qualification de sauvegarde/relecture, courbes, aiguilles dans les deux
  sens et modèles SFR doit précéder la livraison générale.

Voir [les preuves de recherche](research/signal-construction.md). Les adresses
de cette intégration restent dans `src/platform/windows/runtime/construction_bridge.cpp`.

## Recette du 3 octobre 2026

Build local `4512896058239782643`, jeu reconnu ci-dessus, carte de 25 997 signaux :
un premier refus de budget CPU après le chargement du pont a laissé les signaux
et leurs réglages inchangés. L’aperçu a repris sans commande de pose automatique.
Un second clic explicite a ajouté exactement un signal avec les onze valeurs
effectives de sa source, dont travaux activés et deux cantons. L’annulation avec
le même ticket a rétabli exactement les 25 997 lignes et leurs paramètres.

Le fichier de sauvegarde de référence est resté inchangé et les réglages de
l’essai ont été restaurés. Cette recette couvre ce scénario ; elle ne remplace
pas la qualification restante des géométries, de la sauvegarde et du multijoueur.
