# Architecture et maintenance du SDK

Le SDK conserve C++20 pour le noyau natif et Kotlin pour ses consommateurs.
Windows x64 est la plateforme de référence. Le développement et la publication
Linux sont suspendus ; ses dossiers restent séparés pour organiser le code.

Depuis 0.8.0, le client public C++ est retiré. Les outils utilisent `NimbyClient`
en Kotlin/JVM et les mods utilisent les API Kotlin/Native de signalisation ou
d'outils (`SignallingMod`, `toolMod`).
Le pont précompilé possède une `detail::ObservationSession` native privée,
synchrone, qui ne rejoue pas une ancienne capture. `ObservationLoop` reste responsable de la cadence du
mod : il n'y a plus de deuxième worker de rafraîchissement dans une classe cliente.
`detail/observation_values.hpp` conserve les copies et conversions nécessaires
au moteur natif. Ces fichiers ne constituent pas une API pour les consommateurs.

## Règle de découpage

Une fonction va dans le commun dès que son résultat dépend de données et de
contrats communs. Un appel système va dans le dossier de son système. Un format
mémoire propre à un binaire du jeu appartient à son profil de binaire.

La présence d'un appel Windows dans un fichier ne justifie pas de déplacer toute
sa logique dans Windows : extraire cet appel, puis conserver l'algorithme commun.
Ne pas créer une copie Linux d'une classe Windows contenant la même logique.

| Besoin | Emplacement | Ce qui y reste interdit |
| --- | --- | --- |
| Parcours du réseau, trains, validation des données | `src/engine/`, `include/engine/` | Appels Win32/POSIX, handles système |
| Captures, registres, contrats et commandes | `src/runtime/`, `include/runtime/` | Détails du transport Windows/Linux |
| Démarrage/arrêt des mods | `src/mod/`, `src/loader/mods.cpp` | Chargement de DLL/SO directement |
| Processus, hooks et transports Windows | `src/platform/windows/`, `include/platform/windows/` | Algorithmes indépendants de Windows |
| Processus et transports Linux | `src/platform/linux/`, `include/platform/linux/` | Duplication du noyau commun |
| Adaptateurs inclus dans les kits | `include/nimby/detail/platform/windows/` et `linux/` | Dépendance aux sources privées non installées |
| Contrats Kotlin | `kotlin/`, `kotlin-client/` | Offsets ou pointeurs du jeu dans les mods |
| Sélection et packaging des cibles Kotlin | `gradle-plugin/.../platform/windows/` et `linux/` | Logique de plateforme dispersée dans le plugin principal |
| Diagnostics et scripts spécifiques | `tools/windows/`, `tools/linux/` | Présentation comme API publique |
| Tests spécifiques | `tests/windows/`, `tests/linux/` | Copie des tests purement communs |
| Dépendance MinHook | `third_party/windows/minhook/` | Utilisation par le noyau commun |

Les sélecteurs sous `include/nimby/detail/platform/` choisissent un en-tête. Ils
ne contiennent pas d'implémentation système. Cette organisation est nécessaire
aux adaptateurs compilés depuis un SDK installé : ils n'ont pas accès à `src/`.

## Réseau : exemple concret de mutualisation

`src/engine/network.cpp` contient la topologie des voies, gares, signaux et les
contrôles d'appartenance. Les observations des trains et la planification de leur
calendrier sont dans `src/engine/trains.cpp`. La lecture de la table des états de
textures est dans `src/engine/signal_texture_states.cpp`.

Ces fichiers utilisent le même `ReadMemory`, le même lecteur de collections
`include/engine/detail/memory_reader.h` et le même décodage de position
`include/engine/detail/position.h`. Le lecteur reçoit les octets par callback :
il ne connaît ni `ReadProcessMemory`, ni les descripteurs Linux.

Le lecteur borne les allocations, vérifie les identifiants complets, parcourt
un bloc à la fois et relit les métadonnées. Les buffers sont locaux au parcours.
Une fonction de consommation doit copier ce qu'elle conserve avant le bloc
suivant. Ces contrôles détectent des changements ; ils ne rendent pas une lecture
distante atomique.

## Profils du jeu et plateformes système

`include/engine/game_layout.h` sélectionne des données immuables décrites dans
`include/platform/windows/game_layout.h` et `include/platform/linux/game_layout.h`.
Les profils regroupent les offsets qui diffèrent, les conventions de chaînes,
les tables UI et les capacités de capture déjà identifiées.

Ils décrivent le **binaire cible**, pas l'OS qui exécute un test. Un test Windows
peut donc vérifier le décodage d'un objet Linux artificiel. Ces fichiers de
données n'incluent aucun en-tête système. Les layouts encore communs restent
auprès des lecteurs concernés ; les déplacer tous en offsets anonymes ne serait
pas une amélioration de maintenance.

La reconnaissance SHA-256 précède la lecture des structures du jeu. Un profil
inconnu ne doit jamais être remplacé par le profil Windows par défaut. Les
surcharges historiques sans argument de profil gardent leur contrat Windows ;
le nouveau runtime passe toujours le profil explicite de sa connexion.

## Sessions et captures

