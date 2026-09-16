# Dépannage

## Helpers C++

- `nimby/client.hpp` introuvable : utiliser le header de cette révision ou un
  kit reconstruit depuis les sources ; les anciens ZIP 0.6.0 ne l'incluent pas.
- `latest()` renvoie un pointeur nul : aucune capture n'a encore réussi.
  Démarrer `startAutoRefresh()`, attendre `waitForSnapshot(timeout)` et lire
  `getLastError()` en cas de timeout.
- Plusieurs processus détectés : appeler `Client::connect(pid)` avec le PID choisi.
- Données anciennes malgré `waitForSnapshot()` : cette méthode attend une
  première disponibilité, pas forcément une nouvelle capture ; vérifier `getAge()`.
- Vue `span` invalide après rafraîchissement : garder le `Snapshot::Ptr` qui la
  possède, ou copier les objets nécessaires avant de le relâcher.
- Fermeture du jeu : créer un nouveau client pour le nouveau processus ; il n'y
  a pas de reconnexion automatique.

Voir [le tutoriel C++](tutorial-cpp-client.md) et [les contrats complets](cpp-api-reference.md).

[Documentation](README.md) · [Tutoriel](tutorial-first-tool.md) · [Référence](api-reference.md)

Commencer par `MyFirstNimbyTool.exe --check-sdk`. Si ce contrôle fonctionne,
la DLL et ses dépendances se chargent ; passer ensuite à l'ouverture du jeu.

## Compilation et DLL

| Symptôme | Vérification et correction |
|---|---|
| `cmake` ou `ninja` introuvable | Ajouter les outils à `PATH` pour ce terminal, ou compiler depuis CLion ; voir le tutoriel |
| `NimbyRailsSDKConfig.cmake` introuvable | `CMAKE_PREFIX_PATH` doit viser la racine du kit, contenant `lib/cmake/NimbyRailsSDK` ; extraire tout le ZIP |
| CMake refuse la version | Utiliser headers, bibliothèque d'import et DLL du même kit 0.6.x ; la compatibilité CMake 0.x est limitée à la même version mineure |
| Erreur de générateur ou de compilateur dans le cache | Reconfigurer dans un nouveau dossier de build après un changement de toolchain |
| Références non résolues à `NimbySdk_*` | Lier la cible à `NimbyRailsSDK::SDK` ; vérifier que la chaîne est MinGW x64 pour le kit MinGW |
| Bibliothèque `.lib` absente sous MSVC | Reconstruire le SDK avec MSVC x64 ; renommer une bibliothèque MinGW ne la convertit pas |
| `NimbyRailsSDK.dll` ou `libwinpthread-1.dll` absente | Garder toutes les DLL de `bin/` près de l'exécutable ; conserver la commande CMake de copie des runtimes |
| Point d'entrée `NimbySdk_GetVersion` introuvable | DLL antérieure à 0.6 ou DLL différente du kit lié ; remplacer l'ensemble cohérent des DLL de l'application |
| Windows refuse l'image / erreur `0xc000007b` | Vérifier l'architecture x64 et les runtimes correspondants ; éviter les mélanges 32/64 bits |

Avec la liaison normale de l'exemple, Windows résout les imports **avant** `main`.
Une DLL trop ancienne peut donc déclencher une erreur Windows avant que
`--check-sdk` ait l'occasion d'afficher un diagnostic.

## Ouverture du jeu

### `OpenProcess: Process or file access failed`

Refaire la commande suivante et utiliser le PID du jeu actuellement ouvert :

```powershell
Get-Process -Name NIMBYRails | Select-Object Id, ProcessName, Path
```

Vérifier que le jeu et l'outil tournent sous le même utilisateur avec des droits
d'accès compatibles. Ne pas réutiliser le PID d'avant un redémarrage.

### `OpenProcess: Unsupported game binary`

Confirmer le chemin de l'exécutable, puis calculer son empreinte :

```powershell
Get-FileHash -LiteralPath 'C:/Program Files (x86)/Steam/steamapps/common/NIMBY Rails/NIMBYRails.exe' -Algorithm SHA256
```

Adapter le chemin à votre bibliothèque Steam. Comparer avec l'empreinte du
[SDK documenté](README.md#ce-qui-est-expérimental). Un binaire différent exige
un profil validé ; changer seulement un numéro de version ne le rend pas compatible.
Le PID de votre terminal ou de votre outil produit aussi un refus de compatibilité.

### `Process exited; open a new session`

Le processus observé s'est fermé. Libérer les anciens handles, retrouver le PID
de la nouvelle instance, puis ouvrir une nouvelle session. Le SDK ne se reconnecte
pas automatiquement.

## Captures et données

### Cinq tentatives sans capture — sortie 3

Une session peut s'ouvrir alors que le jeu reste dans son menu. Charger une partie,
attendre la fin du chargement et relancer l'exemple. Si les captures restent
indisponibles, conserver le statut, le SHA-256 et la version du SDK pour le diagnostic.

### Vitesse, position ou signal « inconnu »

Les champs ne sont pas tous disponibles ensemble. Tester les bits `flags` du
champ concerné. Une vitesse inconnue n'est pas un train arrêté. L'aspect général
d'un signal reste inconnu dans l'adaptateur actuel même si son sélecteur de texture
est lisible. Voir [les états des signaux](signal-states.md).

### Aucune réservation ou occupation

Examiner le statut de chaque fonction : `OK` avec zéro entrée signifie observé
vide ; `DATA_UNAVAILABLE` signifie inconnu. Les deux collections sont capturées
indépendamment. Retirer les anciennes portions affichées lorsque la nouvelle
capture ne les rend plus disponibles.

### Nom de gare vide / accents incorrects

Un nom vide peut être un nom automatique non résolu. Pour des caractères mal
affichés, les chaînes du SDK sont UTF-8 : utiliser un terminal configuré en UTF-8.
Dans une console Windows, `chcp 65001` sélectionne cette page de codes pour la session.
Le premier exemple conserve les chaînes UTF-8 sans conversion locale.

### `Resource limit; release handles`

Chaque capture réussie doit être suivie de `ReleaseSnapshot`, même si un autre
appel échoue. Fermer aussi les sessions inutilisées. Le premier exemple utilise
des destructeurs C++ pour ces deux ressources.

## Installation gérée par le Hub

Pour créer un outil externe, utiliser la racine du kit SDK installé et ses DLL
près de l'outil. Le sous-dossier `loader` concerne le chargement dans le jeu.
Faire les mises à jour ou désinstallations d'une installation gérée depuis le Hub.
Pour une installation autonome du proxy, suivre sa [procédure dédiée](install-drop-in.md).

## Informations utiles pour signaler un problème

Fournir le statut complet et le nom de l'opération, la sortie de `--check-sdk`,
le compilateur et son architecture, le SHA-256 de l'exécutable du jeu, les commandes
de compilation et le contexte de la partie (menu, chargement, simulation).
Préciser si le problème se reproduit avec `first-observer`. Ne pas remplacer un
résultat absent par des valeurs inventées pour masquer l'erreur.
