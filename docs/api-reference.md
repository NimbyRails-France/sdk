# API publiques

Le SDK propose deux interfaces, avec des périmètres distincts :

- [API Kotlin](kotlin-api.md) : profils de signalisation via `SignallingMod`,
  intégration native et construction prises en charge par le SDK.
- [API C++20](cpp-api-reference.md) : client d'observation, types du réseau
  et fonctions d'intégration natives, via `<nimby/client.hpp>`.

Les API C++, Kotlin et leurs versions de transport ne sont pas interchangeables.
Les interfaces sous `nimby/detail/`, `nimby.internal` et les exports internes
servent au SDK ; ils ne constituent pas une API consommateur.

L'ancienne API C publique 0.6 est retirée. Voir la [migration 0.7](migration-0.7.md)
et la [politique de compatibilité](sdk-version.md).