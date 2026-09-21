# Référence de l'API Kotlin

L'API publique du kit est définie dans `nimby`, livrée en `.klib` et accompagnée
de `sources/nimby-mod-api-sources.jar`. Elle expose actuellement les mods de
signalisation via `SignallingMod`. Elle ne remplace pas toute l'API du client C++.

## Point d'entrée et identité

Le projet fournit :

```kotlin
package nimby.mod

import nimby.SignallingMod

fun createMod(): SignallingMod = MySignals()
```

`MySignals` est une sous-classe du mod. Le SDK fournit les exports natifs et
l'adaptateur ; le code consommateur n'utilise ni `DllMain`, ni JNI, ni pointeur.

| Propriété | Usage |
| --- | --- |
| `id`, `title` | Identifiant stable du panneau et titre lisible |
| `textureSet` | Identifiant du catalogue de textures du jeu |
| `checkboxes` | Réglages déclaratifs : nom, libellé, description, valeur initiale |
| `unknownDecision` | Décision utilisée quand les données sont indisponibles |
| `invalidNetworkDecision` | Décision de repli pour les liens invalides |
| `diagnosticFile` | Nom simple du journal de défauts, sans chemin |
| `maximumLineSpeed` | Option transmise au runtime de signalisation |

La liste des cases est limitée à 64 noms uniques. Les identifiants numériques
des objets du jeu utilisent `Long` ; ne pas les convertir en `Double`.

## Évaluer les signaux

`evaluate(settings, observation)` calcule une décision locale.
Une `Decision` contient les codes entiers `aspect` et `reason`, définis par
le mod. Des enums métier et une conversion à la frontière évitent les codes
numériques dispersés dans les règles.

`Observation` contient notamment l'occupation (`Unknown`, `Clear`,
`Occupied`), la fraîcheur, la connaissance de l'itinéraire, l'arrêt imposé,
le défaut de lampe et l'aspect aval. Une observation inconnue ne prouve pas
que la voie est libre.

`decide(signal, next)` permet de résoudre les dépendances aval : retourner
`null` lorsque la décision nécessite l'aval non encore fourni.
`evaluateNetwork(signals)` résout les liens, détecte les cycles nécessaires
au calcul et utilise `invalidNetworkDecision` pour les liens non résolus.
Le réseau est limité à 512 signaux ; les IDs doivent être uniques et non nuls.

`Signal.settingsStatus` distingue les réglages absents, présents et
indisponibles. Le mod doit donner à ces cas un comportement explicite.
`fromLive(signal)` adapte une observation avant son traitement.

## Rendu et conduite

| Méthode | Contrat |
| --- | --- |
| `texture(decision, simulationMs, halfPeriodMs)` | Chemin de texture correspondant à la décision ; le temps est celui de la simulation |
| `isFault(decision)` | Indique si la décision doit être diagnostiquée comme défaut |
| `isActive(decision)` | Indique si le profil est actif pour cette décision |
| `aspectName(aspect)`, `reasonName(reason)` | Libellés destinés aux diagnostics |
| `drivingRule(decision)` | Consigne de conduite, ou `null` si le mod n'en fournit pas |
| `plan(vehicle, settings, input, constraints)` | Calcul d'un plan ; par défaut, plan indisponible |

Les unités sont mètres, secondes, m/s, kg, N et W. `Vehicle` décrit les
caractéristiques physiques ; `DrivingInput` décrit la position, la vitesse,
la fraîcheur et les informations de parcours. `Constraint` porte les limites
métriques, la vitesse et la condition de libération par la queue.

`DrivingRule` associe des vitesses, un nombre de panneaux en aval et des
`DrivingFlag` : `Clear`, `HoldToClear`, `Stop`, `FollowTarget`,
`OnSight`, `StopThenProceed`, `CancelAtNextClear`. Ces valeurs doivent être
combinées selon les règles du mod ; le SDK ne déduit pas une permission d'une couleur.
Les plans acceptent au plus 4 096 contraintes.

## Cycle de vie et erreurs

Le loader démarre et arrête le mod via l'adaptateur fourni. Le runtime Kotlin
reste chargé jusqu'à la fermeture du processus pour préserver ses ressources :
une nouvelle compilation exige de redémarrer le jeu.

Les objets Kotlin ne traversent pas l'ABI. Le SDK transporte des valeurs et
des buffers bornés, copie les chaînes en UTF-8 et transforme les exceptions en
statuts d'échec. Les fonctions `nimby.internal` et les exports
`NRFKotlin_*` sont privés.

La vérification native de Gradle couvre les exports et le cycle de vie.
Les tests métier du mod et les essais en jeu restent nécessaires pour valider
les décisions, les textures et la conduite.