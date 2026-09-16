# Date de simulation (0.7.1)

La date provient de l'horloge native du jeu, même sans train. Elle est exposée
en UTC, indépendamment du fuseau choisi par le jeu pour son affichage.
Les dates avant 1970, dont 1940, sont acceptées.

```cpp
#include <nimby/client.hpp>
#include <iostream>

auto client = nimby::Client::connect();
auto snapshot = client.capture();
if (auto clock = snapshot->getSimulationClock()) {
    std::cout << clock->getDateTimeUtcString(); // ISO 8601, millisecondes, Z
    auto utc = clock->getDateTimeUtc();        // chrono::sys_time<Milliseconds>
    auto elapsed = clock->getElapsedTime();   // durée depuis l'origine native
}

using namespace std::chrono;
auto result = client.setSimulationDateTimeAndRecalculateTrains(
    sys_days{year{1940}/August/14} + hours{16} + minutes{48});
std::cout << result.clock.getDateTimeUtcString() << " / " << result.interventions;
snapshot = client.capture();
```

`getSimulationClock()` renvoie `std::optional<SimulationClock>` : l'absence de
date n'est jamais remplacée par l'heure du PC. `getEpochSeconds()` expose
l'origine native en secondes Unix signées.

`setSimulationDateTime(sys_seconds)` renvoie l'horloge effectivement écrite.
La phase sous-seconde est conservée : demander 16:48:00 peut donner
16:48:00.790. Le dernier snapshot du client est invalidé après succès ; les
snapshots déjà conservés restent immuables. Capturer à nouveau ou attendre
le prochain rafraîchissement automatique.

## Commandes de l'exemple auto-observer

```powershell
./build/clion-Release/examples/auto-observer/MyNimbyClient.exe --clock
./build/clion-Release/examples/auto-observer/MyNimbyClient.exe --set-date-utc 1940-08-14T16:48:00Z
./build/clion-Release/examples/auto-observer/MyNimbyClient.exe --set-date-and-recalculate-utc 1940-08-14T16:48:00Z
```

Sans argument, l'exemple ouvre une fenêtre graphique : date UTC en direct,
sélecteurs de date et d'heure, bouton **Appliquer et recaler les trains**, vues **Trains**
et **Gares / quais**, et recherche. **Date actuelle** remet les champs à la date
de la partie ; les rafraîchissements ne remplacent pas la saisie en cours.
Les lectures et modifications passent par un worker pour garder la fenêtre
réactive. En cas de déconnexion, une reconnexion est tentée automatiquement ;
une modification échouée n'est jamais rejouée automatiquement.

L'option `--console` conserve l'ancien affichage textuel des snapshots. Le kit
installé doit être en version 0.7.1 ou ultérieure compatible ; une DLL 0.7.0
ne contient pas les nouveaux exports internes.

## Portée expérimentale

Deux opérations sont disponibles. L'IHM utilise désormais
`setSimulationDateTimeAndRecalculateTrains(sys_seconds)`, qui renvoie
`SimulationTimeChange { clock, interventions }`. Le moteur exécute l'équivalent
du bouton **All trains intervention**, à la nouvelle date : services remis à
zéro, voyageurs transférés, trains replacés à leur prochaine destination et
attentes recalculées. Les trains remisés sont exclus comme dans le jeu. Le coût
normal des interventions est débité par le moteur. La reprise des circulations
et l'apparition des nouvelles attentes peuvent nécessiter quelques ticks.

Le pont `NimbyRailsFranceClockBridge-0.7.1.dll`, livré à côté de la DLL du SDK,
est chargé seulement lors de cet appel explicite. Il vérifie le binaire et les
entrées natives, puis traite la demande sur le thread de simulation, après la
mise à jour et avant la copie destinée à l'interface. Il reste chargé jusqu'à
la fermeture du jeu. Une demande non commencée expire après 15 secondes ;
elle n'est pas rejouée. `ClockWriteFailed` peut signifier qu'une intervention
a commencé : relire l'état, sans répéter automatiquement l'appel.

Ce mécanisme ne reconstruit pas la position théorique au milieu d'une voie et
ne simule pas les années écoulées. Le multijoueur, le rechargement d'une
sauvegarde après mutation et tous les systèmes économiques ne sont pas validés.

L'ancien `setSimulationDateTime` effectue uniquement une translation calendaire.
Un train qui attend deux heures au dépôt avant l'appel attend encore deux heures
après l'appel ; sa position ne change pas. Passer de 08:00 à 10:00 décale aussi
son départ calendaire de deux heures. La commande `--set-date-utc` conserve
explicitement ce comportement. Les détails ci-dessous concernent cette translation.

Ce setter décale l'origine calendaire et les dates absolues des services actifs
(départs, débuts de parcours et contrôle des voyageurs), sans modifier le compteur de simulation,
les positions ou les échéances exprimées en temps écoulé. Il ne rejoue pas
les années intermédiaires et ne réinitialise pas la partie. La cohérence de
tous les systèmes calendaires (comptabilité, caches hebdomadaires, changement
de jour, multijoueur, sauvegarde/rechargement) n'est pas validée. Utiliser une
copie de sauvegarde pour ces essais ; le test ci-dessous conserve le mercredi.

