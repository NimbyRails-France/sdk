# Première inspection — 13 septembre 2026

## Sources et identité

README, rapports JSON, journaux et script `InspectNimby.java` lus dans
`C:/Users/samsv/IdeaProjects/NimbyRailsResearch`. Les trois JSON sources sont
préservés dans `source-reports/`. Le script original limite les références à 20
par chaîne : l'inventaire n'est pas une cartographie exhaustive des appels.

Original : `C:/Program Files (x86)/Steam/steamapps/common/NIMBY Rails/NimbyRails.exe`.
Copie : `C:/Users/samsv/IdeaProjects/NimbyRailsResearch/inputs/NimbyRails.exe`.
Les deux fichiers donnent SHA-256
`FFF49AC21720ABFC824C2B4F68B862727630EB0DB71CFE1F9EA8F685D0DB10AE`,
taille 12 583 424 octets. La DLL retrouve cette empreinte indépendamment via BCrypt.
FileVersion, ProductVersion et ProductName sont vides selon Windows VersionInfo :
aucun numéro commercial n'est attribué. Le SHA-256 sert d'identifiant de build.

Ghidra 12.1.3 PUBLIC et son lanceur headless existent. Le rapport d'origine utilise
JDK 25.0.4.1 ; le Java trouvé dans PATH est 22.0.2.9. Aucune nouvelle analyse Ghidra
n'a été lancée et sa base n'a pas été modifiée. L'analyse automatique précédente
annonce 18 914 fonctions et 41 chaînes candidates, avec PDB absent, erreurs PNG/RTTI
et avertissements de décompilation. Les noms FUN sont automatiques.

## Faits vérifiés directement sur le PE

Preuve principale : [pe-inventory.json](reports/pe-inventory.json), généré par
`../../tools/inspect-pe.mjs` en lecture seule. Reproduction avec Node :

```text
node tools/inspect-pe.mjs "chemin/NimbyRails.exe" docs/research/reports/pe-inventory.json
```

Cet utilitaire est un lecteur d'inventaire pour le binaire étudié, pas un validateur
de sécurité de PE arbitraires. Les RVA d'IAT désignent des cellules de pointeurs,
jamais l'adresse d'une fonction à détourner.

| Élément | Valeur / constat |
| --- | --- |
| Machine / format | 0x8664, PE32+ AMD64, sous-système GUI 2 |
| ImageBase préférée | 0x140000000 ; ne pas utiliser comme base chargée |
| Entrée PE | RVA 0x9844F4 |
| SizeOfImage | 13 144 064 octets |
| DllCharacteristics | 0x8160, dont DYNAMIC_BASE et NX_COMPAT |
| Symboles COFF | 0 |
| Exports nommés | 14 : deux variables GPU et douze noms lerc_* ; aucune API de simulation nommée |
| Debug CodeView | RSDS, âge 1, GUID brut 8b24c9404e8904499064dd6366bbfc1a |
| Chemin PDB embarqué | C:\development\nimby\build\win-msvc\Release\NimbyRails.pdb |
| TLS | Répertoire présent, callbacks non examinés |

Imports observés : SDL3.dll, steam_api64.dll, libtcc.dll, BugSplat64.dll,
MSVCP140/VCRUNTIME140 et bibliothèques Windows (liste complète dans le JSON).
`SDL_Init`, `SDL_PollEvent`, `SteamAPI_RunCallbacks`, `CreateThread` et
`QueryPerformanceCounter` donnent des pistes d'initialisation, d'événements et
d'ordonnancement. Leur import ne démontre ni leur fréquence ni le thread utilisateur.
Le chemin PDB et les imports constituent des indices MSVC ; aucune compatibilité
des objets C++ du moteur avec MinGW n'est supposée.

## Initialisation : preuve de contrôle de flux limitée

[entry-disassembly.txt](reports/entry-disassembly.txt), produit par l'objdump livré
avec CLion sur l'original, contient les octets de l'entrée PE :

- RVA 0x9844F4 : `sub rsp, 0x28`.
- RVA 0x9844F8 : appel direct vers RVA 0x984E94.
- RVA 0x9844FD : restauration de rsp.
- RVA 0x984501 : saut direct vers RVA 0x984380.

Fait : ces instructions sont présentes dans le fichier. Hypothèse : séquence
d'amorçage CRT MSVC. Les rôles des deux destinations, leur signature, les callbacks
TLS et le chemin vers l'initialisation du jeu restent à vérifier. Ce n'est pas
une identification de la boucle de simulation.

## Registre initial des pistes

Toutes les lignes suivantes se rapportent exclusivement au SHA-256 ci-dessus.
RVA = adresse Ghidra moins 0x140000000. Signature native, convention d'appel exacte,
threads, réentrance et durées de vie : **non établis pour chaque piste**.
Aucun pointeur de fonction ni motif de recherche n'est introduit dans le runtime.

