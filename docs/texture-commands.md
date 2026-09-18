# Commandes visuelles de texture C++ (expérimental)

## API de haut niveau pour les mods

```cpp
#include <nimby/signal_textures.hpp>
auto textures = nimby::SignalTextures::inGame();
textures.show(signalId, {"sfr_bal_a_cpp_v1", "imgs/ca/sem_bal/tex00.svg"});
textures.restore(signalId);
```

Un outil externe utilise `SignalTextures::connect(pid)` avec les mêmes commandes.
Le SDK résout le chemin du fichier dans le jeu de textures chargé, valide le
signal et gère le pont de rendu. Les séparateurs `/` et `\` sont acceptés ; un
fichier manquant renvoie `DataUnavailable`, une référence ambiguë est rejetée.
L'API ne charge pas un fichier arbitraire du disque : les ressources doivent
déjà être déclarées et activées dans le jeu. Aucun index de texture n'est nécessaire.

La route interne au jeu initialise directement le pont, sans injection distante
dans son propre processus. Le loader est livré avec la DLL du pont correspondante.
Appeler après le chargement de la partie, hors `DllMain`. Le démarrage du mod
après SDL_Init ne signifie pas encore que la simulation est disponible.

## API par index et diagnostics

```cpp
#include <nimby/texture_preview.hpp>
nimby::forceSignalTexture(pid, signalId, "sfr_bal_a_cpp_v1", 2);
// La commande reste dans le jeu après la fermeture du programme appelant.
nimby::forceSignalTexture(pid, signalId, "sfr_bal_a_cpp_v1", 4);
nimby::restoreSignalTexture(pid, signalId);
```

`previewSignalTexture` conserve la durée limitée de 1 à 60 secondes. `forceSignalTexture` n'expire pas : le pont déjà chargé reconnaît la valeur maximale de son échéance. Une commande valide remplace la précédente pour le même signal ; un index invalide conserve la commande en cours. Le retour au rendu natif est explicite, également possible via l'ancien alias `clearTexturePreview`.

Le pont v3 stocke les signaux indépendants dans une table extensible, sans quota fixe de 4 096 entrées. Appeler `forceSignalTexture` pour chaque ID ; remplacer ou restaurer un signal conserve les autres commandes. Chaque aperçu a sa propre expiration. Les entrées expirées sont récupérées à la commande suivante. La mémoire disponible et la taille maximale du conteneur restent des limites techniques ; un échec d'allocation renvoie `ResourceLimit`. La recherche au rendu est binaire, sans allocation. Un thread du pont traite les commandes reçues dans une boîte partagée et agrandit la table sous verrou ; le rendu peut attendre ce verrou pendant une mutation. Aucun conteneur C++ ni pointeur vers celui-ci n'est partagé entre processus.

Les commandes vérifient l'ID complet, la présence du jeu de textures et les limites d'index. Elles sont liées aux pointeurs de la base et de la simulation courantes et ne sont pas enregistrées dans la sauvegarde. La détection d'un rechargement avec réutilisation exacte des adresses mémoire reste une limite du prototype.

Le maximum réel de signaux du jeu n'est pas établi. La [documentation des signaux](https://wiki.nimbyrails.com/Signal) consultée ne donne pas de nombre maximal. Notre lecture des IDs extrait un index de 32 bits, ce qui ne prouve pas que toutes les valeurs sont utilisables simultanément. Le lecteur commun `network.cpp` conserve ses garde-fous de lecture mémoire (notamment 1 048 576 emplacements par collection et 8 192 octets pour le tableau des blocs) ; ils peuvent refuser une très grande collection. Ce sont des bornes du lecteur SDK, pas une limite prouvée du jeu. La suppression du quota de forçages ne signifie donc pas que toute taille de sauvegarde est déjà validée.

`getSignalTextureOverrideStatus(pid, signalId)` retourne `active`, `index`, `expires_at_ms` et le nombre total `active_count`. Les compteurs de `getTexturePreviewStatus` sont cumulatifs et communs à tous les signaux ; ses champs signal/expiration décrivent uniquement la dernière commande. La validation de chaque commande relit actuellement le réseau et le catalogue : cette API n'est pas encore une commande groupée pour actualiser tout un réseau à chaque image.

Après un passage du pont v1 ou v2 au v3, relancer le jeu : les anciens ponts sont épinglés en mémoire. Le client refuse explicitement de charger la v3 si la v1 ou la v2 est résidente, pour éviter deux interceptions concurrentes. Le statut expérimental par signal utilise maintenant un compteur 64 bits : recompiler les consommateurs avec le nouvel en-tête.

La substitution intervient au rendu. Le sélecteur natif lu dans les snapshots, les permissions de circulation et le comportement des trains restent distincts de cette commande visuelle. `getTexturePreviewStatus` fournit les compteurs de substitution ; l'échéance `UINT64_MAX` indique le mode forcé, zéro le retrait. Ces compteurs ne prouvent pas à eux seuls le succès du chargement GPU de l'image.

Le pont expérimental MinGW x64 vérifie le binaire du jeu et les instructions du point d'interception avant activation. Il reste chargé jusqu'à la fermeture du jeu ; restaurer retire l'effet visuel, sans décharger la DLL. Le SDK client et le pont doivent correspondre au même build.

Le paquet `assets/mod.txt` de Signalisation française réaliste ne contient que des ressources et un modèle de signal, sans NimbyScript.

Validation de la v1 en jeu : maintien sans expiration, remplacement d'index, rejet d'index invalide et de restauration sur un autre ID, puis restauration du rendu natif. Le test a utilisé `waw_signal_idx_4` (deux textures natives), avec des compteurs de substitution positifs. Le jeu de textures personnalisé `sfr_bal_a_cpp_v1` était indisponible lors de cette validation ; son affichage visuel reste à vérifier après chargement du paquet. Le script opt-in `tools/verify_texture_commands.py` restaure le signal en fin de test, y compris en cas d'échec après activation. Pour la v3, les tests automatiques couvrent 100 000 entrées, recherche, remplacement, suppression, expiration et traitement des commandes avec changement de contexte. Le transport interprocessus et le rendu v3 en jeu restent à vérifier après redémarrage.