`platform::ObservationProcess` est la frontière privée du runtime. Son interface
ne contient pas de handle système. Chaque implémentation détient ses ressources
dans un objet privé, détruit avec la session.

`src/runtime/observation.cpp` possède les sessions et captures. Le mutex de registre
protège les recherches, insertions et retraits de handles ; il n'est pas conservé
pendant l'ouverture du processus, les lectures distantes, la construction des
enregistrements ou leur copie vers l'appelant. Une référence partagée maintient
l'objet en vie après la recherche. Chaque session sérialise ses propres opérations
avec son verrou, sans retenir celui d'une autre session.

Une capture possède ses données et reste indépendante du processus et des autres
captures. La fermeture d'une session ne libère pas automatiquement ses captures.
Les identifiants sont croissants et ne sont pas réutilisés après libération.
Le registre accepte **32 sessions ouvertes** et chaque session **16 captures
retenues**, y compris une capture en préparation. La fermeture retire le handle
sans attendre une lecture en cours ; cette lecture ne peut plus publier son
résultat. Les captures déjà publiées restent à libérer explicitement, même après
la fermeture. Les plafonds par session ne constituent donc ni une limite globale
du nombre de captures historiques, ni un budget mémoire en octets.

Les sessions d'un même processus cible partagent l'identité du monde et l'ordre
des observations. Les tickets empêchent une capture ancienne achevée en retard
de faire reculer cette identité. Pour les mods isolés, le SDK dans le jeu fournit
l'autorité commune aux workers. Les contrôles de racines, de version et de temps
ne figent pas la simulation et ne rendent pas ses lectures atomiques.

## Textures : logique commune et transport

`include/runtime/texture_table.h` possède la table ordonnée, l'expiration, les
remplacements et le calcul de phase. `texture_publications.h` gère les versions
immuables et la propriété des commandes. La préparation, la copie et la fusion
se font hors du verrou de publication ; une courte section critique vérifie la
version attendue puis échange la version visible. Les destructions restent sur
le producteur, après libération de ce verrou.

Le callback de rendu épingle la version visible par des opérations atomiques,
recherche le signal et copie sa commande. Ce chemin SDK n'attend aucun mutex de
producteur, n'alloue rien et ne détruit aucun ancien tableau. Au plus huit versions
retirées sont retenues par ce mécanisme ; un lecteur suspendu peut provoquer un refus de
publication plutôt qu'une rétention illimitée. Le tableau courant reste visible
pendant la contention, sous réserve de son monde et de son bail.

La publication valide un lot entier avant mutation : 4096 commandes par lot et
par propriétaire, 64 propriétaires et 65536 commandes vivantes au total. Le
nettoyage ne retire que les commandes de son propriétaire ; un ID absent ou
repris par un autre propriétaire est déjà nettoyé pour l'appelant. Un retrait
invalide ses préparations en vol sans annuler celles des autres propriétaires.
Le bail utilise le temps monotone ; la phase animée utilise le temps de simulation.

`src/runtime/texture_client.cpp` conserve les validations de l'API, la résolution
du catalogue et la cohérence du monde. Il dépend de `platform::TextureConnection`.

Les mods isolés passent par un canal de courtier par mod ; le courtier attribue
le propriétaire. Le protocole historique `Mailbox` reste disponible pour les
diagnostics externes : `texture_commands.h` et `texture_dispatch.h` raccordent
ses valeurs au transport. Les événements, mappings, barrières `Interlocked` et
noms de DLL restent Windows. La sérialisation d'une requête de transport ne
constitue pas un verrou du rendu ; `volatile` seul ne synchronise rien.

Le pont valide le monde observé, y compris les transitions détectables à racines
réutilisées. Une commande expirée, un monde invalide ou une ressource illisible
rend la main au rendu natif. Ce repli n'est pas une garantie d'indication rouge ;
le rendu et les permissions de conduite restent des contrats distincts.

`texture_runtime` couvre les calculs de table, dont un cas de 100 000 entrées qui
n'est pas le plafond d'admission du pont. `texture_publications`, les tests natifs
de publication et de monde, et les tests RPC couvrent concurrence, propriétaires,
retraits et expiration. Ils ne remplacent pas les mesures de rendu dans le jeu.

## Calendrier et conduite

`engine/calendar_update.h` prépare les écritures de calendrier et exécute leur
transaction avec une fonction d'écriture vérifiée fournie par la plateforme.
Une écriture échouée peut être partielle : son annulation est tentée avant celle
des écritures précédentes, dans l'ordre inverse. Un échec reste un échec même
si l'annulation semble réussir ; le client doit relire l'horloge.

