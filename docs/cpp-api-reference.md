# Ancien client C++ — retiré en 0.8.0

`nimby/client.hpp` et `nimby::Client` ne font plus partie du SDK.
Les applications utilisent le [client Kotlin/JVM](kotlin-client.md).
Les mods implémentent [SignallingMod en Kotlin/Native](kotlin-mods.md).

La capture, l'horloge, les textures et les commandes de recette sont accessibles
depuis `NimbyClient`. La [lecture ciblée d'un train](driving-observation.md)
retourne des valeurs Kotlin possédées par l'appelant.

Le noyau et le pont précompilé contiennent encore des structures natives et une
session privée pour leurs propres lectures. Ce code appartient à l'implémentation
du SDK ; il ne constitue pas une API C++ alternative à maintenir pour les clients.
Voir le [guide des mainteneurs](sdk-maintainers.md).
