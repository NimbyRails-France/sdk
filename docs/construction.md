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
Depuis un mod, `ToolContext.prepareConstruction` suit le même protocole : chargement
local sur le worker du mod, jamais injection distante dans son propre processus.

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
nouvelle série remplace le jeton courant et abandonne son accès à l’annulation SDK.
Le jeu conserve son propre historique.

Les appels d’une session sont sérialisés. Un mutex nommé sérialise les clients du
même processus. Le travail natif se fait sur les threads habituels du jeu :
enfilage dans l’éditeur, exécution sous le verrou du simulateur, historique dans
l’éditeur. Aucun pointeur du jeu ne traverse le contrat Kotlin.

## Résultats et erreurs

| État | Signification |
|---|---|
| READY | Jeton préparé ; capturer puis calculer |
| APPLIED | Toutes les créations ont été accusées ; IDs disponibles |
| PARTIAL | Une partie seulement a été créée ; examiner les IDs et `canUndo` |
| UNDONE | Annulation exécutée et accusée |
| REJECTED | Demande refusée ; raison numérique disponible |
| PENDING | Résultat incertain ; appeler `pollConstruction(token)` |

Raisons : 1 requête invalide/expirée, 2 jeton/session/révision invalide,
3 retour de création inattendu, 4 éditeur occupé ou contexte indisponible,
5 modèle/voie absent ou état incompatible, 6 historique non annulable.

Un délai ou une exception de transport après une demande de création ne prouve
pas l’absence de mutation. Ne pas refaire automatiquement la commande. Le pont
conserve son dernier résultat pour la consultation. Il reste épinglé jusqu’à la
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
