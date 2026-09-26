# Audit du code conservé dans Windows

Le critère est la responsabilité du code, pas la présence d'un en-tête Windows.
Une validation de règles de conduite est commune ; vérifier les octets d'une
fonction MSVC avant un hook reste propre au binaire Windows.

## Extractions réalisées

Les chemins sont relatifs à la racine du SDK.

| Domaine | Implémentation commune | Rôle Windows restant |
| --- | --- | --- |
| Sessions et captures | `src/runtime/observation.cpp`, `include/platform/observation_process.h` | Ouverture, identité, lecture, suspension et écritures du processus |
| Cycle de vie | `src/runtime/runtime.cpp`, `include/platform/runtime.h` | `DllMain`, bootstrap, identité de l'hôte et MinHook |
| Réseau et trains | `src/engine/network.cpp`, `trains.cpp`, `signal_texture_states.cpp`, lecteurs dans `include/engine/detail/` | Données de profil, aucun parcours du réseau dupliqué dans Windows |
| Commandes de textures | `include/runtime/texture_table.h`, `texture_commands.h` | Mapping, événements, verrous, publication `Interlocked` et raccordement dans `texture_dispatch.h` |
| API de textures | `src/runtime/texture_client.cpp`, `include/platform/texture_connection.h` | Ressources, chargement du bridge, sérialisation et attente |
| Conduite automatique | `include/engine/automatic_driving.h`, `automatic_controller.h`, `physical_view.h` | Capture des paramètres natifs, appels originaux, portée des callbacks, synchronisation et télémétrie |
| Commande de conduite ciblée | `include/engine/driving_command.h` | Hook, lecture des paramètres et publication de l'état |
| Calendrier | `include/engine/calendar_update.h`, `simulation_clock.h`, `src/engine/trains.cpp` | Suspension/reprise ; intervention native avec l'ABI et l'allocateur du jeu |
| Mods | `src/loader/mods.cpp`, `include/loader/manifest.h` | Extension, casse, chargement et résolution des exports dans `src/platform/windows/mods.cpp` |
| Moniteur de recherche | `include/research/train_monitor_model.h` | Fenêtres Win32, accès mémoire, écritures expérimentales, chaînes MSVC ; offsets dans `include/platform/windows/research/train_monitor_layout.h` |
| Étude de signalisation | `tools/signalling-study.cpp` | Adaptateur de chargement dans `include/nimby/detail/platform/windows/native_library.hpp` |
| Consommateurs C++ et Kotlin | Façades `include/nimby/detail/platform/`, adaptateurs et plugin Gradle communs | Processus, bibliothèques, chemins, remplacement atomique et choix des binaires |

Le contrôleur prépare une décision avant l'intégration native et valide ensuite
le mouvement réellement effectué. Le test commun vérifie cette distinction,
les règles dupliquées et l'invalidation d'une preuve d'arrêt.

L'API de textures possède ses validations, la résolution du catalogue, le contrôle
du changement de monde et le traitement des lots. Son transport possède les
ressources Windows. Le format partagé des bridges reste distinct des classes
C++ communes : ses tailles et ses barrières de publication sont un contrat.

## Pourquoi les autres fichiers restent Windows

- `engine/binary_identity.cpp` lit les en-têtes PE, verrouille le fichier suivant
  les règles Windows et calcule son empreinte via BCrypt.
- `hooks/backend.cpp` encapsule MinHook ; `runtime/bootstrap.cpp` et `dll_entry.cpp`
  portent les points d'entrée du chargement natif.
- `loader/loader.cpp` vérifie images, pages exécutables, architecture et identité
  avant les appels distants Windows. `loader/main.cpp` supervise ce chargeur avec
  sa console, ses événements et ses chemins d'installation Windows.
- `proxy/main.cpp` et `proxy/SDL3.def` portent les exports PE de SDL, son cycle de
  vie, le verrou du chargeur et la conservation de `GetLastError`.
- `runtime/signal_ui_bridge.cpp` adapte l'ABI de l'éditeur au modèle commun
  `SignalUiEndpoint`. Ses clients et ceux de la conduite chargent et initialisent
  les DLL dans le processus.
- Les clients d'horloge et de textures gèrent des protocoles IPC Windows avec
  des récupérations différentes. Un délai expiré n'autorise pas à effacer toute
  commande en cours.
- Les bridges conservent signatures, RVAs, octets attendus, structures natives
  et adaptations du rendu propres au binaire Windows.
- Les sondes restantes sous `tools/windows/` observent mappings, registres,
  processus ou structures de recherche Windows ; elles ne qualifient pas Linux.

Cette couche conserve aussi son orchestration locale. Chaque test de retour
Win32 ou argument de console n'exige pas une interface virtuelle. Une nouvelle
règle de domaine, un parcours ou un calcul réutilisable doit rejoindre le commun.

## Manifestes et compatibilité

Découverte, tri et analyse de `nrf-mod.ini` sont partagés. Valeurs vides, doublons
de `library`, traversées de répertoires et extensions incompatibles sont refusés.
Sous Windows, la casse reste ignorée et les valeurs entre guillemets acceptées.
Les dossiers sans manifeste sont ignorés. Le parseur accepte UTF-8/ASCII avec
BOM UTF-8 facultatif, ainsi que les manifestes UTF-16 avec BOM de Windows.
Les noms de bibliothèques restent limités au jeu de caractères ASCII autorisé.

## Validation et portabilité

Les tests autonomes couvrent modèles communs et raccordements Windows avec des
données artificielles. Ils ne remplacent pas une campagne dans le jeu. Les nouveaux
tests sont déclarés pour les deux plateformes dans `cmake/PortableTests.cmake` ;
leur exécution Linux reste à faire dans la VM.

Vérifications locales du 22 septembre 2026, sous Windows x64 :

| Vérification | Résultat |
| --- | --- |
| Compilation MinGW GCC 15.2, Debug et Release | Réussie |
| CTest Debug | 50/50 |
| CTest Release, assertions actives | 50/50 |
| Plugin Gradle, JDK 21 | Construction réussie, 9 tests |
| Client Kotlin/JVM, JDK 21 | Construction réussie, 6 tests |
| En-têtes du SDK installé, sans options privées | Compilation réussie |
| Adaptateur Kotlin et outils natifs depuis ce SDK installé | Compilation réussie |

Le client JVM signale des dépréciations Gradle existantes. Le cycle de vie d'un
véritable mod Kotlin en jeu, les performances en jeu et la VM Linux n'ont pas
été validés pendant cette passe.

Linux n'a pas de transport de textures qualifié. L'interface prépare ce raccordement
sans annoncer cette fonctionnalité disponible. Voir [la validation Linux](linux-validation.md).
