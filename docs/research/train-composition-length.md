# Longueur des compositions — audit Windows

État au 9 octobre 2026 : recherche native statique en lecture seule, suivie du
helper de groupe et de tests synthétiques autorisés. Les observations ci-dessous
ne constituent pas une validation dans le jeu. Aucun exécutable, aucune partie
et aucun projet Ghidra n'ont été modifiés par cet audit.

La demande BC Train Super Long utilise une longueur maximale, **850 m par
défaut**, configurable dans Options → NRF Hub. La demande antérieure de 81
voitures est abandonnée. Une insertion dépassant la longueur doit être refusée
avant la première modification, avec la longueur proposée et la limite affichées.

## Provenance

Binaire Windows `1.19.10.5bfaea3`, SHA-256 :

```text
fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae
```

Exports de travail :
[.validation/bc-train-composition-20261009](../../../.validation/bc-train-composition-20261009).
Ils contiennent les instructions, le pseudo-C, les références et la provenance.
Ghidra 12.1.3 a ouvert `NimbyRailsResearch` avec `-readOnly -noanalysis`.
`InspectConstruction.java` refuse un autre SHA-256. Les adresses ci-dessous sont
des RVA ; les fichiers exportés utilisent la VA avec base `0x140000000`.

La recherche globale de comparaisons à 30 est une recherche de candidats. Elle
exclut certaines références de pile et ne prouve pas l'absence d'une autre borne.

## Bornes retrouvées

| RVA | Observation | Conséquence |
| --- | --- | --- |
| `0x3e33f0` | Validation des compositions : code 1 si plus de 30 voitures | La limite masque aussi un éventuel refus d'attelage déjà calculé |
| `0x7eecd0` | Quantité du bouton Ajouter : de 1 à 30 par action | Cette quantité ne borne pas le total après plusieurs ajouts |
| `0x3e16b0` | Fabrique d'unités : deux branches arrêtent silencieusement l'expansion à 30 | Modifier la validation seule ne supprime pas cette troncature |
| `0x44cf80` | Agrégation des trains attelés : ignore un ajout si le total dépasse 30 | Chemin indépendant de l'achat et de l'éditeur |
| `0x3e0d90` | Statistiques physiques de toutes les voitures ; empreintes individuelles stockées uniquement pour les 30 premières | Le hash final lit au-delà du tableau de pile pour 31 voitures connues ou davantage |
| `0x405560` | Ancien format de modèle : `min_cars` et `max_cars` plafonnés à 30 | Borne de recette/modèle, distincte de la règle de longueur |
| `0x4070c0` | Conversion de ce format vers les parties d'une unité : répétitions plafonnées à 30 | Ne pas modifier ces constantes sans qualifier la conversion complète |

La comparaison à 31 dans `0x44d6b0`, VA `0x14044ef30`, contrôle l'alignement d'une
allocation avant `free` ; elle ne concerne pas le nombre de voitures.

`maximumCarsPerTrain = 4096` dans le lecteur SDK est une borne de lecture. Elle
ne démontre pas qu'une composition de 4096 voitures est prise en charge par
toutes les fonctions du jeu. Une borne interne contre les données corrompues
doit rester distincte de la limite métier exprimée en mètres.

## Lecture des modèles et de la longueur

Un `CarSetup` fait 32 octets. Son identifiant de modèle se trouve à `+0`.
Le vecteur est dynamique : début, fin et capacité aux offsets `+0xc0`, `+0xc8`,
`+0xd0` d'un `Train`, ou `+0`, `+8`, `+16` d'un `Dynamics`.

Dans ces routines natives, `Rules` possède la table de modèles à `+0x38` et le
nombre de seaux à `+0x40`. Un nœud possède l'identifiant à `+0`, le lien suivant
à `+0x2a0` et la longueur physique, un `float` en mètres, à `+0xfc`. Le seau à
l'index `bucketCount` contient la sentinelle.

