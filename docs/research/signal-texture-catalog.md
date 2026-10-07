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

## Lecture ciblée qualifiée — 2026-10-03

Sur le même exécutable Windows 1.19, RVA `0x241780` et son helper
`0x2416b0` calculent la clé du nom du jeu de textures. La transcription
portable reproduit les multiplications 64 × 64, lectures little-endian,
blocs de 64 puis 16 octets et traitement des queues. Comparaison avec la
fonction machine pure exécutée uniquement dans un processus de test :
1 030 entrées concordantes, dont les frontières de taille. Les branches
C++ 128 bits et multiplication décomposée concordent aussi sur 10 000
entrées aléatoires. Par exemple, `sfr_bal_a_v1` donne `9a2356c6eddee8f5`.
Le chemin Linux conserve le lecteur complet historique ; son hash n'est
pas qualifié par ces preuves Windows.

Une requête nommée relit uniquement les buckets correspondants, les
identités et liens traversés, puis les noms/fichiers des jeux demandés.
Les descripteurs globaux, racines et têtes des buckets sont revérifiés.
Les ressources sans lien ne sont plus des dépendances de la capture BAL
ou de la publication d'un lot. Une chaîne corrompue dans un bucket demandé
reste nécessairement une dépendance : elle peut cacher la clé cherchée ou
un doublon. Les lectures facultatives des signaux frontières distinguent
une clé absente, autorisant le repli natif, d'une clé présente illisible,
qui reste inconnue. Aucun cache temporel n'est utilisé.

Validation en lecture seule sur la carte ouverte : les 112 jeux et leurs
240 fichiers concordent exactement entre lecture complète et lectures
ciblées, et leurs 112 clés correspondent au hash calculé. Sur 64 essais
entrelacés par mode, tous disponibles :

| Catalogue | Lectures mémoire | Octets lus | p50 | p95 | p99 |
| --- | ---: | ---: | ---: | ---: | ---: |
| Complet | 1 186 | 93 776 | 1 529,9 µs | 1 979,5 µs | 2 960,4 µs |
| Un modèle BAL | 78 | 4 338 | 97,6 µs | 138,6 µs | 185,2 µs |
| Trois modèles SFR | 162 | 10 332 | 208,0 µs | 305,7 µs | 584,3 µs |

Ces durées mesurent le lecteur de catalogue, pas la capture SDK complète.
La machine exécutait aussi les validations de développement ; avec 64
échantillons, le p99 par rang supérieur correspond au maximum. Les tests
synthétiques et le test de l'ABI de capture vérifient notamment qu'un
catalogue étranger illisible n'est pas lu, que les fichiers demandés
instables sont refusés et que l'API de catalogue complet reste stricte.
Les mesures brutes et programmes de qualification sont conservés dans
`.validation/sdk-isolation/live-catalog-profile.jsonl`,
`live-catalog-profile.cpp` et `texture-hash-research/` du workspace.
