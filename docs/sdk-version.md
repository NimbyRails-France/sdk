# Version du SDK

La version publiée est **NimbyRailsFranceSDK 0.7.0**.
Elle propose uniquement l'API C++20 publique. Le pont interne est en version 2.

```cpp
#include <nimby/client.hpp>
auto version = nimby::getVersion(); // Sans ouvrir de processus de jeu.
```

Les champs sont `major`, `minor`, `patch` et `abi` (version du pont interne).
Une DLL incompatible déclenche `nimby::Exception`. Utiliser les headers,
bibliothèques d'import et DLL du même kit.

Le TCO actuel attend NimbyRailsSDK 0.6.x / ABI 1 et ses anciens exports C :
il doit être migré, pas simplement recevoir la nouvelle DLL. Le catalogue Hub
devra aussi référencer le nouveau nom et la nouvelle compatibilité.

Voir la [migration](migration-0.7.md) et la [référence C++](cpp-api-reference.md).