`0x3e0d90` additionne ces longueurs dans `Dynamics+0x38`, en `float`, et ignore
les modèles absents. Une politique SDK doit distinguer un modèle absent ou une
longueur non finie/non positive d'une composition correctement mesurée. La
longueur du candidat se calcule à partir de ses modèles, jamais à partir d'un
champ `Dynamics` datant d'une composition précédente. Le choix des arrondis à
la frontière de 850 m doit faire l'objet de tests explicites.

Pour les exécuteurs de commandes, `ModelContext+0x890` est un sous-objet
`ModelStore` : `DB = *(ModelContext+0x890)`, puis `Rules = *(DB+0x408)`.
Le catalogue à `DB+0xa80` utilisé par d'autres lecteurs SDK n'est pas ce pointeur.

## Ajout atomique dans l'éditeur

Le point commun aux boutons et au clic avec Maj est `0x7ef7d0` :

```text
void commit(Editor* RCX, NativeContext* RDX, Rules* R8)
```

Il agit seulement quand `Editor+0x15d0` est non nul. Les compositions à modifier
sont dans le vecteur `Editor+0xc0`. Les deux contrôleurs appelants, `0x7fabb0`
et `0x7fd860`, utilisent un sous-objet temporaire stable à `Controller+0xe68`.
Le contrôleur de reconfiguration remet son indicateur d'action à zéro au début
de l'image, puis les contrôles l'arment. Refuser avant `original` conserve donc
la composition et la sélection sans créer de nouvelle allocation native.

L'ordre natif doit être respecté : disposition, décor, orientation, suppression,
déplacement si sélection non vide, ajout, remplacement, sélection, copie,
collage, réinitialisation. Un ancien flag d'ajout ne doit pas bloquer une
suppression ou un changement de décor.

| Action | Candidat à mesurer avant `original` |
| --- | --- |
| Ajout `+0x15c4` | Composition actuelle et `Editor+0x15e0` répété `int32(Editor+0x1600)` fois |
| Remplacement `+0x15c5` | Composition actuelle avec le `CarSetup+0x15e0` aux indices sélectionnés valides |
| Collage `+0x15c9` | Composition actuelle et le vecteur `+0x1608/+0x1610/+0x1618` |
| Réinitialisation `+0x15cb` | Vecteur du `Train` modèle situé **en ligne** à `Editor+0x178`, soit début/fin `Editor+0x238/+0x240` |

L'arbre des indices sélectionnés possède en-tête `+0x1118`, racine `+0x1128`,
compteur `+0x1138`. Un nœud a enfants `+0/+8`, parent `+16`, index `int32+32`.
La lecture doit être bornée et vérifier le nombre de nœuds visités.

Le collage natif ignore les modèles inconnus. Une prévalidation SDK qui refuse
un modèle inconnu est volontairement plus stricte : le message doit expliquer
qu'une mesure fiable est impossible, au lieu d'annoncer un dépassement.

L'insertion basse `0x3e2e40` effectue une voiture à la fois et recalcule
`Dynamics` à chaque passage. La détourner seule provoquerait un ajout partiel
dans une opération multiple. La prévalidation commune à `0x7ef7d0` couvre
l'opération entière et doit rester proportionnelle au candidat, sans callback
de mod sur le thread du jeu. Les grands ajouts natifs répétés conservent leur
coût de recalcul ; cet audit ne prouve pas leur optimisation.

Le panneau `0x7ed720` reçoit un émetteur UI emprunté dans R8. Le SDK peut
y afficher un refus durant les passes de mise en page et d'interaction, sans
boîte de dialogue Windows. La durée de vie du message et la position dans les
groupes UI doivent être vérifiées dans le jeu. Le texte natif « trop de voitures »
ne convient pas à une limite exprimée en mètres.

## Validation et préservation des attelages

`0x3e33f0` retourne son pointeur résultat :

