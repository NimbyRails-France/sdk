# Validation du tutoriel — 15 septembre 2026

[Documentation](README.md) · [Tutoriel](tutorial-first-tool.md)

Validation sur Windows x64, MinGW GCC 15.2, SDK 0.6.0 / ABI 1.
Les exemples ont été compilés depuis leur copie dans le kit installé.

## Contrôles sans jeu

- 13/13 tests CTest Release réussis, dont le test optionnel du proxy.
- Les deux consommateurs autonomes compilent via `tools/package.ps1`.
- `MyFirstNimbyTool --check-sdk` renvoie 0 et affiche `SDK 0.6.0 | ABI 1`.
- Le premier exemple compile aussi avec `CMAKE_PREFIX_PATH` vers un kit déplacé
  dans un chemin contenant des espaces ; son contrôle de version fonctionne
  sans les chemins des outils de développement dans `PATH`.
- Les PID mal formés sont refusés avec la sortie 2 ; un processus non reconnu
  produit une erreur explicite et la sortie 1.

## Contrôles sur une partie ouverte

Exécutable reconnu par l'empreinte :
`fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae`.

| Programme | Résultat |
|---|---|
| `MyFirstNimbyTool <PID>` | Cinq captures réussies, cinq trains ; vitesses et positions évolutives, références de gare affichées ; sortie 0 |
| `nimby_observation_tests <PID>` | 5 trains, 379 voies, 9 gares, 27 signaux ; buffers et durée de vie des snapshots validés ; sortie 0 |
| `MyNimbyObserver <PID>` | Cinq captures réussies ; signaux, réservations et occupations observés ; sortie 0 |

Le test public a également vérifié 379 nœuds, quatre Paths disponibles,
les IDs et bornes des intervalles natifs, l'immuabilité des copies et les
27 fichiers de textures résolus. Les nombres de portions varient avec la simulation.

Le premier exemple a observé un train dont la vitesse était indisponible alors
que la position restait valide : il affiche correctement `speed=unavailable`.
Les deux bits de validité sont donc bien traités indépendamment.

Le test réel a révélé un libellé incorrect dans l'ancien exemple `observer` :
la sélection du texte reposait sur une comparaison de pointeurs de fonctions
importées. Chaque fonction est maintenant associée explicitement à son libellé.
Après recompilation depuis le paquet, cinq lignes `Reserved portions` et cinq
lignes `Occupation portions` ont été vérifiées sur une nouvelle exécution.

Ces exécutions utilisent l'API publique en lecture seule. Elles valident cette
partie et ce binaire ; elles ne garantissent ni une capture atomique ni la
compatibilité avec une autre version du jeu.

Journaux locaux générés, exclus de Git : `build/tutorial-sdk-build.log`,
`build/tutorial-package.log`, `build/tutorial-live-first.log`,
`build/tutorial-live-contract.log` et `build/tutorial-live-observer.log`.
