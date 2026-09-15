# Catalogue natif des textures — 2026-09-15

Version reconnue : SHA-256
`FFF49AC21720ABFC824C2B4F68B862727630EB0DB71CFE1F9EA8F685D0DB10AE`.
Lecture seule ; aucun asset ni état du jeu modifié.

## Disposition vérifiée

- `rules = database + 0xa80` (objet intégré).
- Table à `rules + 0x138` : buckets à +8, capacité à +16, taille à +24.
- Nœud de 0xa0 octets : hash +0, identifiant string +8, vector des fichiers
  +0x80, nœud suivant +0x98.
- Descripteur de fichier de 0x50 octets : source int32 +0, mod string +8,
  chemin relatif string +0x28, hash +0x48.
- Vector des hashes par défaut à `rules + 0x540`, indexé par type de signal.
- Racine des mods locaux : wstring à `module + 0xb77d50`.

La fonction RVA `0x4148d0` recherche le hash explicite puis le jeu par défaut
du type. Le rendu RVA `0x620140` limite le sélecteur à la taille du vector.
Les exports de recherche sont dans `reports/signal-texture-table/`.
Le repli après échec du chargement graphique reste propre au moteur.

Les sources 0/1/2 correspondent aux ressources intégrées, mods locaux et
Workshop. Le SDK résout un chemin local sous la racine correspondante et
vérifie qu'il y reste après canonicalisation. Le lecteur borne les tailles,
rejette les cycles, relit les structures et vérifie les racines de session.

## Validation

La partie observée contient 79 jeux de textures et 27 signaux. Les 27
fichiers sélectionnés ont été résolus. Le mod local `SignalisationSNCF`
fournit `signalisationsncf_textures`, hash `09c4af134204998c`, avec 16 fichiers.
Les sélecteurs 9 et 10 correspondent respectivement à
`imgs/ca/sem_bal/tex04.svg` et `imgs/ca/sem_bal/tex10.svg`.

Le TCO a validé 29 captures avec textures disponibles et décodé 8 fichiers
distincts. La capture visuelle montre les textures du mod, avec des feux
rouges, verts et jaunes. Cela valide la sélection et l'affichage des assets,
sans attribuer de noms ferroviaires aux états.

Tests automatisés : catalogue synthétique, structures invalides ou instables,
version inconnue, contrat de copie et immuabilité, disparition des textures
indisponibles ou périmées dans le TCO. `--verify-textures` permet de répéter
le test réel ; `--screenshot <fichier.png>` enregistre l'affichage.