```text
Result* validate(Train*, Result*, Rules*, uint8 bypassCouplers)
Result : uint32 code +0 ; vector<uint16> begin/end/capacity +8/+16/+24
```

Chaque voiture possède deux octets de compatibilité, avant et arrière. Le
résultat initial vaut zéro, une incompatibilité donne le code 4, une composition
vide donne 2. Le contrôle final `count > 30` écrase le code avec 1. Le code 3
est affiché par certaines interfaces, mais n'est pas produit par cette fonction.

Après une validation native de plus de 30 voitures, si seule la borne de nombre
doit être levée, les flags déjà calculés permettent de récupérer l'autre refus :
un octet nul implique le code 4, sinon le code 0. Le vecteur doit être conservé
avec son allocation native. Le bypass d'attelage et le cas d'un modèle absent
remplissent tous deux `0x0101` ; une mesure SDK peut refuser ce dernier cas
séparément. Il ne faut pas transformer arbitrairement chaque code 1 en succès.

## Achat et reconfiguration réels

Les wrappers de commandes sont `0x304b50` (achat) et `0x304e00`
(reconfiguration), avec cette signature vérifiée dans les instructions :

```text
Result* execute(Command* RCX, Result* RDX, ModelContext* R8, SimState* R9)
```

`DB = *(ModelContext+0x890)` et `Rules = *(DB+0x408)`. Le quatrième argument
est le **SimState direct** : `0x304e00` conserve R9 dans RSI, écrit l'époque
à `RSI+0x2120` et transmet `RSI+0x23e0` au calcul de trésorerie. Il ne faut
pas le traiter comme le contexte à deux pointeurs des commandes SimCmd.

L'achat porte un `Train` en ligne à `Command+0x20` : composition à
`Command+0xe0/+0xe8/+0xf0`. La reconfiguration porte un identifiant de train à
`Command+0x20` et un vecteur de `CarSetup` en ligne à `+0x28/+0x30/+0x38`.

Le wrapper d'achat appelle `0x3e4ae0`, qui calcule les coûts, crée les trains et
recalcule les données sans appeler le validateur des compositions. La
reconfiguration appelle `0x3e83e0`, qui valide le candidat avant les coûts et la
modification. Un hook du seul validateur ne couvre donc pas l'achat.

Dans `0x3e83e0`, la Train locale est initialisée à `RSP+0x40` par `0x322510`.
`0x341ea0` copie ensuite le candidat dans son vecteur à `RSP+0x100`, avant
le validateur à `0x3e8489`. Les trois branches de copie emploient `memmove`
avec un stride de 32 octets ; ni les IDs, ni l'ordre, ni les paramètres de
CarSetup ne sont normalisés à ce stade. Une autorisation de réduction déjà
vérifiée peut donc correspondre au candidat local par ses modèles ordonnés,
son nombre, son pointeur Rules et le même instantané de limite, sans
autoriser une autre composition.

Les wrappers constituent une frontière de refus simple : avant la copie de
vecteur, avant les coûts, avant les changements d'objets et avant leurs compteurs.
Les deux fonctions internes consomment puis libèrent un vecteur alloué même en
cas de refus ; une sortie anticipée à leur niveau devrait gérer cette propriété.

Les retours natifs de refus établissent ces écritures scalaires précises :

| Wrapper | Écritures dans le pointeur `Result*` retourné |
| --- | --- |
| Achat | `uint32+0 = 2`, `uint64+8 = 0`, octets `+0x10..+0x1f = 0`, puis `byte+0x10 = 1`, `byte+0x1f0 = 4` |
| Reconfiguration | Octets `+0..+0xf = 0`, `uint64+0x10 = 0`, `uint32+0x18 = 2`, `byte+0x1c = 1`, `byte+0x1f0 = 4` |

