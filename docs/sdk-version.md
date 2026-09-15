# Version du SDK et NRF Hub

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
