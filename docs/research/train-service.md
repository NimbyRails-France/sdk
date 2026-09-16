# États et horaires des trains — SDK 0.6.3

Lecture externe uniquement, binaire reconnu SHA-256
`fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae`.
Les adresses suivantes sont des RVA de recherche, pas des points d'injection.

## Sources natives

`0x804000` affiche les états optionnels de Motion : Presence `+0x1D0`,
Hitch `+0x1F0`, Blackhole `+0x218`, Drive `+0x4B0`, TimedStop `+0x4D0`,
ScheduleStop `+0x4F0`, RunStop `+0x510`, StationStop `+0x558`,
DispatchCooldown `+0x598`, RunDispatch `+0x5D0`, ScheduleDispatch `+0x5F0`.
Les anciens octets d'un optional désengagé ne constituent pas un état.

- `0x43E130` détermine l'alerte : 1 ligne fermée, 3 trajet introuvable,
  4 ordres incorrects, 5 collision, 6 attente au signal, 7 horaire fermé,
  8 voies de dispatch occupées, 9 aucun service activé, 10 services déjà affectés.
- `0x804000` sélectionne `Blackhole+0` (`Motion+0x1F8`) pour la localisation
  cachée, sinon `Presence+0` (`+0xB8`). Drive fournit également sa position
  à `+0x3A8`. Le mode 2 du modèle Train à `+0xB8` affiche « Mothballed ».
- `0x43E9A0` et `0x804000` utilisent RunDispatch : ID de ligne `+0x5A8`,
  index d'arrêt signé `+0x5C8`. `0x33F7F0` retrouve la ligne par ID complet,
  tag 4 et pas `0x280`, dans `DB+0x180`.
- `0x805710` lit le libellé de ligne à `+0x78`, le type à `+0xFC` (1 dépôt),
  le vecteur d'arrêts à `+0x118/+0x120`, pas `0x158`, et la voie cible de
  l'arrêt à `+0x78`. L'ID de gare observé à `stop+0x110` est obligatoirement
  recoupé avec la gare de cette voie dans le snapshot public.
- `0x58C400` lit l'arrivée cible à `Motion+0x488` si Drive existe, ou le
  départ à `+0x4C0` si TimedStop existe. Il soustrait `Clock+8 * 10000` :
  compteurs de 10 ms, échéances en microsecondes. Son appel à `0x804D02`
  passe `simulation+0x20` comme Clock (désassemblage dans le rapport).
- L'échéance de nouvelle tentative de dispatch est à `+0x560`.
  Ce délai n'est **pas** un horaire de départ commercial.

Le premier entier de Clock (`simulation+0x20`) est l'origine du calendrier
du jeu en secondes. Sur cette partie : 1624876580. Additionné au compteur
de simulation, il correspond au 13 octobre 2033 et à l'heure visible dans
l'interface. Les échéances du SDK sont exposées dans les deux formes :
microsecondes de simulation et conversion vers le calendrier du jeu.
Ne pas les interpréter comme des heures réelles de l'ordinateur.

## Vérification réelle

PID 31988, 16 septembre 2026. Débogueur CLion arrêté dans
`printSnapshot`, appelé par `main`, train 1407374945550337 : nom
`TER X 72011`, flags 7, vitesse mesurée 27.7683540973 m/s. Aucun débogueur
n'a été attaché au jeu ; les données natives proviennent de lectures externes.

Sonde native indépendante : 924 Motion retrouvés, dont à cet instant
534 en DispatchCooldown sans Presence, 37 en arrêt avec Presence et
TimedStop, 352 avec Drive, et 1 avec Blackhole et arrêt sur une ligne dépôt.
La répartition varie pendant la simulation.

API installée : 924/924 états résolus. Exemples d'un même relevé :

- TGV 201 : attente de dispatch, nouvelle tentative dans 1.94 s de jeu.
- Y9001 : dépôt masqué, ligne D-181.2, gare DEP INFRA, départ 12:44:30,
  délai 211.86 s de jeu.
- RER Z20503 : arrêt à Châtelet - Les Halles, départ 12:41:35,
  délai 36.86 s de jeu.
- TER X 72011 : conduite vers Saint-Pierre-des-Corps, arrivée dans 16.86 s.

Contrôle UI supplémentaire sur TER B 84904 : le jeu montre Part dieu F,
« Arrives in 01:21 » à 12:42:34 ; la lecture SDK suivante montre Part dieu,
72.29 s restantes, cohérent avec les secondes écoulées entre les captures.
Le SDK ne fournit pas encore le suffixe de quai « F ».

Validation finale : 13/13 tests CTest en Debug et 13/13 en Release,
contrôle public sur 924/924 états et 3 342/3 342 noms de gare.
L'observer automatique a été exécuté depuis CLion (code de sortie 0).
Sa DLL est identique à celle de `install/Release`, version 0.6.3 / ABI 1.
Les tests couvrent notamment les octets résiduels des optionals, l'horloge
absente, les échéances dépassées, un état modifié pendant la lecture,
un ID de ligne d'une autre génération et les copies sans écriture partielle.

## Contrat et limites

`NimbyTrain` et ABI 1 restent inchangés. `NimbySdk_CopyTrainServices`
ajoute un enregistrement séparé par train et des bits de validité indépendants.
Le lecteur relit les états utilisés, l'ID complet, le mode du modèle,
les tables et les enregistrements d'arrêt. Les références de voie et gare
sont recoupées dans le snapshot. Une lecture manquante reste inconnue.

« Au dépôt » exige Blackhole, RunStop, une ligne de type dépôt et une gare
de localisation correspondant à l'arrêt. Une simple absence de Drive ou de
Presence ne suffit pas. « Arrêt en gare » exige Presence, TimedStop,
RunStop et la même concordance de gare. Le bit StationStop peut subsister
pendant Drive : il ne prouve donc pas à lui seul que le train est arrêté.

Le départ fourni est l'échéance active de TimedStop, pas une garantie de
mouvement à cette seconde. Pendant Drive, seule l'arrivée active est lue.
La grille complète des départs futurs, les numéros de quai, le retard calculé
par le moteur et le nom du service horaire ne sont pas encore exposés.
Un delta d'arrivée négatif est un dépassement de l'échéance lue, pas le
retard prédictif natif. Un train hors voies sans emplacement valide ne reçoit
aucune gare inventée. Ces lectures restent expérimentales et non atomiques.