Le padding `+4` de l'achat n'est pas significatif dans la routine native.
Le dispatcher `0x30e020` fournit la zone de `0x1f8` octets, visite le tag
`+0x1f0` et ne détruit pas d'allocation pour le tag 4. Il construit ensuite le
delta et la corrélation ailleurs dans son enveloppe. Un refus doit reproduire
ces écritures, sans écraser les autres régions de l'enveloppe.

## Empreinte sûre au-delà de 30 voitures

`0x3e0d90` retourne `void` et reçoit `(Dynamics*, Rules*)`. Il calcule les
statistiques physiques pour chaque modèle reconnu. Son tableau d'empreintes
est limité à 30 entrées à `RSP+0x70`, mais le nombre de modèles reconnus continue
d'augmenter. L'appel à `0x2711c0`, VA `0x1403e1150`, reçoit ce nombre multiplié
par huit. Pour 31 modèles connus ou davantage, l'appel lit au-delà du tableau.
Augmenter la comparaison à 30 dans cette fonction provoquerait également des
écritures au-delà du tableau. Cette correction est un prérequis à la levée du cap.

Un détournement complet de `Dynamics` peut installer un contexte temporaire
sur le thread, puis laisser le calcul physique natif s'exécuter. L'appel agrégat
`0x2711c0` peut utiliser une liste d'empreintes appartenant au SDK quand ce
contexte indique une grande composition. Il n'y a qu'un appel à cet agrégat dans
`Dynamics`; le calcul de décor `0x3f8720` appelle `0x2416b0` et `0x3802d0`,
sans réentrer dans `0x2711c0`. Toute reconstruction doit désarmer son propre
contexte pendant ses appels auxiliaires.

Les signatures de hash vérifiées sont :

```text
uint64 aggregate(const void* bytes, uint64 byteCount)              // 2711c0
uint64 leaf(const void* bytes, uint64 totalBytes, uint64 seed,
            uint64 unusedFourth, uint64 remainingBytes)            // 2416b0
uint64 decor(uint8 a, uint32 b, uint32 c,
             uint8 d, uint8 e, uint8 f, uint8 g)                    // 3f8720
```

Le cinquième argument de `leaf` est lu à `[RSP+0x28]` : le pseudo-C ne
l'affiche pas à chaque appel. `decor` et `aggregate` sont parfois décompilés en
`void`; les instructions prouvent le résultat dans RAX. Le hash résultant de
`Dynamics` est stocké à `+0x68`.

Une empreinte par voiture combine, dans l'ordre natif, le `uint16+0x10`, le
hash décor, le `uint64+8`, puis l'identifiant de modèle `uint64+0`. Chaque
combinaison est un couple de deux `uint64`, avec la graine
`0x9c3805fc2c85cacc`. Le décor utilise les champs `+0x12`, `+0x14`, `+0x18`
et les octets `+0x1c..+0x1f`. Il faut conserver tous ces champs et l'ordre de la
composition. Les modèles absents sont exclus de la séquence native.

**Conservation des registres :** les hash natifs n'utilisent aucun XMM.
`Dynamics` utilise encore XMM2 après l'appel agrégat, à `0x3e119f`, comme zéro.
Une entrée de détournement C++ ordinaire peut le modifier. Un détournement global
de l'agrégat doit préserver XMM0..XMM5 autour de son helper, ou cibler un appel
dont tous les registres vivants sont qualifiés. Un simple contexte de thread ne
résout pas cette contrainte du code natif optimisé.

Le hash sûr doit rester disponible lors du retrait du propriétaire de la règle
de longueur : des trains déjà sauvegardés avec plus de 30 voitures existent
encore. Le chargement de Motion (`0x450290`), l'application d'un delta
(`0x3f2210`), le décor, les déplacements et les suppressions recalculent aussi
`Dynamics`. Désactiver la politique ne permet pas de restaurer un hash dangereux.

## Attelage, unités et sauvegarde : limites de portée

