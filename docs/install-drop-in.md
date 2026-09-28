# Installation dans le dossier de NIMBY Rails

Pour un programme externe, le [kit de développement](tutorial-first-tool.md)
suffit. Cette page concerne uniquement le chargement dans le processus du jeu.
Si l'installation est gérée par le Hub, utiliser le Hub pour la modifier ou la retirer.

Télécharger **NimbyRailsFranceSDK-0.7.2-drop-in-windows-x64.zip** dans les Assets de
https://github.com/NimbyRails-France/sdk/releases/tag/v0.7.2 et extraire tout le ZIP.
Ce paquet contient SDL3.dll (notre proxy), NimbyRailsFranceSDK.dll et
libwinpthread-1.dll (dépendance MinGW obligatoire), ainsi que
NimbyRailsFranceTextureBridge-experimental-v3.dll pour les commandes visuelles.
La SDL originale du jeu n'est pas distribuée.

## Installation recommandée

Fermer le jeu. Depuis le dossier extrait, exécuter :

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File ./install-proxy.ps1 -GameDirectory "C:/Program Files (x86)/Steam/steamapps/common/NIMBY Rails"
```

L'installateur vérifie le jeu et sa SDL, conserve SDL3.dll sous le nom
NimbyRailsSDL3Original.dll, puis copie les DLL du paquet. Il crée un manifeste
pour permettre la restauration. Il refuse d'écraser une installation existante.
Si une ancienne installation gérée par ce script existe, la retirer d'abord
avec son ancien script et `-Action Remove`, puis installer ce paquet. Conserver le manifeste dans le dossier du jeu.

Lancer ensuite le jeu normalement depuis Steam : le SDK est chargé après SDL_Init
à chaque démarrage. Aucun chargeur externe à lancer. Ne pas lancer NimbyRailsFranceLoader en parallèle.
Journal : `%LOCALAPPDATA%/NimbyRailsFrance/logs/loader/SDL3_dll.log`.
Message attendu : `OK: SDK initialized after SDL_Init; game hooks disabled`.
Le loader démarre aussi les mods enregistrés dans `NRFMods`. Le pont de textures
est activé à la demande lors d'une commande visuelle, pas par le simple chargement
du SDK. Pour installer ces mods avec le Hub, utiliser le Hub 0.2.3 ou plus récent.

## Copie manuelle

Jeu fermé et sans ancienne installation du proxy :
1. Dans le dossier de NimbyRails.exe, renommer la SDL3.dll originale en **NimbyRailsSDL3Original.dll**.
2. Copier **SDL3.dll**, **NimbyRailsFranceSDK.dll**, **libwinpthread-1.dll** et **NimbyRailsFranceTextureBridge-experimental-v3.dll** du paquet dans ce même dossier.
3. Lancer le jeu normalement. Ne jamais supprimer ni écraser la SDL originale conservée.

Si NimbyRailsSDL3Original.dll existe déjà, ne pas renommer le proxy SDL3.dll par-dessus.
Retirer d'abord l'installation précédente avec sa procédure correspondante.

## Désinstaller

Installation par script, jeu fermé :

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File ./install-proxy.ps1 -Action Remove -GameDirectory "C:/Program Files (x86)/Steam/steamapps/common/NIMBY Rails"
```

Le script restaure la SDL originale et retire uniquement les fichiers vérifiés.
Pour une installation manuelle uniquement : retirer les quatre DLL ajoutées,
puis renommer NimbyRailsSDL3Original.dll en SDL3.dll. Le script de retrait exige
son manifeste ; il ne peut pas retirer une installation manuelle.

## Gestion des DLL dans le script actuel

Ces règles concernent le script source mis à jour, pas les anciennes archives
déjà publiées. Utiliser également le Hub mis à jour pour la réparation.

Une DLL déjà présente est réutilisée seulement si son SHA-256 est identique à
celui du paquet. Toutes les DLL réutilisées sont inscrites dans `sharedFiles`
d'un manifeste de format 3 : désinstallation, réparation et retour arrière les
conservent. Les fichiers copiés par le SDK restent sa propriété et sont retirés
seulement après vérification de leur contenu. Une version différente bloque
l'opération sans écrasement.

Exception documentée : l'ancienne `libwinpthread-1.dll` d'empreinte
`1179c0c0ed77abb4aa92a14db97f369cdf364d810167d251b0dfe466db004c21`
(également distribuée avec le SDK 0.6.6) peut migrer vers la version du paquet.
Le script conserve le fichier précédent sous `libwinpthread-1.dll.nrf-before-sdk`
et enregistre son empreinte dans `replacedFiles`. Retrait, réparation et échec
d'installation restaurent cette version précédente ; une version inconnue reste
un conflit. Ne pas supprimer ces sauvegardes manuellement.

Le script écrit `NimbyRailsFranceSDK-install.tmp` avant de copier les DLL et le
renomme en manifeste final après validation. En cas d'arrêt brutal, conserver
ce fichier : la réparation du Hub peut utiliser ses empreintes et une SDL
originale vérifiée pour revenir à un état réinstallable. Une DLL gérée manquante
n'empêche plus cette récupération. Un fichier modifié ou une sauvegarde SDL
invalide reste un conflit explicite.

Tests autonomes, sans jeu installé ni exécution des DLL :
`powershell -NoProfile -File tests/windows/proxy-ownership.ps1`.

## Compatibilité

Windows x64, jeu SHA-256 :
FFF49AC21720ABFC824C2B4F68B862727630EB0DB71CFE1F9EA8F685D0DB10AE
SDL originale SHA-256 :
2A2704678BF6C9C6A944270AB35079DF76F5AFE92B780394ED72D9C8218B98D8

Ce proxy cible les exports de cette SDL précise. Pour une autre version, attendre
une adaptation. Le SDK refuse son activation sur un binaire inconnu.
Le TCO est une application externe distincte ; ce ZIP ne contient pas le TCO.
