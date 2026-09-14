# Essais automatiques A/B — 14 septembre 2026

Suite : [résolution stable après redémarrage](stable-resolution.md). Le moniteur
utilise désormais les propriétaires Copie/Simulation ; l'écriture de la copie est
refusée. Les commandes A/B ci-dessous décrivent le protocole historique.

**Résultat : B est la source à privilégier pour les expériences de vitesse dans
cette session.** Deux écritures sur A ont été écrasées rapidement, sans entraîner
la même baisse dans B. Deux écritures sur B ont été suivies d'une augmentation
progressive de B et d'une propagation de cette nouvelle vitesse dans A.

## Conditions

- Autorisation explicite de l'utilisateur pour les essais automatiques.
- Binaire SHA-256 `FFF49AC21720ABFC824C2B4F68B862727630EB0DB71CFE1F9EA8F685D0DB10AE`.
- Processus `73252`, train **B81500**, ID complet `0x5000000000003`.
- Collection A `0x1CD80A401E0`, collection B `0x1CDCA7A1C40`, retrouvées par
  découverte avant chaque essai ; adresses de tas propres à cette session.
- Le moniteur interactif a été fermé pendant les essais pour éviter des clics
  d'écriture concurrents. Aucun autre train n'a été ciblé par nos écritures.
- Ordre A1, B1, A2, B2. Chaque essai effectue une seule écriture du double de
  vitesse à **5 km/h**, avec une vitesse préalable supérieure à 20 km/h.
- Environ une seconde de mesures avant écriture, puis 101 mesures après,
  espacées d'une attente de 50 ms. Les intervalles effectivement observés sont
  proches de 61–63 ms ; les temps ci-dessous sont les temps muraux mesurés,
  pas des ticks de simulation. Les lectures A/B sont successives.
- Pas de changement de pause, du multiplicateur de simulation, de protection
  mémoire, de thread ou de hook. Pas de boucle de maintien de vitesse ni de
  restauration ultérieure d'une valeur devenue périmée.

## Mesures

| Essai | Source écrite | Avant (km/h) | Relecture immédiate | Évolution observée |
| --- | --- | ---: | ---: | --- |
| A1 | A | 160,000008 | 5 | A encore à 5 à 62 ms ; A revenu à 160,006438 à 124 ms. B reste proche de 160. |
| B1 | B | 99,658698 | 5 | B = 5,142560 à 62 ms, puis 19,707441 à 6 187 ms. A reprend la valeur basse au plus tard à 188 ms. |
| A2 | A | 38,477818 | 5 | A revenu à 38,302318 dès 62 ms ; B continue sa trajectoire autour de 38. |
| B2 | B | 45,932523 | 5 | B = 5,142560 à 61 ms, puis 19,612401 à 6 152 ms. A reprend la valeur basse dès la mesure à 61 ms. |

Les traces complètes, dont l'heure UTC et les identités des collections, sont
dans `reports/trains/experiment-A1.txt`, `experiment-B1.txt`, `experiment-A2.txt`
et `experiment-B2.txt`. Les quatre processus d'essai se sont terminés avec le
code 0 ; chaque relecture immédiate a donné exactement 5 km/h.

## Interprétation et limites

Le constat de l'utilisateur est reproduit. Les observations soutiennent
l'hypothèse **B = état dynamique de référence, A = état dérivé ou copie**.
Elles ne démontrent pas encore l'identité du thread propriétaire ou la fonction
qui propage B vers A. Le déplacement et les autres composantes physiques du
train n'ont pas été mesurés : la preuve porte sur ces champs de vitesse.

Une écriture sur B change la vitesse instantanée observée ; elle n'impose pas une
consigne permanente, puisque le moteur continue son évolution. La lettre B est
un ordre de découverte, pas un identifiant moteur stable. **Ne pas coder
« toujours la deuxième collection » dans le SDK** sans retrouver son propriétaire
par une chaîne ou un point d'observation validé. L'écriture externe reste exposée
aux changements concurrents d'objets malgré les contrôles avant écriture.

Prochaine recherche : suivre les écritures/copies entre ces deux collections,
identifier les threads et un instant cohérent de lecture, puis retrouver B de
façon stable après changement de partie et redémarrage. Aucun hook interne n'est
autorisé par ces seuls essais.

## Reproduire explicitement

Le mode ajouté au moniteur est volontairement explicite et borné :

```powershell
& build/Release/NimbyTrainMonitor.exe <PID> --experiment <ID-complet> 0 | Out-File essai-A.txt
& build/Release/NimbyTrainMonitor.exe <PID> --experiment <ID-complet> 1 | Out-File essai-B.txt
```

Ce mode effectue réellement une écriture. Il exige deux collections Motion,
une seule collection Train, un ID présent, le binaire reconnu et la même instance
de processus. Il renonce si la vitesse de la source est inférieure à 20 km/h au
contrôle préalable. Le journal de cette voie est sa sortie standard ; le journal
`speed-tests.log` concerne le bouton interactif.

Validation du code : **6/6 tests CTest en Debug et Release**. L'exécutable du jeu,
la DLL SDK installée et les fichiers de sauvegarde n'ont pas été écrits par les
outils ; un enregistrement automatique du jeu pourrait conserver les conséquences
des modifications de son état en mémoire.

Après les essais, le moniteur a été rouvert avec cinq lignes. Une nouvelle piste
de contexte a été trouvée : objet `0x1CDCAAB0FA8`, table virtuelle
`0x7FF79B4BD430` (RVA `0xA6D430`, base chargée `0x7FF79AA50000`), propriétaire monde `0x1CDCA904300`, collection Motion B
`0x1CDCA7A1C40` (`reports/trains/after-experiments-window.txt`). Ce rapprochement
de pointeurs reste à valider par désassemblage et observation des appels.
