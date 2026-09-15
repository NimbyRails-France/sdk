# Lecture des états natifs — 2026-09-15

Profil : NIMBY Rails, SHA-256
`FFF49AC21720ABFC824C2B4F68B862727630EB0DB71CFE1F9EA8F685D0DB10AE`.

## Source de la valeur

Le rendu `RVA 0x620140` consulte `Simulation + 0x2200`, puis une table Swiss
à `Query + 0x378`. En-tête : contrôles +0, slots +8, taille +16, masque +24.
Les slots font 16 octets : ID complet du signal +0, sélecteur int32 +8.
Les contrôles inférieurs à 128 désignent les slots utilisés ; le masque
est de forme 2^n−1 et exclut le slot sentinelle terminal.

Le rendu joint l'ID, prend le sélecteur et appelle `RVA 0x4148d0` avec le type
`Signal + 0x30` et le hash de textures `Signal + 0x38`. Ensuite il borne le
sélecteur selon la taille du tableau de textures (éléments de 0x50 octets),
avec une possibilité de repli vers les textures par défaut.
Source exportée : `reports/signal-state/draw/140620140.c`.

Cette lecture expose donc le sélecteur **avant** clamp/repli, pas une permission
de circulation, ni la garantie qu'une texture particulière soit visible.

## Observation vivante

Processus 3824 : 27 signaux dans une table de taille 27, masque 31.
Le probe en lecture seule `tools/signal_state_probe.cpp` a observé sur 100
lectures le signal `0x8000000110003` alternant entre 9 et 10 (49 valeurs
initiales/changements au total). D'autres sélecteurs observés : 0, 2, 3, 4.
Trace locale : `build/signal-state-live.txt`.

Le test ABI public sur ce processus confirme les jointures et expose notamment
`nimby:09c4af134204998c:kind.4.state.10`. Le débogueur CLion a confirmé auparavant
la capture des 27 signaux dans `NimbySdk_CaptureSnapshot`, appelée depuis
`tests/observation.cpp:32`, et l'initialisation inconnue de l'ancien adaptateur.
La preuve des sélecteurs vivants vient du probe, pas d'un arrêt du jeu au rendu.

## Garde-fous et limites

Lecture bornée à 1 048 575 slots, contrôle des pointeurs, IDs, doublons,
nombre d'entrées, en-tête, contrôles et données relus, puis vérification de
Query et des racines du jeu. Trois essais au plus. Une lecture instable laisse
les états indisponibles indépendamment du reste du snapshot.

Le snapshot reste non atomique : une double lecture égale ne garantit pas un
tick de simulation commun. Le nom ferroviaire des états des mods n'est pas
déduit d'un entier. Aucune écriture, injection ou invocation de script natif.
