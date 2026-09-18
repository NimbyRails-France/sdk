# NimbyRailsFranceSDK 0.7.2

La 0.7.2 ajoute les mods C++ chargés par NRF Loader, l'adaptateur
`NimbyRailsFranceSDK::Mod`, les commandes de textures par chemin et
[le tutoriel pour créer son propre mod](docs/tutorial-create-mod.md).
L'ABI C reste en version 2. Utiliser le Hub 0.2.3 pour gérer ces mods.

La 0.7.1 ajoute la [date de simulation et sa modification expérimentale](docs/simulation-clock.md),
une interface graphique pour auto-observer, les quais regroupés par gare et
la texture native par défaut des signaux sans état explicite.

SDK C++20 pour observer NIMBY Rails sous Windows x64 : trains, gares, quais,
horaires, voyageurs, voies, signaux, réservations et occupations.

La 0.7.0 est une rupture avec NimbyRailsSDK 0.6.x. L'API publique est
`<nimby/client.hpp>`. Les anciens exports `NimbySdk_*` et les headers
`nimby/sdk.h` et `nimby/observation.h` sont retirés. Le TCO existant doit être
migré séparément ; remplacer sa DLL ne suffit pas.

Télécharger les paquets : [release 0.7.2](https://github.com/NimbyRails-France/sdk/releases/tag/v0.7.2).

Voir les [essais de la 0.7.1](docs/release-0.7.1-validation.md), dont le changement
de sauvegarde et les textures qui suivent les états des signaux.

## Utiliser le SDK

```cpp
#include <nimby/client.hpp>
#include <iostream>

int main() {
    try {
        auto client = nimby::Client::connect();
        auto snapshot = client.capture();
        for (const auto& train : snapshot->getAllTrains()) {
            std::cout << train.getName();
            if (auto speed = train.getSpeedKmh())
                std::cout << " : " << *speed << " km/h";
            std::cout << '\n';
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
```

```cmake
find_package(NimbyRailsFranceSDK 0.7 CONFIG REQUIRED)
target_compile_features(mon_outil PRIVATE cxx_std_20)
target_link_libraries(mon_outil PRIVATE NimbyRailsFranceSDK::SDK)
```

Utiliser le kit installé avec `-DCMAKE_PREFIX_PATH=C:/SDK/NimbyRailsFranceSDK-0.7.0`.
Distribuer ses DLL de `bin/` avec votre programme. Les trois exemples autonomes
montrent aussi leur copie automatique par CMake.

- [Tutoriel C++ et rafraîchissement automatique](docs/tutorial-cpp-client.md)
- [Premier outil : captures manuelles](docs/tutorial-first-tool.md)
- [Créer un mod C++ depuis un dossier vide](docs/tutorial-create-mod.md) — écrire ses fichiers, sa DLL et ses actions avec l'adaptateur du SDK de développement
- [Référence C++ complète](docs/cpp-api-reference.md)
- [Migration 0.6 vers 0.7](docs/migration-0.7.md)
- [Quais et occupations](docs/platform-occupations.md)
- [CLion](docs/clion.md) · [Dépannage](docs/troubleshooting.md)

## Construire et distribuer

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools/package.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File tools/package-drop-in.ps1 -SkipBuild
powershell -NoProfile -ExecutionPolicy Bypass -File tools/package-hub-sdk.ps1 -SdkRoot ./dist/NimbyRailsFranceSDK-0.7.2
```

Le premier script construit Release, lance les tests et compile les trois
exemples depuis le paquet installé. Les archives locales sont dans `dist/` :
kit MinGW, chargeur drop-in et paquet Hub. La DLL s'appelle
`NimbyRailsFranceSDK.dll`, le chargeur `NimbyRailsFranceLoader.exe`.

L'observation externe ne nécessite ni proxy ni injection. Pour le proxy,
suivre [l'installation dédiée](docs/install-drop-in.md). Une ancienne installation
0.6 doit être désinstallée avec son ancien installateur avant d'installer 0.7.

## Contrat et limites

Les captures sont immuables mais les lectures du jeu ne sont pas atomiques.
Une valeur inconnue reste optionnelle ; elle ne signifie ni zéro, ni voie libre.
Les réservations sont distinctes des occupations réelles. Le plan de ligne
n'est pas une garantie sur le prochain parcours effectif. Les temps sont ceux
du jeu, indépendants de l'horloge réelle.

Le compteur lit les voyageurs embarqués. La vitesse mesurée est distincte du
zéro d'affichage par défaut (`isSpeedDefaulted()`). La position utilise les
états natifs ; un train sans mesure de vitesse n'est pas forcément au dépôt.

Profil du jeu reconnu :
`fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae`.

L'observation est en lecture seule. L'appel explicite `setSimulationDateTime`
modifie l'origine calendaire du jeu. Les fichiers `nimby/detail/` et les exports
`NimbyInternal_*` forment un pont privé de version 2, nécessaire aux helpers
compilés dans l'application. Ils ne constituent pas une API consommateur.
Les anciennes notes dans `docs/research/` restent des preuves historiques.

L'IHM d'auto-observer utilise `setSimulationDateTimeAndRecalculateTrains` pour
changer l'heure et déclencher une intervention native globale : services et
attentes recalculés, trains replacés à leur prochaine destination. Le pont
versionné est chargé à cet appel uniquement. Voir [les essais et limites](docs/simulation-clock.md).

## Versions, changelog et notifications

- La version de référence est dans `VERSION`. Elle doit correspondre à `CMakeLists.txt` ou à `package.json` et son lockfile, selon le projet.
- Documenter les changements dans `CHANGELOG.md`, sous `[Unreleased]` pendant le développement, puis dans une section `## [X.Y.Z] - AAAA-MM-JJ` au moment de publier.
- Après une CI réussie, créer le tag `vX.Y.Z` sur le commit vérifié et publier sa release GitHub avec les notes de cette section (`python .woodpecker/check-release.py --notes`). Joindre les artefacts construits avec l'outillage habituel lorsqu'ils sont nécessaires.
- Les builds Woodpecker sont annoncés dans le salon Discord `1550478726557470791`. Seules les releases GitHub publiées, versionnées et avec des notes sont annoncées dans `1549088597594873907`. Un push ou un tag seul ne publie aucune annonce de mise à jour.
- La CI refuse les incohérences de versions et les tags sans changelog daté. Les releases en brouillon ne sont pas annoncées. Une correction des notes modifie l'annonce existante.

Woodpecker compile Windows x64 avec MinGW et exécute les tests CTest autonomes sous Wine. Cela ne remplace pas les essais dans le jeu ni la validation native Windows des installateurs et scripts PowerShell.

## Canaux de publication

**Stable** : `vX.Y.Z` (release normale). **Bêta** : `vX.Y.Z-beta.N`. **Alpha** : `vX.Y.Z-alpha.N` (ces deux dernières sont des prereleases GitHub). `N` commence à 1. Le Hub mémorise un canal par projet, stable par défaut, sans basculer vers un autre canal si aucune release n’existe. Un retour vers une version plus ancienne nécessite une installation manuelle.

`VERSION` et le manifeste portent la version complète ; la version CMake garde seulement `X.Y.Z`. Publier le ZIP et son `project.json` dans la **même release**, avec son changelog. Pour le Hub lui-même, publier l’installateur et `hub-latest.json`. Le manifeste donne la taille, le SHA-256, le dossier racine et les règles de compatibilité. Aucun catalogue central ne doit être modifié.

La politique est dans `release-channels.json`. Le contrôle `.woodpecker/check-release.py` refuse les autres canaux. Une release de test n’est jamais marquée comme dernière version stable.

## Publier une mise à jour

- **main** : canal stable.
- **alpha** : canal alpha.
- **beta** : canal beta.

Un commit ordinaire lance les vérifications sans publier. Pour publier, préparez la même version dans `VERSION` et les fichiers de version du projet, puis ajoutez une entrée datée dans `CHANGELOG.md`. Décrivez les nouveautés, améliorations et corrections du point de vue des utilisateurs.

Le titre exact du commit de publication est `release X.Y.Z` (exemple : alpha : `release 0.4.0-alpha.1` ; beta : `release 0.4.0-beta.1`). Poussez ce commit sur la branche du canal choisi. La compilation, les tests et la préparation des téléchargements doivent réussir avant la publication GitHub et son annonce Discord. Une version déjà publiée ne peut pas être remplacée : choisissez un nouveau numéro.

Ne créez pas le tag à la main. Les préversions restent dans leur canal et ne remplacent pas la version stable.
