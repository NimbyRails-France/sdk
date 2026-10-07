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

Le pont actuel publie des tables immuables : le rendu effectue une recherche binaire et une garde de monde sans mutex ni allocation. Chaque propriétaire dispose de 4 096 entrées, avec 64 propriétaires et 65 536 entrées au total. Les lots sont bornés à 4 096 commandes ; les diagnostics groupés à 32 signaux. Un conflit de propriété ou un dépassement renvoie `ResourceLimit` sans remplacer les commandes d'un autre mod. Les entrées expirées sont récupérées lors des publications suivantes.

La restauration interne au jeu est idempotente par propriétaire : elle retire les textures du mod appelant, en laissant intactes celles reprises par un autre mod après expiration. Un succès signifie que les IDs demandés sont libérés pour cet appelant, pas que tous affichent une texture native. Tous les IDs sont validés avant suppression ; un échec de publication conserve le lot pour une nouvelle tentative. Un nettoyage sans effet ne publie pas une nouvelle table et ne perturbe pas une mise à jour concurrente d'un autre mod. Les conflits lors d'une publication restent refusés pour le lot entier.

Un ticket de génération est obtenu dans le pont avant les lectures du catalogue et de présence des signaux. Le pont le revalide à la publication : une ancienne requête ne reçoit jamais l'identité d'une nouvelle partie. Cette autorité existe également pour les outils externes qui chargent uniquement le pont. Une transition détectée invalide définitivement les anciennes commandes, même si les anciennes adresses, métadonnées ou ticks réapparaissent ensuite. Un recul de 50 vers 40 est détecté même lorsque la commande avait été publiée à 10. L'ordre des lectures empêche qu'une lecture concurrente terminée en retard fasse passer un mod sain pour un rechargement.

Ce détecteur ne constitue pas une notification native de chargement : une transition entièrement entre les observations, avec racines, métadonnées et ticks indistinguables, reste indétectable. L'historique complet est validé à l'ouverture d'une commande et lors de sa publication, jamais parcouru au rendu. Une modification uniquement à l'intérieur de cet historique, sans changement de son en-tête ni d'autre donnée surveillée, est détectée à la prochaine validation complète. Aucune commande n'est enregistrée dans la sauvegarde.

Le maximum de signaux du jeu n'est pas connu. Les quotas du SDK sont des bornes de ressources, pas une limite du jeu. Le retour aux textures natives ne garantit pas une indication restrictive : circulation et textures restent des fonctions distinctes.

`getSignalTextureOverrideStatus(pid, signalId)` retourne `active`, `index`, `expires_at_ms` et le nombre total `active_count`. Les compteurs de `getTexturePreviewStatus` sont cumulatifs et communs à tous les signaux ; ses champs signal/expiration décrivent uniquement la dernière commande. La publication groupée valide uniquement les signaux demandés et leurs catalogues ; les catalogues étrangers ne sont pas décodés. Les handles du processus sont réutilisés, tandis que les données du monde sont revalidées à chaque opération.

Le client et le pont doivent provenir du même build. Le protocole de boîte partagée est désormais en version 5 et la publication directe utilise `NimbyTexture_PublishV2`. La DLL conserve son nom expérimental v4 ; son empreinte binaire est vérifiée avant connexion. Un pont précédent déjà épinglé impose de relancer le jeu après installation du nouveau build : deux interceptions du même point natif ne sont jamais installées.

La substitution intervient au rendu. Le sélecteur natif lu dans les snapshots, les permissions de circulation et le comportement des trains restent distincts de cette commande visuelle. `getTexturePreviewStatus` fournit les compteurs de substitution ; l'échéance `UINT64_MAX` indique le mode forcé, zéro le retrait. Ces compteurs ne prouvent pas à eux seuls le succès du chargement GPU de l'image.

Le pont expérimental MinGW x64 vérifie le binaire du jeu et les instructions du point d'interception avant activation. Il reste chargé jusqu'à la fermeture du jeu ; restaurer retire l'effet visuel, sans décharger la DLL. Le SDK client et le pont doivent correspondre au même build.

Le paquet `assets/mod.txt` de AB Signalisation lumineuse ne contient que des ressources et un modèle de signal, sans NimbyScript.

Validation de la v1 en jeu : maintien sans expiration, remplacement d'index, rejet d'index invalide et de restauration sur un autre ID, puis restauration du rendu natif. Le test a utilisé `waw_signal_idx_4` (deux textures natives), avec des compteurs de substitution positifs. Le jeu de textures personnalisé `sfr_bal_a_cpp_v1` était indisponible lors de cette validation ; son affichage visuel reste à vérifier après chargement du paquet. Le script opt-in `tools/verify_texture_commands.py` restaure le signal en fin de test, y compris en cas d'échec après activation. Pour la v3, les tests automatiques couvrent 100 000 entrées, recherche, remplacement, suppression, expiration et traitement des commandes avec changement de contexte. Le transport interprocessus et le rendu v3 en jeu restent à vérifier après redémarrage.
