# Intégration MinHook — 13 septembre 2026

MinHook v1.3.4 intégré depuis le dépôt officiel :
https://github.com/TsudaKageyu/minhook/tree/v1.3.4
Provenance et empreinte de l'archive : `../../third_party/README.md`.

Ce document complète le premier jalon, dont la mention « backend MinHook non
intégré » décrit l'état précédent. Les cibles du jeu restent toutes non validées.

La dépendance est compilée en C et liée statiquement dans le SDK.
Le dossier `third_party/minhook/` contient seulement quatre fichiers `.c`, six `.h`
et la licence nécessaires à AMD64. Aucun binaire MinHook externe n'est requis.
Seul le chemin d'inclusion de MinHook.h dans hook.c a été adapté au rangement local.
Nos propres headers sont regroupés dans `include/`.
Son API ne traverse pas `include/nimby/sdk.h` ni l'ABI destinée aux mods.
`src/hooks/backend.cpp` et `include/hooks/backend.h` en
possède explicitement le cycle de vie ; l'état est sérialisé par le verrou runtime.
Une initialisation MinHook déjà possédée par un autre utilisateur n'est pas
considérée comme notre succès. Les erreurs sont journalisées avec leur nom MinHook.
L'arrêt ne masque pas une erreur MinHook et ne déclare pas le SDK arrêté dans ce cas.

Validation Debug et Release : 3/3 tests réussis dans chaque configuration. Le test MinHook utilise une fonction témoin
séparée (`int __cdecl(int)`), dont le code est fourni dans `tests/hook_target.cpp`.
Il vérifie le refus d'une cible nulle, l'appel initial, le trampoline, l'observation
par quatre threads (4000 appels), le résultat inchangé, le retrait après jonction
des threads et trois cycles complets. Il ne mesure pas les performances du jeu
et ne valide pas le retrait simultané à des appels de detour actifs.

Le backend doit rester interne. Avant tout futur hook du jeu, le registre de
compatibilité et les validations décrites dans `first-milestone.md` restent requis.
`MH_Uninitialize` n'est pas un mécanisme de drainage des callbacks : ceux-ci devront
être arrêtés et terminés avant le retrait des trampolines ou le déchargement du SDK.

Le chargement automatique est une question séparée de MinHook. Il requiert de
lever la contrainte utilisateur interdisant l'injection à ce stade ; aucune
injection ni installation persistante n'a été réalisée pour cette intégration.
