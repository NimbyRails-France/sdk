# Version du SDK et NRF Hub

La version publiée actuelle est **0.6.6**, toujours ABI 1.
La 0.6.6 corrige `NimbyTrainDetails.passenger_count` (voyageurs embarqués, pas capacité), sans changer les structures ni l'ABI.

La 0.6.5 ajoute `NimbySdk_CopyPlatforms` et la vue C++ des occupations par gare. Les nouveaux helpers et exemples demandent la DLL 0.6.5+.

La 0.6.4 ajoute `NimbySdk_CopyTrainDetails` et `NimbySdk_CopyTrainLineStops`, sans agrandir les structures existantes. Les nouveaux helpers et exemples demandent la DLL 0.6.4+.

La 0.6.3 ajoute `NimbySdk_CopyTrainServices` : états natifs, emplacement masqué,
arrêt actif, ligne et échéances d'arrivée/départ. Les helpers 0.6.3 et les
exemples requièrent la DLL 0.6.3 ou ultérieure (nouvel export).
La version 0.6.2 résout les noms automatiques sans modifier `NimbyStation`.
La version 0.6.1 ajoute `NIMBY_TRAIN_SPEED_VALID` et `NIMBY_TRAIN_SPEED_DEFAULTED` sans
modifier la taille de `NimbyTrain`. Les helpers C++ lisent aussi les valeurs
des DLL 0.6.0, qui utilisaient uniquement `NIMBY_TRAIN_PRESENT`.

Le SDK 0.6.0 ajoute `NimbySdk_GetVersion(NimbySdkVersion*)`. Initialiser
`struct_size` à `sizeof(NimbySdkVersion)` ; la fonction renvoie l'ABI et les
trois composants de version sans ouvrir le jeu ni initialiser de hooks.

Le TCO 0.4.0 charge sa DLL avec un chemin absolu, vérifie l'export de version,
requiert SDK 0.6.x / ABI 1 et résout ensuite les fonctions d'observation.
Une DLL absente, ancienne ou incomplète affiche une erreur explicite. Le mode
`--check-sdk` renvoie 0 si compatible, 3 sinon, sans ouvrir de boîte de dialogue.

NRF Hub vérifie le hash du jeu et les contraintes du catalogue avant installation.
Ses paquets SDK contiennent un sous-dossier `loader/` pour l'installation gérée
du proxy. Les autres paquets du SDK conservent leurs usages existants.

Les états et textures restent observés sans interprétation automatique des
règles ferroviaires. Les nouvelles fonctions n'autorisent pas de hooks internes.