Le chemin externe de translation de calendrier Windows possède la suspension et
la reprise du processus, la vérification de son identité et `WriteProcessMemory`.
Un garde tente la reprise sur les sorties d'erreur. Les appels depuis le jeu et
les mods isolés utilisent au contraire le pont d'horloge au point de mise à jour
natif : tuer un worker ne doit jamais laisser le jeu suspendu. Le recalcul des
trains et la construction passent aussi par leurs commandes natives bornées,
avec expiration et validation du demandeur. Voir [l'horloge](simulation-clock.md).
Linux ne fournit pas encore ces opérations ; partager les calculs ne lui donne
pas implicitement cette capacité.

`engine/automatic_controller.h` porte les états de trains, valide les publications,
prépare les décisions et engage les mouvements réellement effectués.
`engine/driving_command.h` borne les commandes ciblées et calcule leurs paramètres.

Sous Windows, `runtime/driving_state.h` sépare les publications immuables et les
états mutables par train. Les hooks épinglent la publication visible ; seuls les
producteurs prennent le verrou de remplacement. Le garde d'un train couvre son
intégration native et la validation de son mouvement, avec emprunt réentrant sur
le même thread. Les autres trains n'acquièrent pas ce garde. Les emplacements
restent stables pendant la vie du monde ; un changement de génération native ou
de mouvement réinitialise l'état concerné.

Chaque source de consigne possède aussi une incarnation interne. Retirer puis
republier le même identifiant, même avec le même propriétaire, invalide les
instructions retenues de son incarnation précédente. Une publication reçue
pendant un mouvement ne peut ainsi ressusciter un passage retiré. Les gardes de
monde, les baux et les permissions natives restent nécessaires. Les plafonds de
mémoire et les refus sont détaillés dans [les règles de concurrence](memory-and-performance.md#concurrence).

`engine/physical_view.h` contient le calcul de couverture observée et sa fraîcheur.
La plateforme fournit le temps monotone. Les conventions d'appel des hooks et
les captures de registres natifs restent dans le backend concerné.

## Mods et diagnostics

`src/loader/mods.cpp` assure découverte, tri et cycle de vie. `include/loader/manifest.h`
analyse les manifestes après lecture bornée à 64 Kio, BOM compris. Sous Windows,
chaque mod admis est chargé dans son propre `NimbyRailsFranceModHost.exe`, supervisé
par `runtime/mod_host.cpp`. Les captures sont construites dans le worker ; les
services du jeu utilisent son canal dédié et des identifiants de propriétaire vérifiés.
Un crash ou un callback bloqué entraîne l'arrêt du worker concerné et le retrait
de ses ressources, sans exécuter son nettoyage fautif dans le jeu.

L'admission du lot entier et les quotas CPU, mémoire et requêtes sont décrits dans
[les règles de ressources](memory-and-performance.md). Ces limites sont fixées
au démarrage ; un voisin ne peut pas emprunter le quota d'un autre mod. Le
matériel et le code des ponts dans le jeu restent partagés : l'isolation n'est
pas une garantie de latence nulle, ni une frontière de sécurité contre du code
volontairement hostile disposant des droits de l'utilisateur.

`include/research/train_monitor_model.h` partage les lectures et validations du moniteur ;
`tools/signalling-study.cpp` utilise les adaptateurs communs de bibliothèques.
Voir [l'audit Windows](windows-audit.md) pour les responsabilités restantes et la
compatibilité des manifestes.

## Contrats à préserver

- Noms des exports, versions d'ABI, tailles et alignements des enregistrements.
- Absence d'exception C++ ou Kotlin qui traverse une frontière ABI.
- Buffers de sortie possédés par l'appelant ; erreurs explicites et tailles vérifiées.
- Noms des DLL distribuées, cibles CMake installées, tâches Gradle et chemins de paquets.
- Aucune entrée métier, aucun thread, aucune attente sous le verrou de chargement Windows.
- Démarrage/arrêt explicites ; code encore utilisé par un hook ou un runtime conservé chargé.

Le `settings.gradle.kts` du mod localise le kit et son plugin. Il ne
sélectionne pas des sources natives et ne doit pas accumuler des règles Windows/Linux.

## Construction et vérification

Le `CMakeLists.txt` racine orchestre les modules. `cmake/windows/Targets.cmake`
déclare les binaires Windows, `Tests.cmake` leurs vérifications et `Install.cmake` le contrat de distribution. `cmake/linux/Targets.cmake` décrit la cible Linux.

`cmake/Architecture.cmake` vérifie à la configuration et par CTest qu'aucun appel
système identifié, branchement de compilation OS ou dépendance de plateforme
directe n'entre dans les sources communes. C'est un garde-fou textuel, pas une
preuve exhaustive : la revue doit toujours rechercher la logique commune encore
enfermée dans un backend. Les assertions restent actives dans les tests Release.

Voir [les commandes de maintenance](sdk-maintainers.md),
[les règles mémoire et mesures](memory-and-performance.md) et
[la validation Linux en VM](linux-validation.md).

## Ajouter ou modifier une fonction

1. Écrire son contrat : entrées, sorties, propriété, thread, erreurs et limites.
2. Placer les calculs/validations communs auprès du domaine concerné.
3. Introduire une opération plateforme seulement lorsqu'un appel OS l'impose.
4. Documenter la preuve d'un layout nouveau et les capacités encore indisponibles.
5. Ajouter un test de comportement et de panne pertinent, puis construire le kit installé.
6. Pour toute modification de hooks ou d'accès au jeu, effectuer une campagne en jeu
   identifiée par version et SHA-256 ; les tests autonomes ne la remplacent pas.
