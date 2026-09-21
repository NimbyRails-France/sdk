# Observateur Kotlin

Exemple JVM sans C++, avec JDK 21. Le client SDK se trouve par défaut dans
`../../kotlin-client` ; utiliser `-PnrfSdkClientDir=/chemin/kotlin-client` pour
un autre emplacement.

```sh
./gradlew run --args="--help"
./gradlew run --args="/chemin/NimbyRailsFranceSDK.so 12345"
```

Sous Windows, utiliser `gradlew.bat` et la DLL du SDK. Le PID désigne le jeu
local en cours d'exécution. Linux nécessite le serveur de lecture du SDK
chargé dans le jeu. Cet exemple lit une capture et ferme toujours sa session.
