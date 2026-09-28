# Panneau d'extensions défilant — Windows 1.19

Analyse locale du binaire reconnu le 27 septembre 2026, SHA-256
`fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae`.
Projet Ghidra ouvert avec `-readOnly -noanalysis`, sans accès au VPS.

Le groupe défilant est une primitive de `LayoutEmit` / `NuklearEmit` :

| Slot | Layout (RVA) | Interaction (RVA) | Contrat Windows x64 |
| --- | --- | --- | --- |
| `0x30` | `0x55c950` | `0x55d620` | `child(parent, const char* key, uint32_t flags)` |
| `0x38` | `0x55c980` | `0x55dc40` | `endGroup(parent)` |
| `0x08` | `0x55c6f0` | `0x55d2c0` | `beginBox(child)` |
| `0x18` | `0x55c770` | `0x55d430` | `endBox(child)` |
| `0x138` | `0x55ce50` | `0x562a10` | `int32 number(child,label,min,value,max,step)` |

Les deux fonctions de début terminent par l'appel du slot `0x20`, qui retourne
un émetteur enfant appartenant au parent. Le code natif `0x7094b0` confirme
l'utilisation : groupe `##train_sched_list` avec flags `0x800`, contenu émis sur
l'enfant, fermeture sur le parent. Les tables de l'enfant sont de nouveau
validées avant tout appel. Aucun pointeur enfant n'est conservé entre deux passes.
Le groupe SDK utilise désormais les flags `0` pour permettre les deux axes.
Dans le fork du jeu, `0x800` supprime la barre horizontale : `0x509f20` teste
explicitement ce bit avant d'ajouter sa hauteur. Ce bit ne se déduit pas de
l'enum [Nuklear amont](https://github.com/Immediate-Mode-UI/Nuklear/blob/master/nuklear.h).

La hauteur du prochain élément utilise l'option à `Declare+0x30` et sa valeur
float à `+0x34`. Le SDK limite le groupe à 320 unités d'interface et conserve
le même budget issu de la copie du panneau pour layout et interaction. Le jeu
gère le clip, la molette et la barre de défilement. La fermeture est garantie
même si une validation de l'enfant échoue.

## Correction du cadre vide (27 septembre 2026)

Le premier montage créait le conteneur défilant sans ouvrir de boîte racine
dans l'enfant. `0x55c690` calcule uniquement l'arbre partant de l'élément zéro :
les autres contrôles étaient orphelins, avec des rectangles nuls. Les anciens
tests simulaient `scroll` au niveau du présentateur et ne détectaient pas cette
erreur de composition native.

Le montage ouvre maintenant une colonne (`Declare+0x20 = true`, `+0x24 = 3`)
sur l'enfant, dessine ses contrôles, ferme la colonne, puis ferme le conteneur
sur le parent. Chaque contrôle reçoit l'alignement horizontal `0xa0`. La hauteur
de la racine reste automatique ; seule la hauteur du conteneur est plafonnée.
Les destructeurs ferment les deux niveaux, dans cet ordre, même en cas d'exception.

Une largeur provisoire non nulle est déclarée lors de la première passe. Dans
la passe interactive, le groupe a sa largeur calculée à `parent+0x1f0`. L'enfant
possède une copie indépendante du layout à `child+0xb0` (`0x55d460`, `0x55c340`).
Le SDK ajuste la largeur de sa racine (`items[0]+0x1c`), puis appelle le calcul
natif `0x55c690` avant toute consommation de ligne. Une réserve de 32 unités
d'interface laisse de la place au défilement et aux marges. Le facteur d'échelle
provient du layout à `+0xb0`. Les pointeurs et la nature de la racine sont contrôlés.
Les offsets restent dans le profil Windows du binaire reconnu.

Vérification locale sans interaction avec la partie : les deux fonctions de
calcul natives, chargées dans un processus de diagnostic sans démarrer le jeu,
reproduisent les rectangles nuls de l'ancien arbre et valident une colonne de
3, 18 et 64 contrôles aux largeurs 220, 400 et 680 pixels. Les hauteurs complètes
sont respectivement 51, 411 et 1515 pixels avec un séparateur de 3 pixels.
Le test `native_signal_ui_scroll` appelle le véritable adaptateur `SignalUi`
via des tables synthétiques Windows : ordre des deux passes, largeur et échelle,
redimensionnement, enfant invalide, dimensions invalides, fermeture sur exception.
Ces vérifications ne constituent pas une validation visuelle en jeu de la molette
ou du glissement de la barre.

