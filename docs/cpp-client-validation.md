> Archive technique antérieure à la 0.7 : noms et API C historiques.
> Pour la version actuelle, utiliser la [référence C++](cpp-api-reference.md) et la [migration](migration-0.7.md).

# Validation des helpers C++ — 16 septembre 2026

[Documentation](README.md) · [Tutoriel](tutorial-cpp-client.md) · [Référence](cpp-api-reference.md)

Configuration : Windows x64, MinGW GCC 15.2, SDK 0.6.0 / ABI 1.
Le header C++ utilise l'API C existante ; aucun export ou layout C n'a été modifié.

## Tests automatisés

**14/14 tests Release réussis**, dont `cpp_client_helpers` ajouté pour cette couche.
Ce test utilise un faux backend C pour contrôler les cas qui ne se produisent pas
forcément pendant une lecture réelle :

- validité indépendante de vitesse, position, état et texture ;
- IDs sur 64 bits, index et relations entre trains, voies et gares ;
- collections inconnues distinctes des collections connues vides ;
- conservation d'une capture après de nouvelles captures et destruction du client ;
- libération du snapshot natif même lorsqu'une copie échoue ;
- dernière capture conservée après échec, erreur effacée après succès ;
- démarrage, arrêt, redémarrage, changement d'intervalle pendant l'attente ;
- sérialisation des captures manuelles et automatiques, même si la capture
  prend plus longtemps que son intervalle ;
- timeout initial, indisponibilité temporaire et état de processus fermé.

Les trois exemples ont été compilés depuis leurs copies dans le paquet installé.
`MyNimbyClient --check-sdk` réussit avec SDK 0.6.0 / ABI 1.

## Partie réelle

L'exemple `auto-observer` a détecté le processus du jeu et affiché cinq captures
successives contenant cinq trains, avec des vitesses évolutives, des relations
voie/gare et des réservations. Le dernier snapshot restait lisible après l'arrêt
du worker. Le programme a terminé avec le code 0.

Le programme minimal et les extraits de recettes ont été extraits des blocs C++
du tutoriel, compilés comme consommateurs indépendants puis exécutés sur cette
partie. Les deux programmes ont terminé avec le code 0. Les recettes ont exercé
les recherches par ID, les relations, les réservations, les états et fichiers
de textures, les réglages du worker et la capture manuelle.

Le binaire du jeu possède l'empreinte supportée
`fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae`.
Ces tests valident cette configuration ; ils ne constituent pas une mesure de
performance sur les grandes cartes ni une garantie d'atomicité de la simulation.

Journaux locaux générés, exclus de Git : `build/helpers-package.log`,
`build/helpers-live-auto.log`, `build/helpers-docs-minimal-live.log` et
`build/helpers-docs-recipes-live.log`.
