# MinHook 1.3.4 — sources x64, liaison statique

`minhook/` contient uniquement quatre fichiers `.c`, six `.h` et `LICENSE.txt`.
Le sous-dossier `hde/` contient le décodeur d'instructions x64 nécessaire aux trampolines.
Les projets Visual Studio, scripts de build, sources x86 et fichiers CI sont omis.

CMake compile ces sources dans une bibliothèque statique privée, ensuite liée
à `NimbyRailsSDK.dll`. Aucune DLL MinHook séparée ni bibliothèque d'import n'est
nécessaire. Les autres dépendances de runtime du SDK restent inchangées.

## Provenance

Sources officielles du tag v1.3.4, téléchargées le 13 septembre 2026 :
https://github.com/TsudaKageyu/minhook/archive/refs/tags/v1.3.4.zip

SHA-256 de l'archive :
`172708123DAA0C98D20D3A980B16A50BE14AF243DC95DEE6F79C24193AD010E4`

Les sources x64 et la licence ont été reprises de cette archive.
Seule adaptation : dans `hook.c`, `../include/MinHook.h` devient `MinHook.h`
pour conserver une arborescence compacte. Aucun changement de comportement.
Les licences MinHook et HDE ainsi que les notices des sources sont conservées.
La compilation ne dépend ni du réseau ni des archives locales sous `build/`.
