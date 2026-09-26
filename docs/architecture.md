# Architecture et maintenance du SDK

Le SDK conserve C++20 pour le noyau natif et Kotlin pour ses consommateurs.
Windows x64 est la plateforme de référence. Le développement et la publication
Linux sont suspendus ; ses dossiers restent séparés pour organiser le code.

Depuis 0.8.0, le client public C++ est retiré. Les outils utilisent `NimbyClient`
en Kotlin/JVM et les mods implémentent `SignallingMod` en Kotlin/Native.
Le pont précompilé possède une `detail::ObservationSession` native privée,
synchrone et sans cache. `ObservationLoop` reste responsable de la cadence du
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

`runtime/observation.cpp` possède les sessions et captures et sérialise leur accès
par son mutex de registre. Le processus sert à lire les octets ; le runtime
construit les enregistrements, traite les erreurs et expose les copies à l'ABI.

Une capture possède ses données et reste indépendante du processus et des autres
captures. La fermeture d'une session ne libère pas automatiquement ses captures.
Les identifiants sont croissants et ne sont pas réutilisés après libération.
Les plafonds existants sont de huit sessions et seize captures conservées ; ils
ne représentent pas un budget global de mémoire en octets.

## Textures : logique commune et transport

`runtime/texture_table.h` possède la table ordonnée, l'expiration, les remplacements
et le calcul de phase. `runtime/texture_commands.h` traite une `Mailbox` composée
de valeurs : requête, état et réponse. Le temps monotone est fourni à l'appel ;
la phase animée utilise explicitement le temps de simulation.

`src/runtime/texture_client.cpp` conserve les validations de l'API, la résolution
du catalogue et la cohérence du monde. Il dépend de `platform::TextureConnection`.

Le transport Windows conserve son format partagé existant dans
`platform/windows/runtime/texture_bridge.h`. `texture_dispatch.h` copie les
champs utiles vers le modèle commun, exécute la commande, puis recopie le résultat.
Les événements, mappings, champs `Interlocked` et noms de DLL restent Windows.
Le verrou de table est acquis avant l'appel ; le résultat est publié avant de
libérer la requête. `volatile` seul ne constitue pas une synchronisation.

Le test commun `texture_runtime` couvre notamment 100 000 entrées, l'expiration,
les changements de monde et les commandes invalides. `windows_texture_dispatch`
vérifie le raccordement au transport sans lancer le jeu.

## Calendrier et conduite

`engine/calendar_update.h` prépare les écritures de calendrier et exécute leur
transaction avec une fonction d'écriture vérifiée fournie par la plateforme.
Une écriture échouée peut être partielle : son annulation est tentée avant celle
des écritures précédentes, dans l'ordre inverse. Un échec reste un échec même
si l'annulation semble réussir ; le client doit relire l'horloge.

Windows possède la suspension/reprise du processus, la vérification de son
identité et `WriteProcessMemory`. Un garde reprend le processus sur les sorties
d'erreur. Linux ne fournit pas encore cette opération et retourne son erreur
explicite. Partager la transaction ne lui donne pas implicitement cette capacité.

`engine/automatic_controller.h` porte les états de trains, valide les publications,
prépare les décisions et engage les mouvements réellement effectués.
`engine/driving_command.h` borne les commandes ciblées et calcule leurs paramètres.

`engine/physical_view.h` contient le calcul de couverture observée et sa fraîcheur.
La plateforme fournit le temps monotone. Les conventions d'appel des hooks et
les captures de registres natifs restent dans le backend concerné.

## Mods et diagnostics

`src/loader/mods.cpp` assure découverte, tri et cycle de vie. `include/loader/manifest.h`
analyse les manifestes. Les backends fournissent nommage, chargement et exports.
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

`examples/kotlin-mod/settings.gradle.kts` localise le kit et son plugin. Il ne
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
