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
Sous Linux, le SDK doit être préchargé dans le jeu pour autoriser la lecture
par le même utilisateur. Les hooks Linux complets restent en migration.

Les exemples C++ externes ont été retirés. Les en-têtes et tests C/C++ encore
présents servent aux composants natifs internes ; leur présence ne constitue
pas un parcours de développement client à maintenir.
