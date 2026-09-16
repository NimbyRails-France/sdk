# Validation de la version 0.7.1

Validation locale du 16 septembre 2026, NIMBY Rails 1.19.10,
profil SHA-256 `fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae`.

- Compilation Release complete et 14 tests CTest reussis.
- Les trois exemples observer, first-observer et auto-observer sont compiles
  hors du code source du SDK, depuis le paquet installe ; `--check-sdk` reussit.
- Grande partie : 16 312 signaux, 16 312 selecteurs par defaut et chemins valides.
  Les six fichiers distincts sont decodes par Qt avec les plugins du TCO installe.
- Brest : les 24 troncons sont regroupes en six quais, occupations et reservations
  dedoublonnees. Le detail par troncon reste accessible.
- Chargement d'une autre sauvegarde dans le meme processus de jeu : les captures
  deviennent indisponibles au menu, puis reprennent sur 27 signaux, tous avec
  selecteur explicite et fichier valide. Le TCO retrouve cinq trains et neuf gares
  sans redemarrage.
- Sur cette seconde partie, 40 captures consecutives sans erreur donnent
  17 changements de selecteur/fichier. Le signal `0x8000000110003` alterne
  notamment entre 9 (`tex04.svg`) et 10 (`tex10.svg`). Le selecteur explicite
  n'est donc pas remplace par le zero par defaut. Les images ne sont pas figees.
- Le meme essai compile contre le paquet de release donne 40 captures sans
  erreur et 19 changements de texture.
- Le TCO 0.5.1 pouvait rendre les textures de mods presque invisibles au dezoom
  et regrouper une balise avec un signal au meme point. La correction TCO 0.5.2
  retire les marges transparentes et espace les petits groupes avec des traits
  vers leur voie. Quatre tests TCO passent, dont les textures et infobulles
  distinctes de deux signaux superposes. Sur la partie de test en cours,
  28 captures du TCO corrigé donnent 28 jeux de textures complets et sept
  fichiers distincts decodes ; le rendu est aussi controle dans la fenetre.

La gestion experimentale de la date et l'intervention native des trains ont
leurs preuves et limites dans [simulation-clock.md](simulation-clock.md) et
[research/train-intervention.md](research/train-intervention.md).
Ces essais ne constituent pas une validation de toutes les sauvegardes,
de tous les mods ni du multijoueur.
