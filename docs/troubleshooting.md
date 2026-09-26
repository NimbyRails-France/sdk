# Dépannage

## Mods Kotlin et Gradle

| Symptôme | Action |
| --- | --- |
| `Configurer nrfSdkDir` | Définir le chemin du kit extrait dans les réglages utilisateur de Gradle, l'environnement ou le Hub |
| Plugin `fr.nimbyrails.mod` introuvable | Vérifier `gradle-repository/` et la version du plugin ; utiliser un kit complet avec le plugin Gradle |
| `Unsupported class file major version` | Sélectionner JDK 21 pour Gradle ; ne pas utiliser automatiquement le runtime de l'IDE |
| `Incomplete Kotlin SDK` | Extraire à nouveau le kit entier, sans mélanger les fichiers de plusieurs versions |
| SDK hors de l'intervalle déclaré | Choisir un kit compatible avec `sdkMin` et `sdkMaxExclusive` du manifeste |
| Échec de téléchargement au premier build | Vérifier l'accès aux dépôts Gradle et Maven ; le kit ne contient pas toutes les dépendances hors ligne |
| Point d'entrée Kotlin absent | Fournir `nimby.mod.createMod(): SignallingMod` dans les sources du mod |
| Échec de `verifyNativeMod` | Consulter la sortie du testeur et vérifier les deux DLL dans `build/gradle/verification` |
| Changement de code invisible en jeu | Recompiler, préparer le projet local dans le Hub et redémarrer le jeu |
| Projet refusé avant la première compilation | Vérifier les champs du `mod.json` complet et utiliser un Hub prenant en charge les projets Kotlin source |

Consulter le [démarrage Kotlin](kotlin-mods.md) et le [contrat Gradle](gradle-plugin.md).

## Clients Kotlin

- Plusieurs jeux détectés : choisir un PID dans `GameProcesses.discover()` et
  le passer à `NimbyClient.open(library, pid)`.
- Données indisponibles : conserver leur état inconnu. Une vitesse `null` ne
  signifie pas arrêt ; une liste de réservations absente ne signifie pas voie libre.
- Jeu fermé : fermer la connexion et en ouvrir une nouvelle sur le processus
  choisi. Le client ne reconnecte pas automatiquement une ancienne session.
- Ancien include `nimby/client.hpp` : le client C++ est retiré en 0.8.0 ;
  suivre le [guide Kotlin](kotlin-client.md).

La configuration **NRF - Banc - Tests** vérifie le logiciel sans ouvrir le jeu.
Pour les DLL, utiliser le même kit Windows 0.8.x que les dépendances du projet.

## Compilation du noyau et chargement des DLL

Les réglages CMake concernent la fabrication du SDK et de son pont. Un projet
consommateur Kotlin utilise Gradle et le kit précompilé.

| Symptôme | Vérification et correction |
|---|---|
| `cmake` ou `ninja` introuvable | Ajouter les outils à `PATH` pour ce terminal, ou compiler depuis CLion ; voir le tutoriel |
| `NimbyRailsFranceSDKConfig.cmake` introuvable | `CMAKE_PREFIX_PATH` doit viser la racine du kit, contenant `lib/cmake/NimbyRailsFranceSDK` ; extraire tout le ZIP |
| CMake refuse la version | Utiliser headers, bibliothèque d'import et DLL du même kit 0.8.x ; la compatibilité CMake 0.x est limitée à la même version mineure |
| Erreur de générateur ou de compilateur dans le cache | Reconfigurer dans un nouveau dossier de build après un changement de toolchain |
| Références à `NimbySdk_*` | Ancienne API retirée : utiliser `NimbyClient` en Kotlin ; voir le guide Kotlin |
| Bibliothèque `.lib` absente sous MSVC | Reconstruire le SDK avec MSVC x64 ; renommer une bibliothèque MinGW ne la convertit pas |
| `NimbyRailsFranceSDK.dll` ou `libwinpthread-1.dll` absente | Garder toutes les DLL de `bin/` près de l'exécutable ; conserver la commande CMake de copie des runtimes |
| Point d'entrée manquant | Recompiler contre le même kit 0.8 que les DLL distribuées ; les anciens exports sont retirés |
| Windows refuse l'image / erreur `0xc000007b` | Vérifier l'architecture x64 et les runtimes correspondants ; éviter les mélanges 32/64 bits |

Une dépendance native manquante peut empêcher le chargement de la DLL avant
la vérification de version par `NimbyClient.open()`. Conserver les dépendances
du même paquet et vérifier le chemin de bibliothèque choisi dans l'application.

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

Les champs ne sont pas tous disponibles ensemble. Tester les valeurs `optional` du
getter concerné. Une vitesse inconnue n'est pas un train arrêté. L'aspect général
d'un signal reste inconnu dans l'adaptateur actuel même si son sélecteur de texture
est lisible. Voir [les états des signaux](signal-states.md).

### Aucune réservation ou occupation

Examiner chaque collection optionnelle : présente et vide signifie observé
vide ; `nullopt` signifie inconnu. Les deux collections sont capturées
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
