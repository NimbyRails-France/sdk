# Maintenir le SDK natif dans CLion

Les projets clients et les exemples utilisent Kotlin et IntelliJ/Gradle.
CLion sert au code natif interne du SDK, à ses hooks et à ses tests.

```powershell
cmake --preset Debug
cmake --build --preset Debug
ctest --preset Debug
```

Le SDK utilise le MinGW fourni avec CLion, sans Qt. Adapter les chemins via
`CMakeUserPresets.json` sur une autre machine. Les presets Release et la cible
`sdk-dev-install` préparent les composants internes utilisés pour fabriquer
le kit Kotlin. Les anciens exemples observateurs C++ et leurs configurations
d'exécution ont été retirés.

Pour observer le jeu depuis une application, suivre [le client Kotlin](kotlin-client.md).
Pour les mods, suivre [le guide Kotlin/Native](kotlin-mods.md).
