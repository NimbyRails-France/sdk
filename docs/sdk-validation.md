# Validation du paquet 0.2.0 — 14 septembre 2026

Windows x64, MinGW GCC 15.2.0 fourni par CLion. **10/10 tests réussis en Debug et
Release**. Le client C11 valide la compilation du header public et les tailles ABI :
Train 304, Track 24, Station 272, Signal 32, SnapshotInfo 112 octets.

La DLL Release installée a le SHA-256
`535833D67C6593C68CF4C8CA5444BFF7BF2F62E0C3CB685F8911A5847641947C`.
Ses imports directs sont `bcrypt.dll`, `KERNEL32.dll`, `msvcrt.dll` et
`libwinpthread-1.dll`. Les dépendances et notices sont présentes dans le paquet.
MSVC n'a pas été testé.

`tools/package.ps1` a installé les seuls headers publics `nimby/sdk.h` et
`nimby/observation.h`, exporté `NimbyRailsSDK::SDK`, puis compilé une copie de
l'exemple installé dans `build/sdk-consumer/source`. Ce projet utilise seulement
`find_package` et la bibliothèque d'import du paquet. Les fichiers CMake exportés
ne contiennent aucun chemin vers le dépôt ou la recherche Ghidra.

L'exécutable consommateur a réellement lu le processus NIMBY Rails **53392** :
5 trains, 379 voies, 9 gares et 27 signaux, sur cinq captures successives. Les
vitesses et positions évoluent. Le jeu conserve son empreinte de recherche
`FFF49AC21720ABFC824C2B4F68B862727630EB0DB71CFE1F9EA8F685D0DB10AE`.

Le test public optionnel `nimby_observation_tests.exe 53392` a également vérifié :
refus d'un buffer trop petit sans écriture partielle, consultation du même snapshot
après fermeture de session, égalité des copies et refus après libération du handle.
Les tests de disposition mémoire utilisent séparément des données entièrement
simulées ; ils ne démontrent pas la synchronisation avec le thread du jeu.

L'installateur du proxy a été exercé sur des **copies** dans
`build/sdk-install-fixture-02`, installation et retrait de la dépendance compris.
La SDL originale de cette copie retrouve son empreinte initiale après retrait.
L'installation réelle du jeu n'a pas été mise à jour pendant cette livraison.

Journaux générés conservés : `build/sdk-package-validation.txt`,
`build/sdk-api-proof.txt`, `build/sdk-consumer/live-proof.txt`,
`build/sdk-installer-proof.txt`. Le moniteur historique a été relancé après
compilation ; le programme exemple s'arrête normalement après ses captures.

Contrat, limitations et utilisation : [guide développeur](developing.md).