La commande d'attelage `0x441070` reçoit `(Command*, SimContext*)`, retourne
`void`, résout `SimState = *(SimContext+8)` puis les Motion dans la table
`SimState+0xa0` via `0x45f300`. Elle écrit le pilote `+0x1d8`, l'ordre `+0x1e0`,
l'orientation `+0x1e8` et le flag `+0x1f0` de la Motion attelée. Elle ne vérifie
ni la longueur ni le nombre. Le pilote se trouve à `Command+8`, le train attelé
à `+0x10`, l'ordre à `+0x18`, l'orientation à `+0x20`.

La routine d'agrégation `0x44cf80` reconstruit la composition courante du pilote
et des Motion attelées, dans leur ordre. Lever son cap exige la prévalidation
du groupe entier avant une nouvelle demande d'attelage, avec les cycles,
orientations et masques de voitures actives. Une baisse de limite ne doit pas
omettre silencieusement des voitures déjà attelées. Ce chemin n'est pas couvert
par une validation de composition manuelle et d'achat seule.

La fabrique `0x3e16b0` remet la destination à zéro dès l'entrée. Un refus doit
donc intervenir avant `original` et libérer la recette dont elle devient
propriétaire. Les répétitions négatives malformées sont dangereuses si les caps
sont retirés. Lever les deux branches à 30 demanderait d'abord un précontrôle de
toute la recette, des modèles, des répétitions et de l'allocation ; une constante
`imm8` ne peut pas recevoir 4096. Ce travail est distinct de l'éditeur manuel.

La sérialisation du vecteur `0x31d180` écrit un nombre `uint64` puis chaque
`CarSetup`. La désérialisation `0x31d230` redimensionne un vecteur dynamique,
sans cap local à 30. Cela établit que le format du vecteur peut représenter une
composition plus grande ; cela ne valide pas son cycle sauvegarde/chargement ni
ses autres usages dans le jeu. Une baisse de la limite ne doit jamais tronquer
une rame existante.

## Vérifications nécessaires avant activation

- Comparer les hashes pour 0 à 30 voitures, en variant tous les champs du
  `CarSetup`; vérifier qu'une modification après la trentième change le résultat.
- Vérifier le calcul physique des voitures motorisées et non motorisées après
  le détournement, particulièrement la conservation de XMM2.
- Ajouter, coller, remplacer et réinitialiser à la limite et au-delà : aucun
  changement de composition/sélection sur refus, y compris clic avec Maj et
  opérations multiples.
- Conserver la possibilité de supprimer/diminuer une rame existante au-delà
  d'une nouvelle limite, et conserver ses incompatibilités d'attelage natives.
- Vérifier achat groupé et reconfiguration réelle : aucun coût, création ou
  modification sur refus, avec un résultat natif correctement consommé.
- Sauvegarder puis recharger plus de 30 voitures, modifier leur décor, annuler,
  retirer le propriétaire de politique et vérifier la sécurité du hash.
- Refuser proprement modèles absents, longueurs invalides, vecteurs et arbres
  corrompus, nouvelle version du jeu et dépassement des bornes de sécurité.
- Tester séparément les unités et attelages si leurs caps doivent être levés ;
  tant qu'ils restent natifs, ne pas annoncer qu'ils sont couverts.

Ces vérifications sont un plan de validation, pas des tests exécutés par l'audit.

## Qualification du contrôle d'attelage (lecture seule, 9 octobre 2026)

Les exports utilisés se trouvent dans
`.validation/bc-train-composition-20261009/hitch-gate`, `physical-routes`,
`purchase-hitch`, `append-action` et `config-gate`. Aucune commande n'a été
exécutée dans le jeu et aucun binaire, source de passerelle ou sauvegarde n'a
été modifié par cet audit. Les offsets ci-dessous concernent exclusivement le
SHA-256 cité au début de ce rapport.

### ABI et refus avant mutation

