# MinHook 1.3.4 — sources x64, liaison statique

`windows/minhook/` contient les sources C, leurs en-têtes privés et `LICENSE.txt`.
Le sous-dossier `hde/` contient le décodeur d'instructions x64 nécessaire aux trampolines.
Les projets Visual Studio, scripts de build, sources x86 et fichiers CI sont omis.

CMake compile ces sources dans une bibliothèque statique privée, ensuite liée
au SDK et à ses ponts Windows. Aucune DLL MinHook séparée ni bibliothèque d'import n'est
nécessaire. Les autres dépendances de runtime du SDK restent inchangées.

## Provenance

Sources officielles du tag v1.3.4, téléchargées le 13 septembre 2026 :
https://github.com/TsudaKageyu/minhook/archive/refs/tags/v1.3.4.zip

SHA-256 de l'archive :
`172708123DAA0C98D20D3A980B16A50BE14AF243DC95DEE6F79C24193AD010E4`

Les sources x64 et la licence ont été reprises de cette archive.
L'inclusion `../include/MinHook.h` de `hook.c` devient `MinHook.h`
pour conserver une arborescence compacte.

Depuis le 3 octobre 2026, l'énumération privée des threads essaie d'abord un
instantané PSS limité au processus courant, au lieu de parcourir les threads
de tous les processus. L'adaptation est limitée à `EnumerateThreads` dans
`hook.c` et au nouvel en-tête privé `thread_snapshot.h`. Les fonctions PSS
sont résolues dynamiquement ; leur absence, un échec de capture, de parcours
ou d'allocation abandonne intégralement la liste partielle puis utilise
l'énumération Toolhelp d'origine. Les threads terminés sont exclus.

Les anciens en-têtes MinGW qui ne fournissent pas `processsnapshot.h` utilisent
les seules déclarations ABI nécessaires dans `thread_snapshot_compat.h`.
Le contrat de liste et de repli est testé avec les deux jeux de déclarations.
La capture réelle est vérifiée séparément sur Windows ; le test indique une
indisponibilité explicite quand les exports PSS sont absents, notamment sous Wine 9.

Seul `PSS_CAPTURE_THREADS` est demandé, sans clonage mémoire, capture de
contexte ni capture de handles. Le marqueur et l'instantané sont libérés
avant toute suspension. Les corps de `Freeze`, `Unfreeze`, la translation
des pointeurs d'instruction et la pose des patches restent ceux de MinHook.
Les courses avec la création ou la disparition de threads ne sont pas
éliminées par cette adaptation ; aucun instantané n'est conservé entre appels.

Contrats Windows : [capture](https://learn.microsoft.com/en-us/windows/win32/api/processsnapshot/nf-processsnapshot-psscapturesnapshot),
[options](https://learn.microsoft.com/en-us/windows/win32/api/processsnapshot/ne-processsnapshot-pss_capture_flags),
[parcours](https://learn.microsoft.com/en-us/windows/win32/api/processsnapshot/nf-processsnapshot-psswalksnapshot),
[entrée de thread](https://learn.microsoft.com/en-us/windows/win32/api/processsnapshot/ns-processsnapshot-pss_thread_entry)
et [libération](https://learn.microsoft.com/en-us/windows/win32/api/processsnapshot/nf-processsnapshot-pssfreesnapshot).

Les licences MinHook et HDE ainsi que les notices des sources sont conservées.
La compilation ne dépend ni du réseau ni des archives locales sous `build/`.

## nlohmann/json 3.12.0

Private catalogue parser: `include/nimby/detail/vendor/json.hpp`, unchanged single header from https://github.com/nlohmann/json/releases/tag/v3.12.0.
SHA-256: `aaf127c04cb31c406e5b04a63f1ae89369fccde6d8fa7cdda1ed4f32dfc5de63`.
MIT licence: `nlohmann-json-LICENSE.MIT`, included in SDK and mod packages.
