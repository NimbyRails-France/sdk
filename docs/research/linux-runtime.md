# Linux natif : observations du 21 septembre 2026

## Complément : interface de réglages et accès ordinaire

Le serveur de lecture préchargé et le TCO fonctionnent désormais sans root
sur le même utilisateur. Le client reçoit un descripteur `/proc/self/mem`
ouvert en lecture seule par le jeu et utilise `pread`. La validation de
l'identité du processus et les contrôles du socket restent obligatoires.
L'ancienne lecture par requêtes reste compatible avec un jeu déjà lancé.

Sur le binaire dont l'empreinte figure ci-dessous, une inspection GDB en
lecture seule du PID 63373 a confirmé les tables suivantes, puis s'est détachée :

| Passe | RVA du pointeur de table | Décalage checkbox | RVA de la fonction |
| --- | --- | --- | --- |
| LayoutEmit | `0x1088938` | `0xf8` | `0x7a1280` |
| NuklearEmit | `0x1088b00` | `0xf8` | `0x7a2490` |

Les pointeurs de table commencent après les deux mots d'en-tête de l'ABI
Itanium. `SignalUi::bind` exige un profil explicite pour Linux et vérifie la
fonction au bon emplacement. Les tests rejettent les tables Windows, les
fonctions remplacées et l'ancien décalage `0xf0`.
La capture du signal dans le corps d'éditeur au RVA `0xabd7f0` n'est pas encore
qualifiée ; `editorSignal` refuse le profil Linux. Aucun hook d'interface Linux
ni appel natif de case à cocher n'a été validé par ces tests.

Les sections suivantes conservent les constats du premier diagnostic.

Ces résultats qualifient un lancement et la résolution des racines. Ils ne
constituent pas une validation du SDK Linux complet, de ses hooks ou des
structures Train, Motion et réseau.

## Binaire et racines

- Jeu : NIMBY Rails 1.19.10, ELF x86-64, 20 787 376 octets.
- SHA-256 : `2581d0e8157f43acb137b2bd9d52e2a7c82bd8af8b62fab5d87f00cc27eefde6`.
- Symbole `nimby::shell::ui_state` : RVA `0x10ee020`.
- `shell::start` alloue `0xac0` octets pour UIState et publie ce pointeur.
- UIState construit Lifecycle à `+0x540`. Lifecycle::load écrit la base à
  `+0`, la copie à `+0x80`, la simulation à `+0x140`.
- Donc, depuis UIState : database `+0x540`, copy `+0x5c0`, simulation `+0x680`.

Lecture GDB sur une partie chargée, PID 47903, puis détachement immédiat :

```text
base       = 0x65405692a000
ui_state   = 0x65406e7bf4c0
database   = 0x7f34cc0b0e60
copy       = 0x65406e8062f0
simulation = 0x7f34cc0b2aa0
```

Ces adresses absolues sont une preuve ponctuelle, jamais des constantes du SDK.
`nimby_linux_inspect` vérifie taille et SHA-256 avant cette résolution, effectue
deux lectures concordantes et indique toujours `game_profile=unvalidated`.
L'inspection externe réelle a été lancée avec les droits root dans WSL pour
la lecture mémoire ; aucune politique système ptrace n'a été modifiée. L'accès
du futur client Linux ordinaire reste à intégrer au SDK.
Les tests synthétiques couvrent les deux RVA, les relocations, le remplacement
de racine et les pointeurs nuls ou identiques. Les offsets des autres objets
Windows ne sont pas validés pour Linux : notamment, la taille de Sim diffère.

## Plantage pendant le chargement des textures

La copie `NRF Linux diagnostic copie.nimbyrails5` reproduit le plantage sans
SDK injecté. Un breakpoint conditionnel sur `__cxa_throw`, filtré sur le RTTI
`_ZTISt9bad_alloc`, donne :

```text
operator new(unsigned long) [clone .cold]
nimby::render::load_image_file(...)
nimby::render::TexArrayCache::update()::{lambda()#1}
async::detail::task_func<...>::run(...)
```

Dans load_image_file, le chemin conservé est `/`. La taille issue de tellg,
gardée à `rsp+0x60`, vaut `0x7fffffffffffffff` (9 223 372 036 854 775 807).
L'appel d'allocation est au RVA `0x78b06b`. Ce n'est pas une mesure de RAM
nécessaire à la sauvegarde : le lecteur tente de traiter un dossier comme image.
L'origine amont de ce chemin de texture reste à déterminer.

## Contournement local, distinct du SDK

`tools/linux/directory-read-guard.c` interpose uniquement fopen/fopen64 du
processus lancé avec LD_PRELOAD. Une ouverture en lecture seule d'un dossier
est fermée et retourne EISDIR. Les fichiers ordinaires restent lisibles,
les ouvertures en écriture restent inchangées. Aucun exécutable du jeu n'est
modifié et aucune configuration globale de l'éditeur de liens n'est utilisée.

Construction et test optionnels :

```sh
cmake -S . -B build/linux -DNIMBY_BUILD_LINUX_COMPAT=ON
cmake --build build/linux
ctest --test-dir build/linux --output-on-failure
LD_PRELOAD="$PWD/build/linux/libnimby_directory_read_guard.so" /chemin/vers/nimbyrails
```

Sur la machine WSL de test, le lanceur `~/.local/bin/nimby-wsl-rtx` utilise
`~/.local/lib/nrf-wsl/directory-read-guard.so`. Désactivation ponctuelle :
`NRF_DIRECTORY_READ_GUARD=0 ~/.local/bin/nimby-wsl-rtx`.
Le test autonome couvre fopen, fopen64, la lecture d'un fichier, l'ajout à un
fichier et ENOENT. Avec ce contournement, la même copie atteint la carte avec
lignes et trains visibles. Cela ne prouve pas que toutes les textures sont
correctes, ni la stabilité d'une session prolongée.