La table `SimCmd::TrainHitch` à `0x140a74600` pointe son slot `+8` sur
`0x140441070`. Le contrat est `void(Command*, SimContext*)`. Il ne s'agit pas
du résultat variant des commandes du modèle. La commande contient le pilote
à `+8`, le train attelé à `+0x10`, l'ordre `int64` à `+0x18` et l'orientation
`uint8` à `+0x20`; sa taille native est `0x28`.

Le contexte contient le DB **direct** à `+0` et le SimState à `+8`.
`0x44d6b0` conserve son argument RDX, le place en première entrée du contexte
et place R8 en seconde entrée avant les appels virtuels `0x44e5aa` et
`0x44eaea`. Il repasse le même DB directement à `0x44cf80`. Ainsi :

| Objet | Emplacement |
| --- | --- |
| DB | `*(SimContext+0)` |
| SimState | `*(SimContext+8)` |
| Rules | `*(DB+0x408)` |
| Collection Train | `DB+0x200` |
| Collection Motion | `SimState+0xa0` |

`0x441070` résout d'abord le train attelé dans cette collection via
`0x45f300`. Il ne fait rien si celui-ci est absent, ou si Motion `+0x5f0`
(association à un horaire) ou `+0x5d0` (marche active) est non nul. Ensuite,
un pilote absent provoque seulement la remise à zéro du flag attelé
`Motion+0x1f0` : cette désassociation doit rester permise quand la limite est
abaissée. Un pilote présent entraîne les écritures `+0x1d8`, `+0x1e0`,
`+0x1e8` puis `+0x1f0=1`. Un refus doit simplement retourner **avant**
l'original : aucun résultat natif à construire, aucune allocation à détruire,
aucune composition ou attache à restaurer.

### Groupe effectif et commandes successives

`0x45cd50` normalise les attaches en étoile, à un seul niveau. Pour une Motion
non attelée, il enlève les enregistrements enfants absents, détachés ou dont
le pilote a changé. Pour une Motion attelée, il vide sa propre liste d'enfants
et annule son attache si son pilote est absent **ou lui-même attelé**. Sinon,
il ajoute la Motion à la liste de ce pilote si elle n'y figure pas encore.
Les champs qualifiés sont :

| Champ Motion | Offset/type |
| --- | --- |
| Identifiant complet | `+0`, `uint64` |
| Composition courante | `+8/+0x10/+0x18`, vecteur `CarSetup` |
| Configuration active | `+0x80`, `int32` borné à 0..8 |
| Liste des attaches | `+0x88/+0x90/+0x98`, stride 32 |
| Pilote déclaré | `+0x1d8`, `uint64` |
| Ordre | `+0x1e0`, `int64` |
| Orientation | `+0x1e8`, `uint8` |
| Flag attelé | `+0x1f0`, `uint8` |

La liste d'attaches n'est **pas** une source suffisante au moment de la
commande. `0x44d6b0` appelle la normalisation à `0x44d751`, puis applique les
commandes en boucle, puis agrège les compositions à `0x44ecc9`. Plusieurs
commandes d'attelage de la même phase modifient déjà les champs pilote/flag
alors que la liste `+0x88` n'a pas encore reçu ces nouveaux membres. Un
précontrôle qui additionne seulement cette liste peut accepter plusieurs
ajouts individuellement puis dépasser 850 m.

Le contrôle sûr de cette phase doit donc regarder les champs pilote/flag des
Motion de la collection, y compris les commandes précédentes, avant de
constituer le candidat. Aucun parcours périodique n'est nécessaire : ce
parcours intervient uniquement lors d'un ajout d'attelage. La collection
utilise les blocs `+0x18/+0x20/+0x28`, le shift `uint32+4`, le nombre de slots
`uint32+8`, le masque `uint32+0x10` et un stride Motion `0x638`. Le résolveur
`0x45f300` exige le tag d'identifiant 5 et une égalité de l'identifiant complet
dans le slot : un identifiant réutilisé ne doit pas être accepté par son seul
index. Les limites et vérifications de collection existantes du SDK donnent
un cadre de travail, sans prouver un coût faible pour un parcours de tous les
slots d'une immense sauvegarde.

