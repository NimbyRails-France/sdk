# Installation dans le dossier de NIMBY Rails

Pour un programme externe, le [kit de développement](tutorial-first-tool.md)
suffit. Cette page concerne uniquement le chargement dans le processus du jeu.
Si l'installation est gérée par le Hub, utiliser le Hub pour la modifier ou la retirer.

Télécharger **NimbyRailsFranceSDK-0.6.0-drop-in-windows-x64.zip** dans les Assets de
https://github.com/NimbyRails-France/sdk/releases/tag/v0.6.0 et extraire tout le ZIP.
Ce paquet contient SDL3.dll (notre proxy), NimbyRailsFranceSDK.dll et
libwinpthread-1.dll (dépendance MinGW obligatoire). La SDL originale du jeu n'est pas distribuée.

## Installation recommandée

Fermer le jeu. Depuis le dossier extrait, exécuter :

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File ./install-proxy.ps1 -GameDirectory "C:/Program Files (x86)/Steam/steamapps/common/NIMBY Rails"
```

L'installateur vérifie le jeu et sa SDL, conserve SDL3.dll sous le nom
NimbyRailsSDL3Original.dll, puis copie les trois DLL du paquet. Il crée un manifeste
pour permettre la restauration. Il refuse d'écraser une installation existante.
Si une ancienne installation gérée par ce script existe, la retirer d'abord
avec son ancien script et `-Action Remove`, puis installer ce paquet. Conserver le manifeste dans le dossier du jeu.

Lancer ensuite le jeu normalement depuis Steam : le SDK est chargé après SDL_Init
à chaque démarrage. Aucun chargeur externe à lancer. Ne pas lancer NimbyRailsFranceLoader en parallèle.
Journal : `%LOCALAPPDATA%/NimbyRailsFranceSDK/proxy.log`.
Message attendu : `OK: SDK initialized after SDL_Init; game hooks disabled`.
Ce chargement initialise le SDK ; il n'active pas encore les hooks internes du jeu.

## Copie manuelle

Jeu fermé et sans ancienne installation du proxy :
1. Dans le dossier de NimbyRails.exe, renommer la SDL3.dll originale en **NimbyRailsSDL3Original.dll**.
2. Copier **SDL3.dll**, **NimbyRailsFranceSDK.dll** et **libwinpthread-1.dll** du paquet dans ce même dossier.
3. Lancer le jeu normalement. Ne jamais supprimer ni écraser la SDL originale conservée.

Si NimbyRailsSDL3Original.dll existe déjà, ne pas renommer le proxy SDL3.dll par-dessus.
Retirer d'abord l'installation précédente avec sa procédure correspondante.

## Désinstaller

Installation par script, jeu fermé :

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File ./install-proxy.ps1 -Action Remove -GameDirectory "C:/Program Files (x86)/Steam/steamapps/common/NIMBY Rails"
```

Le script restaure la SDL originale et retire uniquement les fichiers vérifiés.
Pour une installation manuelle uniquement : retirer les trois DLL ajoutées,
puis renommer NimbyRailsSDL3Original.dll en SDL3.dll. Le script de retrait exige
son manifeste ; il ne peut pas retirer une installation manuelle.

## Compatibilité

Windows x64, jeu SHA-256 :
FFF49AC21720ABFC824C2B4F68B862727630EB0DB71CFE1F9EA8F685D0DB10AE
SDL originale SHA-256 :
2A2704678BF6C9C6A944270AB35079DF76F5AFE92B780394ED72D9C8218B98D8

Ce proxy cible les exports de cette SDL précise. Pour une autre version, attendre
une adaptation. Le SDK refuse son activation sur un binaire inconnu.
Le TCO est une application externe distincte ; ce ZIP ne contient pas le TCO.
