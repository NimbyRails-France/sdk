# Langue des interfaces de mods — Windows 1.19

Binaire qualifié : SHA-256
`fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae`.

`FUN_1402d8a50` lit le pointeur global à RVA `0xb7b610`. S'il est non nul,
il copie le premier `std::string` MSVC de cet objet via `FUN_140019ca0`.
Sinon il copie le code de trois octets à RVA `0xa5ebd8` (anglais).
`FUN_1402d88b0` remplace cet objet après sélection d'une langue ;
`FUN_1402db200` construit la copie du catalogue actif.

La préférence `language_code_iso_639_2` lue dans `FUN_1407272d0` est une entrée
de configuration ; le lecteur utilise le catalogue effectif, pas la langue de
Windows ni un chemin de configuration présumé. Relecture du pointeur et de
l'en-tête ; taille bornée et caractères de code langue validés. Aucune fonction
du jeu n'est appelée. Une lecture indisponible choisit le repli du mod.

Production : `include/platform/windows/game_language.h`, appelé uniquement par
le pont UI après validation du binaire. Le reste du catalogue et du rendu reste
commun. Les dispositions des autres versions du jeu ne sont pas supposées.

Validation locale du 28 septembre 2026 : lecture seule du jeu déjà ouvert,
code effectif `eng`. Les fixtures couvrent la langue active, l'anglais natif,
le remplacement du pointeur, les tailles et caractères invalides. Aucun
changement de langue ni contrôle visuel n'a été effectué dans le jeu.

Les catalogues JSON sont lus une fois depuis le dossier de chaque DLL, validés
par l'adaptateur puis copiés dans le pont résident. Chaque panneau et fournisseur
possède son catalogue. Une action initiale utilise celui du panneau ; les
contrôles développés utilisent celui du fournisseur. Le passage de layout fige
les textes traduits pour le passage interactif. Les identifiants, epochs, valeurs
persistées et brouillons numériques ne dépendent pas de la traduction.
