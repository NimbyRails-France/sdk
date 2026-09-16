# Version du SDK

La version publiée est **NimbyRailsFranceSDK 0.7.1**, avec lecture et modification
expérimentale de la [date de simulation](simulation-clock.md). Ses headers
requièrent une DLL 0.7.1 ou ultérieure compatible (pont interne 2).

Elle propose uniquement l'API C++20 publique. Le pont interne est en version 2.

```cpp
#include <nimby/client.hpp>
auto version = nimby::getVersion(); // Sans ouvrir de processus de jeu.
```

Les champs sont `major`, `minor`, `patch` et `abi` (version du pont interne).
Une DLL incompatible déclenche `nimby::Exception`. Utiliser les headers,
bibliothèques d'import et DLL du même kit.

Les anciens TCO utilisant NimbyRailsSDK 0.6.x / ABI 1 doivent être migrés.
Le TCO 0.5.1 utilise le pont 2 et accepte la DLL 0.7.1 ; le correctif des
textures ne nécessite pas de reconstruire son interface.

Voir la [migration](migration-0.7.md) et la [référence C++](cpp-api-reference.md).
