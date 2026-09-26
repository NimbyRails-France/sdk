# Préparer la validation Linux dans VMware

Windows reste la référence. Les sources Linux existantes sont conservées et
raccordées aux mêmes lecteurs, modèles et tests communs. Cette refonte ne déclare
pas les hooks, l'édition d'horloge ou l'affichage Linux opérationnels.

## Construire sans dépendances Windows

Dans la VM x86-64, installer un compilateur C/C++20, CMake 3.24+, Ninja, OpenSSL
avec ses en-têtes et les dépendances de threads du système. Depuis le SDK :

```sh
cmake -S . -B build/linux-debug -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build build/linux-debug --parallel 2
ctest --test-dir build/linux-debug --output-on-failure
```

Le backend choisi par CMake doit contenir uniquement `src/platform/linux/`.
MinHook, les DLL, les outils PowerShell et les tests Windows ne sont pas compilés.
Les profils binaires Windows restent disponibles comme **données de test** pour
vérifier les lecteurs communs ; aucun appel Win32 ne doit en résulter.

## Ordre de vérification

1. Faire passer les tests autonomes : processus, broker, modules et contrats communs.
2. Vérifier la version et le SHA-256 exacts de l'exécutable Linux du jeu.
3. Tester l'observation en lecture seule et ses erreurs de processus disparu.
4. Comparer les données de scénarios équivalents, sans exiger des adresses ou PID identiques.
5. Tester le chargement et l'arrêt du mod lorsque cette intégration est qualifiée.
6. Qualifier chaque nouvelle capacité native séparément avant de l'annoncer disponible.

Le helper optionnel `NIMBY_BUILD_LINUX_COMPAT` reste désactivé par défaut. La
compilation de l'adaptateur Kotlin Linux est une vérification de sources, pas la
preuve d'un kit Linux distribué et utilisable dans le jeu.

La VM sert aux comportements Linux et aux régressions. Les comparaisons de
performance avec Windows doivent mentionner la virtualisation, le stockage
partagé et les ressources attribuées ; les chiffres bruts ne sont pas équivalents.

## Ajouter une capacité

Réutiliser d'abord les modèles communs. Implémenter l'opération système dans
`platform/linux/`, renseigner les différences vérifiées du profil et ajouter un
test Linux pour le raccordement. Les retours d'indisponibilité doivent rester
explicites tant que l'implémentation n'existe pas.