Un candidat conservateur contient le pilote (non attelé), tous les membres
valides dont le pilote déclaré est ce pilote, puis le nouveau membre une seule
fois. La source doit être vérifiée aussi : un train candidat qui conduit déjà
d'autres attaches, un pilote lui-même attelé, un cycle, un membre non résolu ou
une topologie ambiguë ne donnent pas la preuve requise pour ajouter. Refuser
une nouvelle attache laisse le groupe existant intact. Une désassociation ou
un simple changement d'ordre/orientation d'un membre déjà attaché au même
pilote ne doit pas supprimer sa composition quand la limite vient de baisser.

### Longueur, masques et cap d'agrégation conservé

`0x44cf80` obtient la composition configurée de la Train dans `DB+0x200`.
Le vecteur est `Train+0xc0/+0xc8/+0xd0`, stride 32. Pour le pilote, le mode
Motion `+0x80` est borné à 0..8. Mode 0 inclut toutes les voitures ; les modes
1..8 incluent une voiture si son octet `CarSetup+0x1f` contient le bit
`1<<(mode-1)`. Pour les membres attelés, le jeu agrège leur composition
courante, déjà filtrée. Le contrôle doit appliquer le même masque à la
composition configurée de **chaque** membre pour ne pas dépendre d'une
composition courante retardée par une commande de configuration.

La copie du pilote s'effectue directement depuis son vecteur configuré,
sans appeler l'usine d'unités `0x3e16b0`. Une rame manuelle seule de plus de
30 voitures courtes n'est donc pas tronquée par le cap de cette usine ; elle
requiert toujours la correction d'empreinte décrite plus haut.

L'ordre et l'inversion ne changent pas la longueur ; un modèle inconnu ou une
longueur non finie/non positive empêche cependant la preuve. Il faut mesurer
le candidat entier avant le premier appel natif. Deux trains de 500 m et
10 voitures chacun passent le cap natif de nombre tout en dépassant 850 m :
laisser ce cap ne suffit donc pas au contrôle physique demandé.

Le cap d'agrégation est un garde distinct, conservé dans cette première portée.
`0x44cf80` ajoute un enfant seulement si le total de voitures courantes est
strictement inférieur à 31. Le pilote, lui, est copié sans ce cap. Le tri peut
placer des enfants avant le pilote : laisser la routine décider du cap peut
omettre un enfant ou produire un résultat dépendant de l'ordre. Tant que cette
branche reste native, le contrôle d'une **nouvelle** attache doit vérifier à la
fois la longueur et un total agrégé de 30 voitures au plus. Cette restriction
n'est pas la limite métier de composition manuelle et ne doit pas être
présentée comme une levée complète des limites d'attelage.

### Voies complémentaires identifiées, pas encore couverture générale

`SimCmd::TrainSetConfig`, slot `+8` de la table `0x140a744f8`, appelle
`0x440f50` avec `void(Command*, SimContext*)`. La commande porte l'ID Train à
`+8` et le nouveau mode `int32` à `+0x10`. Elle remet Motion `+0x78` à zéro et
écrit le mode borné à 0..8 à `+0x80`. Elle peut donc augmenter la longueur
effective d'un groupe **après** la validation initiale de l'attelage. Une
recomposition d'un membre peut aussi agrandir son groupe si seule sa
composition individuelle est mesurée. Une garantie globale « aucun train
assemblé ne dépasse la limite » exige un contrôle du groupe candidat sur ces
voies, en plus du contrôle d'attelage. Aucune mutation tardive de l'agrégation
ne doit tronquer silencieusement un train existant.

Tests synthétiques pertinents pour ce garde : deux 500 m refusés ; 400+450 m
acceptés ; plusieurs demandes de la même phase cumulées ; reparentage sans
double comptage ; mode actif et mode proposé ; références absentes, cycles et
pilote déjà attelé refusés ; baisse de limite conservant les attaches existantes
et permettant leur séparation ; cap d'agrégation distinct à 30 ; composition
et champs Motion inchangés sur refus. L'intégration et le cycle natif dans le
jeu ne sont pas encore validés par cet audit.

