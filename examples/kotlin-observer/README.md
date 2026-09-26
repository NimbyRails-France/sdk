# Observateur Kotlin

Exemple JVM sans C++, avec JDK 21. Le client SDK se trouve par défaut dans
`../../kotlin-client` ; utiliser `-PnrfSdkClientDir=C:/dev/nrf/sdk/kotlin-client` pour
un autre emplacement.

```powershell
.\gradlew.bat run --args="--help"
.\gradlew.bat run --args="C:/dev/nrf/sdk/build/clion-Release/NimbyRailsFranceSDK.dll 12345"
```

Remplacer `12345` par le PID du jeu local choisi. L'exemple lit une capture,
puis une observation ciblée du premier train s'il existe, et ferme sa session.
Une observation manquante ou une vitesse inconnue reste `null`.

La version 0.8.0 est préparée pour Windows x64. Le développement et la publication
Linux sont suspendus. Le client public C++ est retiré ; aucun compilateur C++
n'est nécessaire pour cet exemple Kotlin.
