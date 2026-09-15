# NimbyRailsSDK dans CLion

Ouvrir **ce dossier** comme un projet indépendant. Profils **Debug** et **Release**, configurations d’exécution partagées dans `.run/`.

Les profils sont définis dans `CMakePresets.json`. Compiler avec `cmake --preset Debug`, puis `cmake --build --preset Debug`. Tester avec `ctest --preset Debug`. Remplacer Debug par Release pour la distribution.

Le SDK utilise le MinGW fourni avec CLion. Pour préparer le SDK consommé par le TCO : configurer Release, puis compiler la cible **sdk-dev-install** (ou `cmake --build --preset install-sdk`). Le résultat est installé dans `install/Release`, sans modifier le jeu. Le chargeur et le proxy font partie du SDK ; l’exemple observer reste un exemple de consommation du SDK.


Les emplacements de compilation, de distribution et les réglages personnels CLion sont exclus de Git. Adapter les chemins des outils avec `CMakeUserPresets.json` sur une autre machine. Les sources et configurations partagées restent dans le dépôt sdk.
