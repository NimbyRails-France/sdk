# Proxy SDL3 — 13 septembre 2026

Objectif : charger le SDK en lançant le jeu normalement, sans programme externe.
L'exécutable du jeu importe SDL3.dll directement. La bibliothèque originale est
conservée byte pour byte sous `NimbyRailsSDL3Original.dll`. Le nouveau fichier
SDL3.dll expose les mêmes noms et ordinaux et transmet les appels à l'original.

## Provenance vérifiée

- Jeu : SHA-256 `FFF49AC21720ABFC824C2B4F68B862727630EB0DB71CFE1F9EA8F685D0DB10AE`.
- SDL originale : SHA-256 `2A2704678BF6C9C6A944270AB35079DF76F5AFE92B780394ED72D9C8218B98D8`, 2 840 576 octets, AMD64.
- Inventaire complet : `reports/sdl3-inventory.json` (fichier fourni avec le jeu).
- `tools/generate-sdl-proxy.mjs` produit `src/proxy/SDL3.def` à partir de cet inventaire PE.
- 1 271 exports nommés et leurs ordinaux comparés exactement, en respectant la casse.
  1 269 sont des forwarders PE ; SDL_Init et SDL_Quit sont les deux wrappers.
  Le proxy ne dépend que de bcrypt, KERNEL32 et msvcrt avec MinGW.

Prototypes publics utilisés : `bool __cdecl SDL_Init(uint32_t)` et
`void __cdecl SDL_Quit(void)`. Ils correspondent à l'API SDL3 documentée, pas à une
signature de fonction interne du jeu supposée. SDL exige un appel sur le thread
principal. Le proxy ne crée aucun thread et exécute son initialisation sur le
thread appelant après le retour réussi de SDL_Init. Aucun travail de chargement,
hash, journalisation ou synchronisation n'est exécuté dans son DllMain.

Sources SDL : [SDL_Init](https://wiki.libsdl.org/SDL3/SDL_Init),
[SDL_Quit](https://wiki.libsdl.org/SDL3/SDL_Quit).
Les sites d'appel présents dans l'exécutable sont relevés séparément dans
`reports/sdl-call-sites.txt`. L'observation de la simulation et des objets du jeu
reste à faire ; ce proxy ne constitue pas un hook interne de simulation validé.

Dans l'exécutable connu (base préférée 0x140000000), le désassemblage montre :
RVA 0x2D5D75 charge ECX avec 0x20, RVA 0x2D5D7A appelle la cellule IAT SDL_Init
à RVA 0x9AB9F0, puis RVA 0x2D5D80 teste AL. Cela corrobore un argument de flags
32 bits et un retour booléen. SDL_Quit est appelé aux RVA 0x2CCBD2 et 0x2D6447
via la cellule IAT 0x9AB908. Ces adresses servent uniquement de preuves d'analyse,
elles ne sont pas utilisées comme cibles de patch dans le proxy.

## Comportement

Après SDL_Init réussi, le proxy vérifie le jeu et la SDL originale, charge le SDK
voisin et appelle son bootstrap. Un refus conserve le résultat SDL et son dernier
code d'erreur Windows. Le SDK n'utilise pas l'API SDL et ne change pas SDL_GetError.
Un SDK déjà initialisé par un autre propriétaire n'est pas arrêté par le proxy.
Un cycle possédé est arrêté avant SDL_Quit ; les handles de modules sont retenus
jusqu'à la fin du processus, sans retrait concurrent de code.

Le journal du proxy est écrit hors du dossier des sauvegardes, sous
`%LOCALAPPDATA%/NimbyRailsSDK/proxy.log`. La présence d'un log d'initialisation prouve
le bootstrap, pas le fonctionnement d'un hook de simulation. Tous les hooks du
jeu restent désactivés.

## Vérifications effectuées

- Debug et Release : 5/5 tests CTest réussis avec la SDL originale locale fournie
  par `NIMBY_SDL_ORIGINAL` (copie de référence sous build/references).
- Le test proxy compare les adresses résolues des forwarders à celles de l'original,
  vérifie SDL_Init(0)/SDL_Quit sur trois cycles sans ouvrir de fenêtre ni de sauvegarde,
  et confirme le refus du SDK pour son exécutable témoin inconnu.
- L'installation et la restauration ont été exécutées sur une copie isolée des
  fichiers sous build/install-test. Les empreintes d'origine sont restaurées.
- Une erreur dans le test Release (absence de return dans wmain) a été corrigée.
  Le débogueur a montré des modules distincts et un wrapper SDL_Init situé dans le
  proxy. Le test corrigé passe dans les deux configurations.

Le test proxy est optionnel sur une machine sans SDL originale ; les autres tests
restent disponibles. La copie originale n'est pas incluse dans les deux DLL du
dossier drop-in. Pour une autre version SDL, regénérer les exports après analyse
et adapter les empreintes validées ; ne jamais réutiliser aveuglément cette table.

## Installation locale du 14 septembre 2026

Installation effectuée à 15:39:02 UTC dans le dossier Steam de NIMBY Rails,
après fermeture du jeu et du chargeur externe. Les empreintes relues après
installation correspondent au manifeste `NimbyRailsSDK-install.json` :

- Proxy SDL3.dll : `76781627F4619461FF58502C36E548ADBAEE170B4CBD3E0F157C16B7C12F37D7`.
- NimbyRailsSDK.dll : `4BF792B4666BF7E5D3EC7AE342DC91377B58E56161B70161D9DBA9DBB8DA3A52`.
- L'exécutable conserve l'empreinte du jeu indiquée plus haut.
- NimbyRailsSDL3Original.dll conserve exactement l'empreinte SDL d'origine.

Cette vérification confirme les fichiers installés. Le chargement automatique
dans le jeu reste à observer au prochain lancement normal via Steam, avec une
nouvelle entrée `OK: SDK initialized after SDL_Init; game hooks disabled` dans
`%LOCALAPPDATA%/NimbyRailsSDK/proxy.log`. Aucun hook interne n'est activé.
