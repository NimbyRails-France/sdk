# Lecture ciblée de conduite — SDK 0.7.3

Inclure `<nimby/client.hpp>`. `Client::readTrain(id)` lit un seul train, sans
capturer les voies, signaux, textures, voyageurs ou autres trains du réseau.
Réutiliser le client pour conserver le handle du processus et la validation
du binaire. La lecture n'écrit pas dans le jeu et n'installe aucun hook.

```cpp
auto client = nimby::Client::connect();
auto sample = client.readTrain(trainId);
if (sample) {
    auto speed = sample->getSpeedMps();
    auto position = sample->getPosition();
    auto dynamics = sample->getCurrentDynamics();
    if (dynamics) {
        const double length = dynamics->lengthM;
        const double braking = dynamics->serviceBrakingMps2;
        // Paramètres du matériel : ce n'est pas une commande de freinage.
    }
}
```

## Contrat

- `nullopt` : ID absent, partie indisponible ou lecture instable. Ne pas réutiliser
  la dernière valeur comme une observation fraîche. Un ID de type incorrect lève
  `InvalidArgument` ; un processus terminé lève `ProcessExited`.
- `getPurchasedDynamics()` et `getCurrentDynamics()` sont indépendants et optionnels.
  La première valeur correspond au modèle acheté, la seconde au Motion courant.
  Aucun remplacement implicite de l'une par l'autre.
- `TrainDynamics` fournit `maxSpeedMps`, `maxAccelerationMps2`,
  `serviceBrakingMps2`, `emergencyBrakingMps2`, `tractiveEffortN`, `powerW`,
  `emptyMassKg` et `lengthM`. Ce sont les caractéristiques déclarées par le jeu,
  pas des performances physiques mesurées ni la masse avec voyageurs.
- Vitesse et position sont optionnelles. `isSpeedDefaulted()` distingue le zéro
  de présentation natif sans Drive d'une vitesse mesurée nulle.
- `getElapsedBegin()` / `getElapsedEnd()` donnent l'intervalle de temps simulé
  encadrant la lecture. `getCapturedAt()` est l'heure système de fin de lecture.
- `getSessionGeneration()` change lors d'une racine remplacée, d'une perte de
  racine constatée ou d'un retour en arrière du temps simulé constaté. Le compteur
  appartient au client : une reconnexion invalide les anciennes mémoires même si
  le nouveau compteur reprend à 1. Ce n'est pas un UUID de sauvegarde.

Les records sont copiés et contrôlés deux fois ; l'identité complète des objets
et des pools est revérifiée. Trois essais immédiats au maximum. Les données
restent des observations externes optimistes, pas un tick atomique du moteur.
Un remplacement entre deux contrôles ou un cycle ABA ne peut être exclu.

Cette lecture ne prouve ni une autorisation de mouvement, ni le chemin restant,
ni une distance à un signal. La pente n'est pas exposée. Utiliser les captures
complètes pour la découverte et les autres tables à une fréquence appropriée.
Les captures et lectures du même `Client` sont sérialisées ; le registre natif
partagé sérialise aussi ses opérations. Éviter les captures complètes concurrentes
si l'on recherche une faible latence. Aucun budget temps réel n'est garanti.

## Disponibilité des commandes

`client.getDrivingCapabilities()` décrit les possibilités de cette version :
lecture ciblée disponible ; commandes de traction, frein de service, frein
d'urgence, limitation de vitesse et ajout d'interface native indisponibles.
Les écritures ponctuelles des anciens outils de recherche ne deviennent pas
des commandes de production. Les fonctions de calendrier et textures restent
séparées et ne constituent pas des commandes de conduite.

## Accès depuis un mod

`nimby::Mod` possède maintenant un callback optionnel
`std::optional<DrivingObservation> (*readTrain)(Id)`.
Le mod peut utiliser directement la fonction haut niveau `nimby::readTrain(id)`
déclarée dans `<nimby/mod.hpp>` et fournie par `NimbyRailsFranceSDK::Mod` :

```cpp
auto observation = nimby::readTrain(trainId);
```

Pour exposer aussi cette lecture au diagnostic, renseigner simplement
`.readTrain = nimby::readTrain` dans `createMod()`. Le SDK gère la connexion
différée, sa réutilisation, la synchronisation et sa libération à l'arrêt de
l'adaptateur. Si le processus quitte, la connexion est libérée et l'erreur
remontée ; l'appel suivant tentera une nouvelle connexion. Aucun callback
`stop` n'est nécessaire pour ce lecteur. Les valeurs manquantes restent
optionnelles et les erreurs de connexion restent des exceptions.

L'adaptateur ajoute l'export de
diagnostic `NRFMod_ReadTrainV1`, valide les arguments, transmet une copie à
travers le pont C privé et convertit les exceptions. Les anciens loaders peuvent
ignorer cet export supplémentaire. Les clients créés manuellement restent
sous la responsabilité du mod.

La DLL SDK **0.7.3 minimum** doit être disponible avant de charger un mod utilisant
ce nouvel export interne. Les structures ABI précédentes restent inchangées.
Les paquets et manifestes consommateurs doivent annoncer cette version minimale.

## Vérification

Fixtures : ID périmé, Motion absent, mouvement pendant lecture, position en sens
inverse, NaN, racine remplacée, distinction des zéros, arguments et callbacks.
Tests réels : DLL SFR chargée dans l'hôte de diagnostic, observant le jeu ouvert ;
100/100 lectures du TER B 82506. Premier appel 33,092 ms (connexion comprise),
puis moyenne 244,6 µs, minimum 66 µs, maximum 1850 µs sur 99 appels Debug.
Ces chiffres concernent cette session et ne comparent pas des volumes de données
équivalents à une capture globale. Le chargement de la nouvelle DLL dans le
processus du jeu exige encore le remplacement des binaires puis un redémarrage.

Sources de disposition : enregistrement des champs du binaire reconnu,
`tools/InspectDriving.java`, lecture exploratoire `tools/driving_probe.cpp`.
Le code de production réside dans `src/engine/driving.cpp` ; aucun offset du jeu
ne doit apparaître dans un mod consommateur.