Le premier séparateur utilisait un bouton vide désactivé de 3 unités. Il est
remplacé par une règle plate d'une unité, avec 8 unités de marge de chaque côté
verticalement ; les détails de composition sont décrits ci-dessous.
Le slot `0x140` n'est **pas** un séparateur :
il reçoit un identifiant de surface de rendu et ne doit pas être appelé avec
une couleur ou un identifiant arbitraire.

Les tests logiciels couvrent les fonctions de table altérées, la conservation
du groupe, du séparateur et des lignes lors du retrait d'un fournisseur entre
les deux passes, et la désactivation des clics devenus périmés. Ils ne remplacent
pas une qualification visuelle du défilement dans le jeu.

## Largeur des textes et champ numérique

La largeur minimale du contenu utilise le rappel de mesure de la police native,
selon l'appel de `0x561190` : style `NuklearEmit+0x1e0`, police `style+0xf0`,
handle `font+0`, hauteur `+8`, fonction `+0x10` avec la signature
`float(handle, float height, const char* utf8, int byteCount)`. La plus grande
largeur des libellés, avec marge pour les contrôles, devient la largeur du contenu
si elle dépasse la largeur visible. Le conteneur conserve sa largeur et sa hauteur.

Le contrôle entier du slot `0x138` accepte un nom, un minimum, la valeur courante,
un maximum et un pas. Il retourne la valeur modifiée ; le jeu gère son édition
au clavier. `0x562a10` transmet un variant de type entier à `0x515120`.
Le SDK utilise exclusivement des entiers copiés, jamais une `std::string` du jeu.
Les limites viennent de `ToolNumberInput` déclaré par le mod.

Les événements V2 ajoutent une valeur optionnelle sans changer les structures V1.
Une édition bloque les boutons du panneau jusqu'à sa republication par le worker.
La dernière valeur d'un même champ remplace l'événement encore en attente ; les
autres champs en attente survivent à l'accusé de réception d'un premier champ.
Le groupe natif est identifié par signal, session et fournisseur : le focus
d'une saisie ne doit pas être transféré au prochain signal sélectionné.

## Présentation et édition du texte (28 septembre 2026)

Le retour en jeu a confirmé l'affichage et la pose, mais aussi deux défauts :
un message dessiné comme un bouton grisé élargissait tout le contenu, et la
valeur du contrôle entier se retrouvait hors du viewport. Les journaux de la
session montrent un calcul puis une pose réussis ; ils ne prouvent aucun
aperçu graphique sur la carte, qui n'est pas implémenté par ce panneau.

- Les titres et les libellés utilisent le slot `0xa8` (`0x55ca70` / `0x55f570`),
  signature `void(emitter, const char*, uint32_t alignment)`. `0x11` aligne à
  gauche et au milieu, confirmé par `0x50ad90`.
- Les résultats utilisent le texte avec retour à la ligne `0xb8`
  (`0x55cb30` / `0x55f640`), `void(emitter, float width, const char*)`.
  Sa première passe mesure elle-même la hauteur pour une colonne de 260 unités.
  Cette largeur ne devient plus celle de tous les contrôles du panneau.
- Les marges natives sont l'option `Declare+0x38` puis quatre floats à `+0x3c`
  dans l'ordre gauche, haut, droite, bas, confirmés par `0x55c4b0`.
- Une règle réserve un espace au slot `0xd0` (`0x55cc10` / `0x55f930`).
  En interaction elle consomme exactement un rectangle avec `0x55d140`, puis
  `0x50abd0` le convertit en coordonnées écran et vérifie sa visibilité.
  `0x4fc370(commandBuffer, rect*, rounding, rgba)` émet le rectangle plat avec
  la couleur de bordure du groupe (`context+0x2420`). `0x55ba40` réinitialise
  ensuite les options de l'émetteur. Le clip et le thème restent natifs.
- Les dimensions des barres (`context+0x248c`, deux floats) sont limitées à
  10 unités multipliées par l'échelle, uniquement pendant ce groupe. Une garde
  restaure les deux valeurs après `endGroup`, y compris sur exception.

