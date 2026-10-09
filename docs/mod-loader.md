# Chargement et isolation des mods — contrat interne Windows

Les créateurs de mods utilisent l’API Kotlin `nimby` et le plugin Gradle.
Le [wiki](https://wiki-dev.nimbyrails-france.fr/commencer/installation) décrit ce
parcours public. Cette page documente le loader du SDK, pas une API C++ à
recopier dans les mods.

## Frontières d’exécution

Le proxy SDL démarre le SDK dans le jeu. Le gestionnaire de mods lance ensuite
un processus distinct pour chaque mod natif. Sa DLL et son pont Kotlin sont
chargés dans ce processus, pas dans le thread de rendu du jeu. Les composants
natifs du SDK qui interviennent sur le jeu restent dans le processus du jeu.

Une exception Kotlin, un blocage de callback et un crash natif sont des cas
différents. Les exceptions sont consignées ; la supervision borne le démarrage,
le travail et l’arrêt d’un worker. Un worker bloqué est mis en quarantaine,
sans attendre son callback indéfiniment. Les autres workers restent supervisés
indépendamment. Les Job Objects bornent les ressources des processus ; les
lectures, publications et commandes passent également par des budgets du SDK.

Ces bornes limitent l’impact d’un mod défectueux. Elles ne garantissent ni temps
réel strict ni absence de contention : le CPU, la mémoire et le moteur du jeu
restent partagés. Ce mécanisme n’est pas une sandbox de sécurité pour exécuter
du code malveillant. Une défaillance d’un composant du SDK chargé dans le jeu
peut toujours affecter celui-ci.

## Découverte et cycle de vie

Le Hub prépare `<jeu>/NRFMods/<projet>/` pour les projets déclarant `loaderApi: 1`.
Le manifeste `nrf-mod.ini`, section `[NRFMod]`, fournit `library`, un nom de DLL
sans chemin ni remontée de dossier. Le kit et le plugin génèrent les éléments
nécessaires ; ne recopiez pas une version du SDK dans chaque mod.

Les exports internes `NRFMod_StartV1` et `NRFMod_StopV1` sont appelés dans le
worker. Les initialiseurs de DLL doivent rester minimaux : les observations et
services appartiennent aux callbacks du runtime Kotlin. Le démarrage du mod
peut précéder le chargement d’une partie ; une observation indisponible à ce
moment ne constitue pas une preuve de corruption de la sauvegarde.

Le SDK conserve les diagnostics des workers arrêtés ou mis en quarantaine.
Un mod absent ou invalide est journalisé sans interdire le chargement des autres
mods. Les références de session, les baux et les résultats en cours ne sont pas
des identifiants persistants d’une sauvegarde.

## Réveil des actions d’outils

La boucle interne des outils utilise une capture de session et conserve son
intervalle périodique de 250 ms au repos. Une intention acceptée par le panneau
de signal ou par une fenêtre d’outil peut avancer le prochain cycle. Ce réveil
ne lance aucun callback sur le thread de la fenêtre : la même boucle sérielle
effectue une capture fraîche, puis dépile les intentions avec leurs contrôles
de propriétaire, de génération, de monde et d’expiration habituels.

Le courtier associe chaque fournisseur à l’événement de son propre worker.
Le worker hérite uniquement du droit d’attendre cet événement Windows ; le
pont UI détient son propre duplicata pour le signaler. Une fenêtre hébergée
dans le worker utilise un second événement, local au processus. Aucun handle
fourni dans une requête de mod n’est accepté pour choisir le destinataire.
Le signal ne contient ni commande ni autorisation : la file validée reste
l’autorité, même après un retrait de fournisseur ou un changement de monde.

Les réveils sont regroupés ; deux départs anticipés sont espacés d’au moins
20 ms. Un événement arrivé pendant un callback reste pris en compte après
celui-ci, sans exécution concurrente ni rattrapage des cycles manqués. L’arrêt
est prioritaire et les handles attendus sont fermés après la fin du thread.
L’absence de timer haute résolution conserve une attente d’événements avec
timeout ; si cette attente est indisponible, le cycle périodique reste le
secours. La signalisation BAL et les boucles auxiliaires ne consomment pas
les réveils de cette boucle interne.

Dans une fenêtre d’outil Windows, une publication acceptée, une invalidation
ou un arrêt signale également un événement privé du thread UI. L’affichage
peut ainsi traiter une réponse sans attendre son prochain contrôle de 50 ms.
Ce timeout reste inchangé pour la fraîcheur et la langue, et sert de secours
si l’événement n’a pas pu être créé. Les notifications sont regroupées sans
ajouter de thread ni exécuter de callback Kotlin dans l’UI.

## Installation et validation

Les empreintes du binaire du jeu et des composants natifs sont vérifiées avant
leur utilisation. Les DLL et le kit Kotlin doivent provenir du même build.
L’activation d’un nouveau profil se fait jeu fermé ; il n’y a pas de remplacement
à chaud des composants épinglés dans le processus.

Les tests de qualification couvrent un worker qui bloque, un worker qui inonde
les requêtes et un worker qui crashe pendant que les autres mods continuent.
Leurs résultats sont datés et liés à un build : voir les preuves de validation,
sans en déduire qu’un nouveau binaire ou tous les scénarios ont été qualifiés.