### Helper de groupe et validation synthétique effectuée

À la demande du coordinateur, l'audit a ensuite créé seulement
`include/platform/windows/train_length_group.h` et son test
`tests/windows/train_length_group.cpp`. Le helper reçoit explicitement les
pointeurs DB/SimState et le callback de lecture ; il ne lance aucune fonction
du jeu et n'écrit aucune mémoire étrangère. Les opérations disponibles sont
`groups::hitch`, `groups::setConfig` et `groups::recompose`. Chaque résultat
expose l'ID racine, l'ID modifié, les membres, les comptes actifs avant/après,
les longueurs mesurées et la première faute de lecture avec adresse/detail.
La recomposition vérifie à la fois la composition complète individuelle et
le groupe masqué. Une réduction de longueur reste possible après baisse de
limite, y compris si elle emploie plus de voitures courtes ; le cap natif
d'agrégation est évalué séparément.

Un garde supplémentaire couvre le décalage de `CurrentDynamics`. Le choix
de configuration et la recomposition peuvent précéder son rafraîchissement
dans la même phase de simulation. Pour chaque enfant, le helper utilise le
plus grand de sa longueur courante et de sa longueur configurée masquée. Il
établit ces bornes pour le groupe **avant et après** l'action, à partir d'un
seul instantané courant par enfant. Un pilote passant de 100 à 500 m avec un
enfant prévu de 100 m mais encore courant de 1 000 m est donc refusé : le
groupe passe de 1 100 à 1 500 m, même si la cible prévue est seulement 600 m.
Un refus dû à ce décalage expose `CurrentCompositionLag`, la cible prévue,
la borne physique et le nombre de membres décalés. Une réduction physique
prouvée reste possible après baisse de limite ; une recomposition ne coupe
jamais tardivement les voitures courantes pour obtenir cette preuve.

La longueur et le compte de voitures sont bornés séparément : le plus long
des deux vecteurs d'un enfant peut contenir moins de voitures que l'autre.
Le garde natif d'agrégation utilise donc la somme des comptes maximaux,
même si le calcul physique utilise les vecteurs de longueur maximale.

Chaque requête garde un cache possédé de 128 longueurs de modèles, commun
aux mesures de la composition entière, des groupes avant/après et des
vecteurs courants. Il ne persiste aucun pointeur du jeu. L'inventaire de
diagnostic contient au plus 128 modèles avec leur nombre de voitures, leur
longueur éventuelle et l'indication de mesure ; `omittedElements` compte
les voitures non enregistrées au-delà de cette capacité. L'observateur
réutilise les mesures nécessaires et n'ajoute aucun transfert mémoire pour
la télémétrie. Au-delà des 128 entrées du cache, les lectures redeviennent
directes sans changer la décision de sécurité.

Le test synthétique a été compilé avec g++ C++20, `-O2 -Wall -Wextra -Werror
-UNDEBUG`, puis exécuté avec succès. Il couvre les cas physiques, masques,
commandes d'attelage précédentes, reparentage, réduction, limite d'agrégation,
modèles absents/invalides, générations réutilisées et changements/refus de
lecture. Il couvre aussi le décalage physique/configuré lors de l'attelage,
du changement de profil et de la recomposition, les réductions conservatrices,
un compte courant supérieur au profil et la saturation de télémétrie à
129 modèles. Il vérifie l'absence de mutation du fixture et un seul transfert
groupé du bloc Motion par contrôle. CMake expose aussi
`native_train_length_groups`, passé sur le fixture. Ces résultats valident le
calcul et la lecture synthétique, pas les hooks en jeu, leur coût sur une
immense partie, ni le fonctionnement de la sauvegarde/chargement réelle.