| Piste | RVA | Références / preuve | Statut et hypothèse |
| --- | --- | --- | --- |
| FUN_140424c00 | 0x424C00 | chaîne event_signal_lookahead à 0xA71C90 ; références 0x4255AA, 0x4255B1, 0x425B70, 0x425B77, 0x425E75, 0x425E7C | Références statiques rapportées ; possible enregistrement de l'API |
| FUN_140428790 | 0x428790 | SignalLookaheadResult à 0xA72390 depuis 0x429A90 ; max_speed depuis 0x42D2DE et 0x431E1A | Références statiques rapportées ; possible description de types/champs |
| FUN_1405a1740 | 0x5A1740 | chaîne avec retour à la ligne event_signal_lookahead à 0xA854A0 depuis 0x5A1AEB | Référence rapportée ; rôle indéterminé |
| SDL_PollEvent, cellule IAT | 0x9AB8F0 | import SDL3.dll lu directement | Piste d'observation de boucle d'événements ; aucun lien prouvé avec un tick de simulation |
| CreateThread, cellule IAT | 0x9AB150 | import KERNEL32.dll lu directement | Piste pour retrouver les entrées de workers ; pas une cible de hook retenue |

La signature supposée reste volontairement « inconnue » : aucun prototype de
fonction interne n'est déduit d'une simple chaîne. Les chaînes RTTI nommant
`init_sim_extrapolator` et `train_hits_train` ne donnent pas à elles seules leurs
adresses d'entrée. La présence de `max_speed` ne prouve pas l'application du freinage.

## Prochaine validation du premier hook d'observation

Le premier hook reste **à identifier et valider**. Pour orienter la recherche,
examiner d'abord les appelants de SDL_PollEvent, puis les relier ou les distinguer
des workers de simulation. La piste NimbyScript doit être suivie jusqu'au code
qui consomme le résultat, au-delà de l'enregistrement des noms.

1. Dans une session Ghidra exclusive, relever instructions, appelants/appelés,
   tailles et bornes des fonctions. Suivre l'entrée PE et les callbacks TLS.
2. Vérifier chaque prototype avec les sites d'appel et le désassemblage : RCX/RDX/
   R8/R9, XMM, arguments sur pile, retour caché éventuel, alignement et registres
   non volatils. La convention Windows x64 générale ne suffit pas à typer une cible.
3. Lors d'une future session dynamique autorisée, relever thread ID, piles, cadence,
   réentrance et durées de vie, dans menu/pause/simulation et avec un/cinq trains.
   Utiliser le débogueur avant tout detour ; ne pas lancer d'injection pour cette étape.
4. Choisir une observation minimale sans écriture moteur ni pointeur emprunté
   conservé. Définir un compteur ou événement copié, un tampon borné et un budget
   de coût ; préserver arguments, valeur de retour et état observable.
5. Avant MinHook : empreinte autorisée, base réelle, RVA dans section exécutable,
   octets attendus et désassemblage complet de la zone déplacée, relocalisations,
   instructions RIP-relatives, saut/trampoline et unwind vérifiés. Refuser toute
   ambiguïté, différence de bytes ou cible inconnue.
6. Tester installation transactionnelle/rollback, concurrence et retrait avec
   interdiction de nouveaux appels et drainage des appels actifs avant FreeLibrary.
   Comparer le comportement avec/sans observation avant de déclarer un hook validé.

L'identification actuelle porte sur le fichier sur disque, pas l'image chargée :
elle ne détecte pas un patch mémoire et ne suffira donc jamais à activer un hook.
Le registre des cibles validées est vide ; le backend MinHook n'est pas intégré.
La future API de mods utilisera des données copiées et des identifiants stables,
après établissement de leur sémantique et de leur durée de vie.

## Validation du socle

Chaîne observée : GNU C++ 15.2.0 MinGW x64 de CLion, C++20, CMake/Ninja intégrés.
Les tests sont autonomes : ils ne simulent pas le moteur et ne prouvent aucun
comportement des trains. Ils couvrent un témoin PE synthétique dont SHA-256 est
vérifié indépendamment avec Node crypto, les en-têtes invalides/tronqués/x86,
le refus des versions/cibles, les arguments ABI invalides, 8 lecteurs concurrents,
l'arrêt idempotent, la réinitialisation et trois chargements/déchargements de DLL.

La vérification facultative de l'original via l'hôte lit le fichier sans le charger
comme module. Elle reconnaît l'empreinte attendue et vérifie que cette inspection
ne peut pas remplacer l'identité de l'hôte de tests.

Résultat final : build Debug depuis un nouveau répertoire `build-sdk-Debug`,
sans avertissement affiché, **2/2 tests CTest réussis**. Journal conservé dans
`reports/build-and-tests.txt`. Les quatre exports publics sont présents ; la DLL
importe bcrypt, KERNEL32, msvcrt et libstdc++-6. Les runtimes MinGW sont copiés par
CMake. L'empreinte de l'original a été relue en fin de travail et reste identique.
Release et MSVC n'ont pas été exécutés. La capture effective du journal Windows
par un débogueur n'a pas été testée.
