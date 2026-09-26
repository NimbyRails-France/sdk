# Créer un client Kotlin

Les nouveaux outils utilisent Kotlin/JVM avec le module `kotlin-client` et
JDK 21. L'exemple complet est dans `examples/kotlin-observer` ; le TCO Compose
utilise ce même client. Aucun code C++ n'est requis dans l'application cliente.

```kotlin
NimbyClient.open(Path.of(library), pid).use { client ->
    val snapshot = client.capture()
    println(snapshot.trains.size)
}
```

Fournir la bibliothèque native du SDK adaptée au système et le PID du jeu.
Les captures sont des copies immuables. Les données indisponibles restent
optionnelles ; une absence ne signifie pas une voie libre ou une vitesse nulle.
Cette branche est validée et distribuée pour Windows x64. Le développement
et la publication Linux sont suspendus ; la séparation des plateformes reste en place.

Le client public C++ et ses exemples ont été retirés. Les en-têtes et tests C/C++ encore
présents servent aux composants natifs internes ; leur présence ne constitue
pas un parcours de développement client à maintenir.

`client.readTrain(trainId)` fournit une [lecture ciblée de conduite](driving-observation.md)
sans capture du réseau complet. `null` signifie indisponible, pas un train arrêté.
`capture()`, `readTrain()`, les commandes et `close()` sont sérialisés sur la
connexion. L'application possède sa cadence ; il n'y a pas de worker caché,
de dernière capture mise en cache ni de relance automatique d'une commande.
