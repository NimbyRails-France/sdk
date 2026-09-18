# Documentation de NimbyRailsFranceSDK

Cette documentation décrit **NimbyRailsFranceSDK 0.7, API C++20**, pour Windows x64.
Le SDK permet à votre programme de lire les trains et le réseau d'une partie de
NIMBY Rails. Des commandes expérimentales distinctes gèrent les textures et la
date de simulation ; voir leurs contrats avant utilisation.

## Commencer

1. **[Tutoriel C++ : captures automatiques et getters](tutorial-cpp-client.md)** —
   démarrer à 4 Hz, obtenir les trains et chercher un objet par ID.
2. **[Guide développeur](developing.md)** — comprendre les captures, gérer les
   ressources, intégrer le SDK dans une application et distribuer les DLL.
3. **[Référence C++ et types de retour](cpp-api-reference.md)** — objets, getters,
   optional, collections, durées de vie et erreurs.
4. **[Dépannage](troubleshooting.md)** — résoudre les problèmes de compilation,
   de DLL, de compatibilité et de données indisponibles.

Le tutoriel suppose que vous savez ouvrir un terminal et compiler un petit
programme C++. Il explique les notions propres au SDK au fur et à mesure.

Pour les captures manuelles : [premier outil C++](tutorial-first-tool.md).
Pour écrire votre propre DLL : [créer un mod, fichier par fichier](tutorial-create-mod.md).
L'ancienne API C est retirée ; voir la [migration 0.7](migration-0.7.md).

## Choisir le bon parcours

| Votre objectif | Point de départ |
|---|---|
| Créer un programme qui lit la partie | [Client C++](tutorial-cpp-client.md) ; le kit de développement suffit |
| Écrire un mod C++ chargé par NRF Loader | [Créer son mod depuis zéro](tutorial-create-mod.md) ; adaptateur du SDK de développement requis |
| Utiliser le TCO déjà prêt | [Projet TCO](https://github.com/NimbyRails-France/tco) ; aucun code à écrire |
| Gérer SDK et TCO avec le Hub | [Projet Hub](https://github.com/NimbyRails-France/hub) |
| Charger le SDK au démarrage du jeu | [Installation du proxy SDL](install-drop-in.md) ; usage distinct de l'observation externe |
| Modifier ou compiler le SDK | [Guide développeur](developing.md#compiler-le-sdk) et [CLion](clion.md) |
| Exploiter les signaux | [États et textures](signal-states.md) |
| Comprendre les versions | [Version du SDK et Hub](sdk-version.md) |

## Exemples fournis

| Exemple | Ce qu'il montre |
|---|---|
| `examples/auto-observer` | Client C++, détection du jeu, captures automatiques à 4 Hz, getters, recherches et âge des données |
| `examples/first-observer` | Vérification de la DLL, session, cinq tentatives de capture, trains et liaison voie → gare |
| `examples/observer` | Lecture des collections, états des signaux, disponibilité des réservations et occupations |

Dans un paquet installé, ils se trouvent dans
`share/NimbyRailsFranceSDK/examples/`. La documentation est dans
`share/doc/NimbyRailsFranceSDK/`. Les trois exemples sont des projets CMake autonomes.

Le kit 0.7 fournit les trois exemples C++ et tous les headers nécessaires.
Voir la [migration](migration-0.7.md) pour les anciens consommateurs.

## Ce qui est expérimental

Le SDK accepte le binaire du jeu dont le SHA-256 est
`fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae`.
Une mise à jour du jeu peut nécessiter un nouvel adaptateur.

Les captures sont immuables après leur création, mais les lectures du jeu ne
forment pas un instant simultané de la simulation. Un champ inconnu doit rester
affiché comme inconnu. Les réservations natives, les Paths et les textures des
signaux ont des significations différentes : voir la référence avant de les utiliser.

## Recherche et validations

Les rapports sous `research/` décrivent les observations et leurs limites ; leurs
offsets mémoire ne font pas partie de l'API à utiliser dans votre programme.

- [Lecture du réseau](research/network.md)
- [Paths et graphe partiel](research/train-paths.md)
- [Réservations et occupation](research/reservations.md)
- [Catalogue de textures](research/signal-texture-catalog.md)
- [Validation historique du paquet 0.2.0](sdk-validation.md)
- [Validation du tutoriel sur le SDK 0.6.0 et une partie réelle](tutorial-validation.md)
- [Validation des helpers C++ et des exemples du tutoriel](cpp-client-validation.md)
- [Diagnostics et ancien chargeur](usage.md)

Les résultats d'une ancienne validation concernent sa version et sa configuration,
pas automatiquement la version actuelle.