Le champ numérique réserve désormais un libellé et un véritable éditeur texte
de 180 × 28 unités. `0x5137d0(context, flags, char*, int* length, capacity, filter)`
édite un tampon SDK de 32 octets, avec sélection, presse-papiers et touche Entrée
(`flags=0x264`, lecture seule `|1`). Cette signature est confirmée par le wrapper
du jeu `0x55fe90` ; aucune `std::string` MSVC ne traverse l'adaptateur MinGW.
`NumberInputDraft` conserve la chaîne vide ou partielle entre les frames et les
publications du worker. Seul un entier complet dans les limites du mod produit
un événement V2. Un brouillon invalide invalide aussi les actions déjà en attente.
La présentation retient uniquement les brouillons visibles, identifiés par
panneau, signal, session, fournisseur, époque et champ (pas par révision).

Les fixtures contrôlent les tampons vides, remplacement, bornes, dépassement
entier, valeur publiée ancienne, règle plate et restauration des barres.
La saisie et le rendu réels doivent encore être qualifiés après remplacement
de la DLL et redémarrage du jeu.

Le rendu d'un éditeur Nuklear ne suffit pas à recevoir du texte sous Windows.
`0x731d80` utilise le compteur `owner+0x64e0` pour appeler `SDL_StartTextInput`,
actualiser la zone IME, puis `SDL_StopTextInput` lorsque le champ perd le focus.
L'ancien contrôle de propriété ne rafraîchissait pas ce compteur. Le nouveau
champ reproduit le bail du wrapper `0x55fe90` tant que le bit actif est présent :
zone de saisie à `owner+0x64cc` et compteur QPC à `+0x64e0`. L'horloge QPC est
fournie par le pont Windows, sans dépendance Windows dans le modèle commun.
Le SDK ne lance pas SDL lui-même et ne force pas les drapeaux d'activation :
la boucle normale du jeu garde la gestion de l'entrée de texte et de sa fermeture.

## Correction du clavier bloqué malgré le curseur (28 septembre 2026)

Le premier champ texte envoyait `0x64` : sélection, presse-papiers, Entrée.
Il manquait `ALWAYS_INSERT_MODE` (`0x200`). Le bail SDL était nécessaire mais
insuffisant : `0x5137d0` initialise le mode d'édition à zéro (`VIEW`), et
`0x5119a0` n'active l'insertion qu'avec ce bit. Le champ pouvait donc prendre
le focus et dessiner le curseur, tout en ignorant les caractères et l'effacement.
`0x50fc90` confirme que Retour arrière et Suppr exigent un mode différent de zéro.
Le SDK transmet maintenant `0x264`. Le drapeau lecture seule reste prioritaire.
Les types de champs [Nuklear amont](https://github.com/Immediate-Mode-UI/Nuklear/blob/master/src/nuklear.h)
incluent aussi ce bit ; les valeurs et le comportement ont été vérifiés dans
le binaire précis du jeu, sans supposer une compatibilité de ses structures.

Le nouveau test `native_signal_ui_text` charge ce binaire avec
`DONT_RESOLVE_DLL_REFERENCES`, après contrôle de son identité. Il appelle le
véritable `0x5137d0` avec un contexte et des événements possédés par le test.
Il n'exécute ni point d'entrée ni boucle du jeu, ne crée aucune fenêtre et
n'envoie aucune touche au bureau. Il reproduit le défaut avec `0x64`, puis
vérifie avec `0x264` : frappe, Retour arrière jusqu'au champ vide, conservation
du vide à l'image suivante, remplacement de sélection, Suppr et lecture seule.
Le test d'adaptateur `native_signal_ui_scroll` vérifie séparément les options
réellement transmises par `SignalUi`.

Pour exécuter cette régression locale, définir `NIMBY_TEST_GAME_EXE` avec le
chemin complet de l'exécutable reconnu, puis lancer
`ctest --test-dir build/hub-native -R native_signal_ui_text --output-on-failure`.
Sans cette variable, le test est explicitement ignoré (code 77), notamment en
CI où le binaire propriétaire n'est pas disponible. Cette preuve d'édition
native ne remplace pas la vérification de toute la chaîne SDL dans une partie.
