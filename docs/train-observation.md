# Observations de trains, lignes et matériel

Le SDK fournit des observations copiées ; il n'implémente aucune priorité ni
régulation. Les identifiants `TrainId`, `LineId`, `StationId`, `TrackId`,
`TimetableId`, `TagId` et `VehicleModelId` sont opaques. Une propriété `null`
est indisponible ou n'a pas été demandée : elle ne signifie ni zéro passager,
ni train arrêté, ni composition vide.

## Lectures selon le besoin

`ToolContext.trains(query)` pour les mods Kotlin natifs et
`Game.trains.snapshot(query = query)` pour les applications JVM demandent une
capture dédiée. `TrainQuery()` lit par défaut le service et la localisation.
Les autres groupes sont explicites :

| Option | Données demandées |
| --- | --- |
| `includeService` | État, alerte, présence, affectation et échéances actives |
| `includeLocations` | Position et gares référencées |
| `includeCharacteristics` | Vitesse maximale et caractéristiques du matériel |
| `includeTimetables` | Plans d'arrêts ; implique le service |
| `includeTags` | Tags déclarés et leur catalogue ; implique les lignes |
| `includePassengers` | Nombre d'occupants observé |
| `includeLines` | Toutes les lignes, y compris celles sans train affecté |
| `includeComposition` | Véhicules configurés/actuels et modèles référencés |

Le nom et les observations de mouvement de base restent inclus. Mettre toutes
les options à `false` demande seulement ce socle. Les groupes sont lus ensemble
pour l'ensemble des trains, sans une capture par train. La capture dédiée ne
lit ni carte entière, ni signaux, ni textures, ni occupations, ni réservations.
Les callbacks inactifs et le BAL ne demandent pas ces groupes supplémentaires.

Dans un callback natif, `trains()` réutilise la copie déjà obtenue. Une demande
plus riche étend les groupes demandés ; une demande couverte par cette capture
ne la relit pas. `network()` et une modification explicite de l'horloge
invalident ce cache. Les copies restent utilisables après le callback ; le
`ToolContext` ne peut pas être conservé. Le JVM réalise une nouvelle capture
à chaque appel explicite de `snapshot` et sérialise les appels d'une même
session, sans verrou global partagé entre clients.

## Trains et jointures

Le `TrainSnapshot` natif fournit `trains` et une recherche indexée
`snapshot[TrainId(...)]`. Le JVM fournit `Observation.train(id)`, qui joint
train, service, détails, matériel et gares par des index locaux sans appel
natif caché. Les gares et noms proviennent de la même capture.

La vitesse observée est en m/s (`speedKmh` pour l'affichage). Une vitesse de
secours reste `null` avec `speedDefaulted = true`. Les enums `TrainState` et
`TrainAlert` évitent d'interpréter les codes moteur dans un mod. Les
identifiants d'horaire et de service sont conservés ; un service est identifié
dans son horaire par `TimetableShiftId`. Les noms non résolus restent `null`.

Les champs `capturedAtMillis` désignent l'heure UTC de l'ordinateur ;
`TrainSnapshot.ageMillis` utilise une horloge monotone au moment de la copie.
La simulation continue pendant la capture. Les validations sont faites par
enregistrement et par jointure, sans arrêt atomique du jeu.

## Caractéristiques et composition

`configured` et `current` sont deux profils indépendants de
`TrainCharacteristics`. Le SDK ne remplace jamais une donnée actuelle absente
par une donnée configurée. `maximumSpeedMps` désigne la vitesse maximale du
matériel, distincte de la vitesse instantanée du train et de la limite de voie.
Les unités figurent dans les propriétés : mètres, kilogrammes, m/s², watts et
newtons. La capacité voyageurs et le nombre de véhicules sont indépendants
du nombre de passagers actuellement observé.

L'option `includeComposition` est indépendante des caractéristiques physiques.
Chaque profil possède une `composition: List<TrainVehicle>?`, ordonnée par
`index`. Une liste vide signifie une composition connue vide ; `null` conserve
l'indisponibilité. Chaque véhicule référence un `VehicleModel` : identifiant,
code, `nameEnglish` et nom de source lorsqu'ils sont disponibles. Le catalogue
retourné contient les modèles référencés par les compositions observées. Si un
modèle est inconnu, le véhicule et son identifiant restent présents avec des
noms `null`. Aucune classification voyageurs/fret n'est inventée.

## Lignes et tags

Le catalogue des lignes comprend aussi les lignes non affectées à un train.
`LineType.Depot` est la seule catégorie particulière validée ; `Other`
n'implique aucune catégorie commerciale. La filiation distingue une racine
connue d'un parent inconnu.

`declaredTags` contient les tags déclarés sur l'objet. `tagsForLine(LineId)`
ajoute les tags des parents avec déduplication. Un parent manquant, une liste
de tags inconnue, un cycle ou le dépassement d'une borne donnent `null`, jamais
une liste présentée comme complète. Le parcours est limité à 256 ancêtres et
65 536 tags uniques. Les tags ne déclenchent aucune priorité automatique.

## Temps, prévision et horaires

`TrainServiceTimes` expose les compteurs en microsecondes depuis l'origine de
simulation. `observedAt`, `arrival`, `departure` et `dispatchRetry` sont des
`GameInstant` UTC du calendrier du jeu ; le JVM fournit des `Instant`.
La conversion exige une origine calendaire validée et conserve les compteurs
signés et la fraction de seconde. Les débordements restent indisponibles.

Les échéances actives ne sont pas nécessairement des horaires commerciaux.
La nouvelle tentative de dispatch n'est pas un départ commercial. Le temps
restant avant arrivée peut être négatif ; départ et dispatch sont bornés à
zéro. `predictedArrivalDelaySeconds` est une prévision signée distincte,
disponible seulement lorsque les données du calcul sont valides. Elle ne
constitue ni une priorité ni une commande de régulation.

`ToolContext.linePlan(trainId)` retourne le plan demandé ou `null` s'il est
absent ou instable, et conserve les waypoints hors gare. Le JVM peut demander
le plan via `Game.trains.snapshot(selectedTrain = id)`. Les offsets
`arrivalOffsetSeconds` et `departureOffsetSeconds` restent relatifs au plan :
aucune date absolue de train n'en est déduite. Le lecteur accepte une arrivée
non négative et un départ au moins égal à l'arrivée. Sinon, les horaires de
cet arrêt restent inconnus. `plannedDwellSeconds` évite le débordement 32 bits.

## Compatibilité et validation technique

`Game.snapshot(selectedTrain)` conserve la capture réseau existante sans
ajouter les nouvelles tables de matériel/tags. `Game.trains.read(id)` reste
une lecture ciblée du mouvement et `Game.clock.read()` une lecture de l'horloge.
Un ancien runtime dépourvu des nouvelles tables les laisse indisponibles.

Le transport natif utilise des pages de 32 enregistrements et des buffers
réutilisés. Les tests couvrent 16 385 trains, les 4 096 véhicules d'une
composition, 32 objets portant chacun 4 096 tags, les compositions distinctes
et les modèles inconnus. Les plafonds de capture sont 262 144 associations de
tags, 262 144 véhicules et 16 384 modèles ; les données incomplètes ne sont
jamais exposées comme une liste complète. Des compteurs de copies vérifient
qu'une requête minimale ne copie que les trains et qu'aucune requête dédiée
ne copie la topologie, les textures ou les chemins du réseau entier.
