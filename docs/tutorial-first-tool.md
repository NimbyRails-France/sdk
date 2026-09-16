# Premier outil C++ — NimbyRailsFranceSDK 0.7

L'exemple `examples/first-observer` vérifie la version puis effectue cinq
captures manuelles. Il affiche les trains, leur état, leur localisation, les
voyageurs et quelques entrées du plan de ligne. Il utilise uniquement
`<nimby/client.hpp>`, comme l'observer automatique.

## 1. Préparer le kit

Depuis la racine du SDK :

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools/package.ps1
```

Le kit est dans `dist/NimbyRailsFranceSDK-0.7.0/`. Copier les deux fichiers
de `share/NimbyRailsFranceSDK/examples/first-observer/` dans un nouveau dossier.
Ne pas mélanger ce kit avec les DLL ou headers 0.6.

## 2. Configurer

Prévoir Windows x64, CMake 3.24+, Ninja et MinGW avec C++20. Le kit utilise le
MinGW fourni avec CLion. Le projet appelle
`find_package(NimbyRailsFranceSDK 0.7.0 CONFIG REQUIRED)` et lie
`NimbyRailsFranceSDK::SDK`.

## 3. Compiler

Avec les outils dans `PATH`, depuis le dossier de l'exemple :

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=C:/SDK/NimbyRailsFranceSDK-0.7.0
cmake --build build
./build/MyFirstNimbyTool.exe --check-sdk
```

Adapter le chemin du kit. Le contrôle affiche `NimbyRailsFranceSDK 0.7.0`
sans ouvrir le jeu. Les DLL sont copiées automatiquement près de l'exécutable.
Dans CLion, ouvrir ce dossier et renseigner le même `CMAKE_PREFIX_PATH`.
Dans le dépôt SDK, utiliser directement les [configurations partagées](clion.md).

## 4. Observer la partie

Charger une partie, puis :

```powershell
Get-Process -Name NIMBYRails | Select-Object Id, ProcessName
./build/MyFirstNimbyTool.exe 12345
```

Remplacer 12345 par le PID obtenu. Les IDs des trains et voies sont distincts
du PID Windows et doivent conserver leurs 64 bits.

Le code utilise `nimby::getVersion()`, `Client::connect(pid)` et
`client.capture()`. Le client gère sa session et les captures sont conservées
par `shared_ptr`. Les erreurs sont des `nimby::Exception` ; le programme
réessaie seulement si `e.code() == nimby::ErrorCode::DataUnavailable`.

Les valeurs inconnues restent optionnelles. Une vitesse d'affichage par défaut
n'est pas une mesure ; la position vient des données natives. Un plan de ligne
n'est pas une promesse sur les prochains arrêts servis. Les temps sont ceux
du jeu. Voir [la référence C++](cpp-api-reference.md).

Codes de sortie : 0 succès, 1 erreur du SDK, 2 arguments invalides, 3 aucune
capture disponible. Pour les captures automatiques, suivre le
[tutoriel client C++](tutorial-cpp-client.md).
