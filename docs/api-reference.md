# API publiques

Le SDK propose deux API Kotlin, avec des périmètres distincts :

- [API Kotlin](kotlin-api.md) : profils de signalisation via `SignallingMod`,
  intégration native et construction prises en charge par le SDK.
- [Client Kotlin/JVM](kotlin-client.md) : captures, lecture ciblée d'un train,
  horloge, textures et commandes de recette pour les outils externes.

Le client public C++ est retiré en 0.8.0. Kotlin/JVM et Kotlin/Native ne sont
pas interchangeables : un mod utilise le kit natif, un outil utilise le client JVM.
Les interfaces sous `nimby/detail/`, `nimby.internal` et les exports internes
servent au SDK ; ils ne constituent pas une API consommateur.

L'ancienne API C publique 0.6 est retirée. Voir la [migration 0.7](migration-0.7.md)
et la [politique de compatibilité](sdk-version.md).