L'écriture est limitée au profil binaire reconnu. Un handle avec droits
d'écriture est ouvert uniquement lors du setter. Le processus distant est
brièvement suspendu ; les deux horloges natives, simulation et copie, sont
contrôlées. Les modifications des horloges et des services des deux copies
sont préparées avant toute écriture, puis écrites et relues avant de reprendre le jeu.
Le setter refuse de suspendre son propre processus : utiliser un client externe.
Les sessions d'observation ordinaires restent en lecture seule.

Si une écriture échoue, la restauration de toutes les valeurs touchées est tentée avant la
reprise. `ClockWriteFailed` impose de relire l'horloge : une erreur ne garantit
pas que la restauration a réussi. L'API ne fournit pas de commande native
officielle de changement de date.

## Premier essai du 16 septembre 2026 — validation insuffisante

Cet essai initial vérifiait uniquement l'affichage et la lecture des données.
Il n'avait pas vérifié les départs après une nouvelle arrivée. Le code initial
laissait les services en 2026, entraînant ensuite des attentes de 86 ans.
Les résultats ci-dessous ne validaient donc pas la circulation.

- Partie ouverte, PID 30864, 381 trains ; date initiale 12 août 2026.
- Avant : `2026-08-12T16:47:36.190Z`, origine `1781123300`.
- Appel demandé : `1940-08-14T16:48:00Z` ; retour `.790Z`.
- Relecture : `1940-08-14T16:48:01.240Z`, origine `-932614276`.
- Interface du jeu : mercredi 14 août 1940, 17:48:09, fuseau affiché +1.
- Observation complète réussie : 381 trains, vitesses, voyageurs, arrêts,
  délais de départ et occupations de quais lisibles ; horloge en progression.
- Retour vérifié par SDK : `2026-08-12T20:01:03.650Z`, origine initiale
  `1781123300` rétablie, compteur écoulé conservé à `5441563650` ms.

Cette vérification démontre le changement effectif et la lecture continue,
pas la cohérence de tous les horaires futurs après un saut arbitraire.
Les tests automatisés couvrent les dates négatives, les bornes, les lectures
instables, la conservation des ticks et délais, et l'immuabilité des snapshots.

## Correction et nouvel essai

Le setter translate désormais `TimedStop.departure_epoch_s` (Motion +0x4B8),
`RunDispatch.run_epoch_start` (+0x5C0) et le contrôle calendaire des voyageurs
(+0x630, sauf valeur initiale zéro), pour la simulation et sa copie. Les champs
d'optionals inactifs restent intacts. Les échéances en microsecondes ne changent pas.

La cause a été reproduite : pour le train 1407374883553281, après une arrivée
avec l'ancien code, le départ restait `1786572360` (2026) tandis que l'origine
était `-932615164`. Le moteur calculait alors `2719187524000000` microsecondes
comme échéance. La lecture de la date seule ne pouvait pas détecter ce défaut.
Le calcul natif est visible dans `research/reports/clock-offsets/140443200.c` ;
le blocage sur l'échéance est confirmé par `140449e40.c`.

Après rechargement de la partie (nouveau PID 42744), essai du correctif :

- Passage au 14 août 1940, 16:50 UTC ; origine `-932614187`.
- Après 2 487,82 secondes de simulation : 381 trains, dont 150 en conduite,
  66 initialement arrêtés maintenant en conduite et 92 nouvelles arrivées.
- Aucun train arrêté avec une attente supérieure à un an dans ce relevé.
- Le train 1407374883553281 est observé en gare avec 23,77 secondes restantes,
  puis de nouveau en conduite à 294,31 km/h, arrêt suivant d'index 9.
- Retour exact à l'origine `1781123300`, compteur préservé à `543946391` ticks.

Cette correction empêche les nouveaux départs d'être calculés dans l'ancien
calendrier. Elle ne répare pas automatiquement une sauvegarde dont les échéances
ont déjà été faussées par l'ancien setter : recharger une sauvegarde antérieure
à cet essai avant d'utiliser la version corrigée. La partie rechargée utilisée
pour la validation ne contenait aucune de ces échéances faussées.

## Intervention native : essai du 16 septembre 2026

Partie PID 32448 : 381 trains, dont 36 remisés. Validation du point d'entrée
avec CLion sur un client externe : callbacks sur le thread 10088, adresses Sim
et DB concordantes avec les racines lues indépendamment. Le jeu reste actif
pendant la pause du client dans le débogueur.

- Saut d'une heure : 345 interventions, nouveaux débuts de service, puis
  9 trains observés en circulation après leurs attentes.
- Version installée et nouvelle API : `1940-08-14T16:50:00.170Z`, 345 interventions.
  Deux relevés successifs passent de 1 à 35 trains en circulation ; 34 redéparts,
  aucune attente supérieure à un an.
- Le jeu affiche le BB26024 + Corails au dépôt de Francfort, avec environ
  3 h 45 restantes, alors qu'avant le changement il était à Kaiserslautern.
  Le SDK lit la gare `562950066798594`, voie `281491575341058` et 13 553,6 s
  restantes. Un dépôt visible est actuellement classé « stopped at station »
  par l'observateur ; le libellé « at depot (hidden) » concerne les dépôts masqués.
- Retour au calendrier initial : `1965-08-13T22:06:48.030Z`, nouvelle intervention
  native et 193 localisations différentes du relevé en 1940. Cette restauration
  du calendrier ne restaure pas les positions, voyageurs ou dépenses d'avant test.
