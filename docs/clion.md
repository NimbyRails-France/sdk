# NimbyRailsFranceSDK dans CLion

## Lancer un outil

Dans la liste des configurations en haut de CLion, sélectionner un nom puis
cliquer sur le triangle vert (Run) :

- **Outil - Observer le jeu (automatique)** : charger une partie dans NIMBY
  Rails, puis lancer cette configuration. Le processus du jeu est détecté automatiquement.
- **Verification - Client C++ (sans jeu)** et **Verification - Premier outil (sans jeu)** :
  vérifier le chargement du SDK sans lancer le jeu.
- **Outil - Premier observer (saisir PID)** et **Outil - Observer complet (saisir PID)** :
  CLion demande le PID du jeu au lancement. Il change lorsque le jeu redémarre.

Ces configurations partagées sont dans `.run/` et compilent leur outil en Debug
avant de le lancer. Elles consomment toutes le SDK installé dans `install/0.7.1/Release`.
Pour travailler dans cette fenêtre, conserver le projet CMake racine chargé ;
il suffit de sélectionner une configuration pour changer d'outil.

## Configuration CMake

Les presets Debug et Release exposent aussi `MyFirstNimbyTool`,
`MyNimbyObserver` et `MyNimbyClient` dans la même fenêtre. Garder le
`CMakeLists.txt` racine comme projet CMake. Ces outils utilisent le paquet
**installé dans `install/0.7.1/Release`**, via `find_package`, y compris en Debug :
headers, bibliothèque d'import et DLL proviennent de cette installation.
Après une modification du SDK, compiler `sdk-dev-install` en **Release**,
puis recompiler l'outil. `NimbyRailsFranceSDK_DIR` permet de choisir un autre paquet.

Sur un clone sans installation, préparer d'abord le paquet :

```powershell
cmake --preset Release -DNIMBY_BUILD_EXAMPLES=OFF
cmake --build --preset install-sdk
cmake --preset Release
cmake --preset Debug
```

Les exemples restent ouvrables comme projets autonomes.

Ouvrir **ce dossier** comme un projet indépendant. Profils **Debug** et **Release**, configurations d’exécution partagées dans `.run/`.

Les profils sont définis dans `CMakePresets.json`. Compiler avec `cmake --preset Debug`, puis `cmake --build --preset Debug`. Tester avec `ctest --preset Debug`. Remplacer Debug par Release pour la distribution.

Le SDK utilise le MinGW fourni avec CLion. Pour préparer le SDK consommé par les outils C++ : configurer Release, puis compiler la cible **sdk-dev-install** (ou `cmake --build --preset install-sdk`). Le résultat est installé dans `install/0.7.1/Release`, sans modifier le jeu. Le chargeur et le proxy font partie du SDK ; l’exemple observer reste un exemple de consommation du SDK.


Les emplacements de compilation, de distribution et les réglages personnels CLion sont exclus de Git. Adapter les chemins des outils avec `CMakeUserPresets.json` sur une autre machine. Les sources et configurations partagées restent dans le dépôt sdk.

La 0.7 utilise NimbyRailsFranceSDK. Le TCO existant doit être migré séparément.
