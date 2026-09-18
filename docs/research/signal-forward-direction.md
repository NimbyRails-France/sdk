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
