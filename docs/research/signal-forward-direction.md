# Sens de circulation d’un signal

Le visualiseur utilise le sens de `Signal_M_forward`, et non le sens brut de la position stockée.

Preuves dans les exports du moteur déjà conservés :

- `reports/network/registration.txt` associe `Signal_M_forward` à RVA `0x427ef0`, avec la description « position of a train facing forward from the signal ».
- `reports/network/140427ef0.c` copie la position stockée (+0x40, +0x48, +0x50) et inverse le sens uniquement pour le type 4 (Path).
- `reports/network/140427f20.c` effectue ensuite l’inversion supplémentaire pour `Signal_M_backward`.

Règle du visualiseur : `kind == 4 ? -direction : direction`. Le champ brut du SDK reste inchangé. Cette orientation n’est pas une autorisation de franchissement.

Les recherches brutes export?es couvrent les deux sens. Le visualiseur compose leurs chemins pour traverser les signaux oppos?s, sans retourner la direction de d?placement. Il conserve le sens d?arriv?e ? chaque jonction. Les IDs des liaisons physiques sont concat?n?s avec les n?uds.

Un signal rencontr? dans son sens actif devient la prochaine destination. Un NoWay ou un OneWay pris ? revers arr?te la branche. Les ?tats d?j? visit?s bornent les boucles. Cette recherche topologique n?est pas une autorisation IPCS ni l?itin?raire r?serv? d?un train.

Correction : arr?ter syst?matiquement au premier Path oppos? supprimait des destinations valides selon la r?gle de parcours demand?e. L?audit compare d?sormais cette composition avec une marche directe sur les ports natifs, sur tous les signaux de la capture.

## Correction du calcul des cantons

Le calcul BlockTopology utilisait le sens stocké au lieu de Signal_M_forward :
il pouvait chercher la limite du canton en arrière du signal Path. Le SDK
expose maintenant Signal::getForwardDirection() et BlockTopology utilise ce
sens au départ et pour filtrer les panneaux rencontrés. Signal::getDirection()
conserve sa valeur brute pour les consommateurs existants. SignalTopology
accepte explicitement la convention Forward ; sa convention par défaut reste
Stored pour préserver les outils géométriques existants.

Le test de régression place deux Path de direction stockée +1 sur une même
voie, aux fractions .8 et .2, plus un Path opposé à .5. Le premier doit trouver
celui à .2 dans son sens de circulation -1, sans prendre le panneau opposé.
