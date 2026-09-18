# Mods C++ via le NRF Loader du Hub

Pour débuter en créant vous-même chaque fichier, suivre
[le tutoriel de création d'un mod](tutorial-create-mod.md).

## Écrire un mod sans code Windows

Inclure `<nimby/mod.hpp>`, implémenter `nimby::Mod nimby::createMod()` et lier
la DLL à `NimbyRailsFranceSDK::Mod`. Cette cible ajoute `entry.cpp` du SDK au
build du mod. L'adaptateur contient tous les exports C et le `DllMain` minimal.
Le mod fournit uniquement des callbacks C++ : `showTexture`, `restoreTexture`,
et éventuellement `start` et `stop`. Aucun objet C++ ne traverse une frontière
DLL : les callbacks et l'adaptateur sont compilés ensemble.

L'adaptateur valide les arguments, convertit les exceptions en codes de retour
et empêche l'arrêt pendant une commande. Un échec de démarrage laisse le mod
inactif ; un échec d'arrêt conserve son état pour permettre une nouvelle tentative.
Les callbacks ne doivent pas rappeler les exports du même adaptateur (verrou non
récursif). Les exports génériques de texture sont `NRFMod_ShowTextureV1` et
`NRFMod_RestoreTextureV1` ; `NRFMod_IsInitializedV1` sert au diagnostic.

## Protocole du loader

Le proxy SDL initialise le SDK puis les modules de `<jeu>/NRFMods/<projet>/`.
Chaque dossier déclare sa DLL dans `nrf-mod.ini`, section `[NRFMod]`, clé `library`.
Exemple : `library=SignalisationFrancaiseRealisteMod.dll`. Seul un nom de fichier
DLL est accepté, sans chemin, ni remontée de dossier.
Le Hub crée ces dossiers sous forme de jonctions vers les projets installés qui
déclarent `loaderApi: 1`. Il ne faut pas copier les DLL du SDK dans chaque mod.
Les dépendances propres à un mod peuvent se trouver près de sa DLL.

Exports C, convention WINAPI, sans exception traversant la frontière DLL :

```cpp
extern "C" __declspec(dllexport) DWORD WINAPI NRFMod_StartV1(void* reserved);
extern "C" __declspec(dllexport) DWORD WINAPI NRFMod_StopV1(void* reserved);
```

`reserved` vaut nullptr. Start retourne 0 pour succès, 4 si déjà démarré ; Stop
retourne 0 pour succès. Les autres valeurs sont des erreurs. DllMain ne doit pas
initialiser le mod. Start intervient après SDL_Init, avant la disponibilité de la
partie. Stop intervient avant l'arrêt du SDK, dans l'ordre inverse du démarrage.
Si Stop échoue, le loader conserve le SDK. Les références DLL sont conservées
jusqu'à la fermeture du processus. Il n'y a pas de rechargement à chaud.

Le contrôle des empreintes du jeu et de SDL reste obligatoire avant de charger
les mods. Une DLL manquante ou un export absent est journalisé et ne bloque pas
les autres mods. Un crash à l'intérieur d'un mod peut affecter le jeu : les mods
installés sont du code natif de confiance, pas des scripts isolés.

Cette fonction appartient au proxy SDL installé par le Hub. Le chargeur externe
`NimbyRailsFranceLoader.exe` garde son rôle de diagnostic du SDK.
