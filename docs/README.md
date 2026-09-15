# Documentation de NimbyRailsSDK

Cette documentation décrit le **SDK 0.6.x, ABI 1**, pour Windows x64.
Le SDK permet à votre programme de lire les trains et le réseau d'une partie de
NIMBY Rails. L'API publique ne propose pas de commandes pour modifier la simulation.

## Commencer

1. **[Tutoriel : mon premier outil](tutorial-first-tool.md)** — préparer le SDK,
   compiler un programme C++ et afficher les trains, leur vitesse et leur voie.
2. **[Guide développeur](developing.md)** — comprendre les captures, gérer les
   ressources, intégrer le SDK dans une application et distribuer les DLL.
3. **[Référence de l'API](api-reference.md)** — fonctions, structures, unités,
   indicateurs de validité et erreurs.
4. **[Dépannage](troubleshooting.md)** — résoudre les problèmes de compilation,
   de DLL, de compatibilité et de données indisponibles.

Le tutoriel suppose que vous savez ouvrir un terminal et compiler un petit
programme C++. Il explique les notions propres au SDK au fur et à mesure.

## Choisir le bon parcours

| Votre objectif | Point de départ |
|---|---|
| Créer un programme qui lit la partie | [Premier outil](tutorial-first-tool.md) ; le kit de développement suffit |
| Utiliser le TCO déjà prêt | [Projet TCO](https://github.com/NimbyRails-France/tco) ; aucun code à écrire |
| Gérer SDK et TCO avec le Hub | [Projet Hub](https://github.com/NimbyRails-France/hub) |
| Charger le SDK au démarrage du jeu | [Installation du proxy SDL](install-drop-in.md) ; usage distinct de l'observation externe |
| Modifier ou compiler le SDK | [Guide développeur](developing.md#compiler-le-sdk) et [CLion](clion.md) |
| Exploiter les signaux | [États et textures](signal-states.md) |
| Comprendre les versions | [Version du SDK et Hub](sdk-version.md) |

## Exemples fournis

| Exemple | Ce qu'il montre |
|---|---|
| `examples/first-observer` | Vérification de la DLL, session, cinq tentatives de capture, trains et liaison voie → gare |
| `examples/observer` | Lecture des collections, états des signaux, disponibilité des réservations et occupations |

Dans un paquet installé, ils se trouvent dans
`share/NimbyRailsSDK/examples/`. La documentation est dans
`share/doc/NimbyRailsSDK/`. Les deux exemples sont des projets CMake autonomes.

Le nouvel exemple `first-observer` est présent dans les sources et les paquets
construits à partir de cette révision. Le ZIP 0.6.0 déjà publié peut ne contenir
que `observer` : le tutoriel explique comment récupérer les deux fichiers manquants.

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
- [Diagnostics et ancien chargeur](usage.md)

Les résultats d'une ancienne validation concernent sa version et sa configuration,
pas automatiquement la version actuelle.
