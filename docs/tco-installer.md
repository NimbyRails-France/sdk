> Archive technique antérieure à la 0.7 : noms et API C historiques.
> Pour la version actuelle, utiliser la [référence C++](cpp-api-reference.md) et la [migration](migration-0.7.md).

# Installation et mises à jour du TCO

`dist/NimbyTco-0.3.0-Setup.exe` installe par utilisateur le TCO et le SDK 0.5.0.
La DLL utilisée par le TCO est à côté de l'exécutable ; le kit de développement
complet (headers, bibliothèques, exemples et documentation) est dans `SDK/`.
Le dossier par défaut est `%LOCALAPPDATA%/Programs/NimbyTco`.
Le programme apparaît dans les applications Windows, avec désinstallation et
raccourci du menu Démarrer. Le jeu et les mods ne sont pas modifiés.

## Publier une version

1. Mettre à jour la version du projet TCO dans `examples/tco/CMakeLists.txt`
   et celle du script `package-installer.ps1` (et le SDK si nécessaire).
2. Construire le SDK : `powershell -File tools/package.ps1`.
3. Construire le paquet avec Inno Setup 6 :

```powershell
powershell -File examples/tco/package-installer.ps1 `
  -Iscc 'C:/Program Files (x86)/Inno Setup 6/ISCC.exe' `
  -FeedUrl 'https://votre-serveur/tco-latest.json' `
  -ReleaseBaseUrl 'https://votre-serveur/releases/0.3.0'
```

Les URL ci-dessus sont des exemples à remplacer. Publier d'abord le `.exe`,
puis le fichier `dist/tco-latest.json` à l'URL du feed. GitHub Releases convient
aussi, avec un lien HTTPS stable vers le manifeste. Ne pas modifier le fichier
EXE après génération du manifeste sans recalculer sa taille et son SHA-256.
Signer l'EXE, si un certificat est disponible, **avant** de calculer le manifeste.
Le paquet actuel n'est pas signé : aucun certificat de publication n'est configuré.

Le schéma du manifeste est `schema:1`, `product:"NimbyTco"`,
`platform:"windows-x64"`, `version:"x.y.z"`, `sdkVersion`, `url`, `sha256`,
`size` (octets). La version TCO représente le paquet entier, y compris le SDK.

## Comportement de l'updater

- Lit `update.json` à côté de l'exécutable (`{"feed":"https://…"}`).
- Vérifie au démarrage, puis toutes les six heures ; bouton de vérification manuelle.
- Accepte uniquement une version supérieure, des URL HTTPS, une taille bornée
  et une empreinte SHA-256 exacte. La confiance repose sur le serveur HTTPS du feed.
- Télécharge en arrière-plan dans les données locales du TCO, sans remplacer
  une DLL chargée. Vérifie de nouveau le fichier avant exécution.
- Installe le paquet à la fermeture du TCO. Le bouton « Redémarrer et mettre
  à jour » installe puis relance. Inno Setup traite les fichiers en cours d'utilisation.
- Une erreur réseau ou d'intégrité laisse la version installée intacte.

Sans `FeedUrl`, l'installateur fonctionne hors ligne et le TCO indique
« serveur non configuré ». Aucun service de publication n'est créé implicitement.
Les tests locaux couvrent le manifeste, l'installation et le lancement du paquet ;
le parcours depuis un serveur public demande un feed effectivement publié.
