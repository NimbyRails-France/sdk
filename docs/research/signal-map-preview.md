# Aperçu de signaux sur la carte — Windows

Le mod choisit toutes les positions. Le SDK fournit un dessin temporaire via
`ToolContext.showSignalPreview(request, positions)` et `clearSignalPreview()`.
L'API ne choisit ni espacement ni branche et n'appelle aucune construction.

## Contrat et cycle de vie

Le pont copie au maximum 64 positions (ID de voie, fraction strictement comprise
entre 0 et 1, direction -1 ou +1). Une demande est liée au fournisseur, à son
service déclaré, au panneau du signal source et à la génération observée.
Les données inconnues, périmées ou d'une autre partie sont refusées.
La dernière publication remplace l'aperçu actif. Effacer ne retire que celui du
fournisseur appelant. Arrêt, déchargement et changement de contexte l'invalident.
Le renouvellement doit avoir lieu avant expiration des deux secondes.

La saisie d'un champ numérique masque immédiatement l'ancien aperçu et invalide
les anciens boutons. Le panneau source doit être édité pour dessiner : sa présence
est confirmée par le passage UI, valable au maximum 250 ms sans nouveau passage.
Une demande acceptée ne garantit pas que chaque position est dans le cadrage.
Les niveaux explicitement masqués sont exclus. Le cadrage reste géré par le
rendu natif ; une voie n'a pas besoin de contenir un signal déjà construit.

## Liaison native qualifiée

Binaire Windows reconnu par SHA-256 :
`fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae`.
Adresses suivantes relatives à la base du module, aucun offset dans le mod :

- `0x6236d0` : rendu de la vue complète. Le hook appelle d'abord le trampoline
  avec ses sept arguments inchangés, puis soumet toutes les positions.
  La référence de DB est dans le contexte de rendu à `+0x428`, les règles à
  `+0x408`, les routes à `+0x420` et le calendrier à `+0x890`.
- `0x6211d0` : rendu d'un signal temporaire, dix arguments. Utilisé par
  `NewSignalEditor::draw` (`0x7833e0`) avec le mode `0x0a`.
- `0x7830a0` : préparation du signal temporaire natif, ID nul, octet blueprint
  actif, voie/fraction/direction et orientation liée à la voie.
- `0x61b580` : position et tangente du signal à partir de la géométrie de voie.
- `0x620f40` : ancien point d'accroche, abandonné. Sa liste par couche ne contient
  que les voies ayant déjà un signal : le test de vecteur à `0x61efc9` suivi de
  l'insertion à `0x61efd9` dans `0x61e7d0` le démontre. Ce n'est donc pas une
  liste générale de voies visibles et elle excluait des positions planifiées.
- `0x62414a` : recherche dans l'arbre des niveaux masqués (`options +0x48`),
  réutilisée pour respecter le masquage sans dépendre des signaux existants.

`BorrowedFrame` résout les identités complètes dans la DB du passage de rendu,
exclut les niveaux masqués et copie le modèle source dans un tampon local de
`0xc8` octets. Cette DB ne doit pas être supposée identique à la DB vive.
Les chaînes et filtres sont empruntés en lecture seule.
L'appel natif consomme le tampon synchroniquement ; aucun constructeur,
destructeur ou pointeur de mod n'est transmis au jeu. Aucun octet des pools n'est
écrit. Les positions sont dessinées avec la géométrie et la texture natives.
La racine, la DB vive et la simulation sont aussi contrôlées pour empêcher la
réutilisation d'une publication après remplacement de partie.

Le nouveau hook est dans le pont UI résident Windows. Le SHA du jeu et les
octets d'entrée sont vérifiés avant installation. Le registre, les validations
de publication et le cycle de vie restent dans le code commun.

## Validation

`native_signal_preview` couvre copie sans écriture, direction, orientation de
voie, modèle, identités périmées, niveaux masqués et arbre invalide, paramètres invalides,
copie des publications, expiration, saisie et arrêt/changement de contexte.
La régression couvre trois aperçus sur trois voies, dont deux sans signal
existant, ainsi que 64 positions distinctes. Le rapport utilisateur avait trois
positions calculées (500, 1000, 1500 m) mais un seul aperçu dessiné.
`signal_ui_dll_c_exports` contrôle le nouvel export et le refus hors du jeu.
`native_signal_ui_text` contrôle également la signature du hook quand
`NIMBY_TEST_GAME_EXE` est renseigné ; le binaire est mappé sans le démarrer.
Les tests Kotlin de Signal Placement couvrent transmission, recalcul, masquage,
renouvellement, erreurs et retrait avant construction.
Le journal SDK indique `requested` (positions publiées) et `submitted` (appels
au rendu), uniquement lorsque ces compteurs ou la source changent. Ce dernier
compteur ne prouve pas que les pixels sont visibles dans le cadrage courant.

Restent à qualifier visuellement dans le jeu : apparence, zoom, voies courbes,
deux sens, couches superposées et fermeture du panneau. Les tests hors jeu ne
prouvent pas ces propriétés du rendu. Aucun test sur le VPS de production.
