# Autorité centrale des itinéraires et remplacement des réservations natives

**Décision du 16 octobre 2026 : remplacement abandonné.** Ce document conserve
les recherches et leurs limites ; les tâches de remplacement décrites plus bas
ne constituent plus la feuille de route. Les réservations restent gérées par le
jeu. Le SDK conserve le suivi de conduite et les observations de passage,
d'occupation et de dégagement arrière déjà disponibles, sans transformer ces
observations en une autorité centrale. Les évaluateurs purs ne sont pas activés
comme système de réservation. Le travail courant porte sur le transport haut
niveau des occupants, les diagnostics de conflits et l'optimisation des captures
existantes. Voir [cantons et occupants](../blocks.md).

Recherche interne commencée le 9 octobre 2026, complétée les 10 et 11 octobre 2026.
Ce document distingue les mécanismes natifs
identifiés, les fondations présentes dans le SDK et le travail restant pour que
le SDK devienne l'autorité de réservation de tous les trains. Il ne décrit pas
une fonctionnalité déjà activée dans le jeu.

**État actuel du 11 octobre :** une copie native de la composition et du corps
physique courant est raccordée au diagnostic privé, sous une vraie propriété
exclusive et une lignée de monde monotone. Les points d'admission finale
`0x448210` et `0x447310` sont compilés et exercés par les fixtures du bridge.
**Aucune autorité centrale autonome n'est activée et aucune réservation native
n'est supprimée.** La photographie courante ne certifie pas la progression
historique permettant de libérer un bail. Voir la section de reprise du
11 octobre à la fin du document ; les sections datées du 10 octobre conservent
leurs conclusions et résultats historiques. La nouvelle capture et ses hooks
restent à valider dans une partie réelle.

**État historique du 10 octobre :** le moteur pur de baux et les évaluateurs de
couverture arrière sont testés ; les adaptateurs natifs supplémentaires restent
des observateurs de diagnostic, désactivés par défaut. **Un bail central obtenu
dans une fixture ne remplace aucune réservation native.** Aucun producteur
natif de couverture arrière complète ni autorité centrale de passage n'est
raccordé. La fenêtre locale de propriété est raccordée aux seuls diagnostics,
avec contrôle de son ascendance native. La reconstruction complète de
composition, les publications de mouvement et la lignée de session pendant
les chargements asynchrones restent à qualifier.

## Périmètre et preuves

L'analyse porte uniquement sur le binaire Windows x64
`1.19.10.5bfaea3`, 12 583 424 octets, SHA-256
`fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae`.
Les adresses courtes du texte sont des RVA ; les VA complètes sont identifiées
explicitement dans les preuves du 10 octobre. Les exports Ghidra utilisent la base
`0x140000000`. Un autre binaire ne reçoit aucune de ces hypothèses.

Les preuves primaires de cette campagne sont les décompilations, assembleurs et
références de fonctions dans le dossier privé
`.validation/signal-interlocking-20261009/native-reservation-lifecycle/`, ainsi
que `native-permission/`. `provenance.txt` et `invocations.txt` qualifient chaque
export. Ils sont reproductibles avec `tools/InspectConstruction.java`, le projet
analysé privé et les options Ghidra `-readOnly -noanalysis`. Les prototypes
décompilés ne sont pas des signatures d'appel validées.

Cette campagne n'a ni lancé le jeu, ni posé un nouveau hook, ni appelé une
fonction native, ni modifié une réservation. Les anciennes captures de
[réservations et occupation](reservations.md) restent des observations datées,
pas une validation en jeu du futur remplacement.

## Trois états distincts à conserver

- **Occupation physique** : portions réellement couvertes par des voitures. Le
  SDK doit continuer à utiliser les producteurs et lecteurs natifs qualifiés.
- **Réservation exclusive native** : portions promises à un train dans les deux
  index natifs. C'est ce mécanisme que l'autorité SDK pourra remplacer.
- **Autorité SDK** : bail atomique sur les ressources d'un itinéraire qualifié,
  autorisation de mouvement et maintien jusqu'à dégagement de l'arrière. Ce bail
  ne se déduit ni d'un aspect vert, ni de la présence du train dans un `Path`.

Une réservation virtuelle annoncée par un script est encore un quatrième fait :
elle n'est pas nécessairement présente dans l'index natif. Un mode compatible
doit conserver le veto de ce script ou fournir un contrat de remplacement
explicitement pris en charge. Vider les réservations natives ne le remplace pas.

## Diagnostics d'observation — reprise du 10 octobre 2026

La lecture d'approche conserve son contrat et ses budgets. Elle classe les
échecs pendant les lectures déjà effectuées, sans seconde capture ni nouvelle
requête au jeu : aucun scan tenté, scan invalidé, capture périmée ou horloge
incohérente, chemin,
topologie, tête ou frontières non qualifiés, ou échec du contrôle final après
lecture. « Non qualifié » couvre une lecture indisponible ou une valeur qui ne
correspond plus ; le diagnostic ne prétend pas distinguer ces deux causes.

Une capture ne contenant aucun signal demandé est ignorée sans nouvelle
lecture. Un chemin qualifié peut ne produire aucune approche lorsque le
signal demandé est déjà franchi ou au-delà du nombre de cantons observés.
Un résultat `ready` indique une lecture qualifiée, jamais une permission de
circuler. Les flags `NATIVE_CLEAR` et `NATIVE_BLOCKED` restent les résultats
du premier stage natif, distincts de la décision finale et de l'autorité SDK.

Le lecteur agrège les motifs, les slots occupés par un autre callback, les
lignes/sections produites et le statut du lot. Les identifiants complets
échantillonnés concernent au plus quatre trains dont le slot a été verrouillé.
Un slot occupé ne permet de lire ni son identité ni son état ; aucun train ou
signal bloquant n'est alors inventé. Une déqualification du chemin entier
ne désigne pas un signal responsable.

La fenêtre conserve les échecs brefs jusqu'au prochain rapport. Les rapports
sont limités à un par seconde en cas de changement, avec un bilan toutes les
30 secondes en régime stable. Le stockage d'agrégation est fixe ; la mise en
forme se fait seulement lors d'un rapport, côté lecteur d'observation. Aucun
callback natif de scan ne formate ou écrit ces journaux. Ces diagnostics
complètent l'observation ; ils n'activent pas l'enclenchement central.

## Index natifs et fonctions qui les modifient réellement

Le stockage commun de réservations possède deux ensembles de 16 partitions :
le premier index par propriétaire, puis l'index par voie à `base + 0x880`.
Dans `Sim`, cette base est `Sim + 0x518` et l'index par voie se trouve donc à
`Sim + 0xD98`. Il ne faut pas appliquer `+0x880` directement à `Sim`.
L'index par propriétaire utilise des slots de `0x60` ; l'index par voie, des
slots de `0x20` et des vecteurs de portions de `0x20` octets. Les partitions font
`0x88` octets. Les mutex et allocations appartiennent au jeu.

| RVA | Preuve primaire et rôle | Portée d'une future interception |
| --- | --- | --- |
| `0x451F70` | Ajoute `{trainID, trackID, from, to}` au deque privé de construction, puis au vecteur par voie sous mutex via `0x45D710`. Ajuste légèrement la première borne et écrit les contrôles de la table lors d'une insertion. | Point bas de création de portions ; ne pas le supprimer isolément, car le deque par train et l'index par voie doivent rester cohérents. |
| `0x451C90` | Appelle d'abord `0x4512D0(owner)`, construit les portions via `0x458750`, ajoute éventuellement des portions de `Motion + 0x3E8/+0x3F0`, puis publie le deque dans l'index par propriétaire. | Transaction de reconstruction par train. Candidat principal pour observer un début/fin de transaction avant de décider d'un remplacement. |
| `0x458750` | Prépare l'état de route et de service depuis `Motion`, puis appelle `0x45DD80`. | Préparation native de la construction, distincte de la permission d'entrer. |
| `0x45DD80`, `0x45E720` | Parcourent le chemin et les transitions de service, appellent `0x451F70` pour chaque portion et s'arrêtent notamment sur une frontière de signal de kind 6 autre que la source. | Producteurs de la suite de portions, à observer sur le thread natif ; aucune hypothèse d'itinéraire tirée d'un simple ensemble d'IDs. |
| `0x450B40` | Copie le deque d'un train, cherche la première portion dont la voie correspond à `Motion + 0x3A8`, retire les portions précédentes des vecteurs par voie puis remet le suffixe dans l'index par propriétaire. | Retention/libération du préfixe natif. Ce comportement lié à la tête **ne prouve pas** que l'arrière a quitté une zone de conflit. |
| `0x4512D0` | Retire l'ensemble des portions d'un propriétaire de l'index par voie et supprime son agrégat par propriétaire. | Invalidation d'un propriétaire ; garder séparées annulation avant entrée et occupation après entrée. |
| `0x451820` | Collecte les propriétaires d'une voie puis appelle `0x4512D0` pour chacun. Appelé par `0x479530` lors du traitement de changements. | Invalidation liée au réseau, qui peut concerner plusieurs trains. |
| `0x4510B0` | Vide les deux index sous leurs mutex et libère les allocations. Appelé par `0x47D260` et `0x47D410`, qui remettent aussi d'autres états de simulation à zéro. | Observer une transition de monde ; ne jamais utiliser ce reset global comme réparation d'un signal. |

Les appelants directs connus de `0x451C90` comprennent `0x448710` (mouvement,
plusieurs branches), `0x4482C0` (frontière de signal/reprise), `0x445FE0`,
`0x44A330` (modification de route), `0x440840` et le parcours global des `Motion`
à `0x45CF80`. `0x44EF70` appelle le maintien `0x450B40` ou l'effacement
`0x4512D0` selon l'état du train, avant les autres traitements du train.
Ces chemins montrent qu'un hook sur une seule demande de signal ne couvre pas
toutes les créations, reconstructions ou suppressions natives.

Le sweep `0x45CF80` parcourt les slots natifs `Motion` de stride `0x638`, sans
filtre d'identifiant de mod. Cela prouve l'existence d'un chemin commun aux
trains natifs ; cela ne prouve pas que l'actuel contrôleur SDK observe tous ces
trains ni que ce sweep couvre chaque mode de conduite.

## Lecteurs, admission et callbacks de scripts

| RVA | Rôle observé | Ce que son résultat ne prouve pas |
| --- | --- | --- |
| `0x4582D0` | Prédicat d'occupation physique d'une portion, en excluant le train demandeur. | Une permission complète ou une réservation. |
| `0x458460` | Lit les réservations exclusives d'une portion via `0x45F860`, ignore le train demandeur, écrit seulement le résultat fourni par l'appelant. | Création ou suppression d'une réservation. |
| `0x458560` | Callback d'admission sur une portion : occupation et réservation propres, conflits de `Track + 0xF8`, puis contrôleur virtuel de signal lorsque le kind vaut 7. | Décision finale de tous les chemins de passage. |
| `0x451B60` | Assemble des contextes et un résultat sur la pile, appelle `0x45DB00`, retourne cet octet local. | Réservation engagée, passage effectué ou décision finale après scripts. |
| `0x448210` | Combine `0x451B60` avec `0x4273D0` ; le veto script égal à 1 interdit l'entrée même si le premier stage est libre. | Couverture de toutes les branches : certaines passent par `0x447310` ou par d'autres états de mouvement. |
| `0x45F860`, `0x45F780` | Recherches de tables, la première avec maintien du verrou natif fourni par l'appelant. | Mutation des portions ; ne pas confondre un verrou avec une réservation. |
| `0x4515C0` | Recherche un propriétaire dans réservations puis occupation sur un intervalle. | Writer de réservation. |
| `0x452250`, `0x452860`, `0x452C30` | Consommateurs Extrapolator et probes des réservations/occupations. | Hooks de création. |

Le callback `0x458560` consulte précisément le vecteur `Track + 0xF8/+0x100`,
stride `0x30`. Les autres listes `+0xE0`, `+0x110`, `+0x128` restent distinctes.
[Le décodeur de conflits](track-conflicts.md) qualifie uniquement cette liste.

Le compilateur de scripts `0x424C00` fournit la correspondance primaire suivante
dans un record de callbacks de `0x78` octets :

| Champ | Événement | Dispatcher observé |
| --- | --- | --- |
| `+0x10` | `event_signal_check` | `0x4273D0`, veto lorsque le résultat vaut 1 |
| `+0x18` | `event_signal_lookahead` | Présent dans le scan natif `0x449700` |
| `+0x20` | `event_signal_change_path` | Préparation/changement de chemin ; contrat à conserver |
| `+0x28` | `event_signal_pass_by` | `0x4275B0`, événement de passage, sans résultat d'admission |
| `+0x30` | `event_signal_marker_reserved` | `0x4276C0`, retour d'un ID propriétaire non nul |

`0x4276C0` est un probe de propriétaire virtuel ; `0x4275B0` n'est pas une
notification de création de réservation. Les dispatchers parcourent les scripts
attachés au signal, résolvent leur modèle et invoquent des pointeurs de fonction
du script compilé. Une interrogation supplémentaire peut donc exécuter du code
tiers : aucune nouvelle lecture SDK ne doit rejouer ces événements, changer leur
thread ou les exécuter sous un verrou du moteur central.

Les branches de `0x448710` et `0x4482C0` montrent la séquence
permission → reconstruction `0x451C90` → mouvement/événement, mais également des
chemins alternatifs de service et de reprise. Un futur hook sur `0x451B60` seul
ne peut pas être présenté comme l'autorité universelle de passage.

## Occupation physique à maintenir

La collection globale d'occupation `Sim + 0x2C8`, son itérateur `0x459EA0` et le
prédicat `0x4582D0` restent séparés des index de réservations. Le producteur
`0x43F670`, appelé une fois par `0x44B5B0` pour un `Motion` admissible, parcourt
une longueur agrégée fournie par `Motion + 0x40`. Il écrit des portions de `0x20`
octets contenant l'identifiant du train et ses bornes ; ces lignes ne sont pas
des identifiants ou positions individuels de voitures. Le compteur et les clés
de l'append map sont publiés atomiquement avant les valeurs ordinaires. Le
débordement dispose d'un chemin protégé par mutex. Ce protocole ne constitue pas
un certificat de fin d'écriture pour un lecteur externe.

`0x44B5B0` calcule aussi la géométrie et les données de hit map. Il intervient
avant le déplacement/replan du même train dans le worker `0x44EF70`. La présence
de lignes dans `Sim + 0x2C8` ne prouve donc ni une composition complète ni leur
cohérence avec les `Motion` déjà déplacés. Il ne faut pas désactiver ce worker
ou un reset général de simulation pour supprimer les seules réservations
exclusives.

Le SDK actuel peut interroger le prédicat natif original dans son callback
synchrone, avec un contexte copié et un octet résultat privé. Cette lecture garde
l'exclusion du train demandeur et ne modifie pas la table d'occupation. Elle
n'autorise pas un appel différé sur un worker de mod, ni une lecture sans contrôle
du monde, de l'identité et de la durée de vie du contexte natif.

`0x46CBD0` contient aussi des boucles de 16 partitions `0x88` et un calcul
`+0x880`, mais le bloc observé démarre à `object + 0x220` et utilise des slots
`0x50`. Cette seule similarité ne le qualifie pas comme writer des réservations.
`0x459BE0` est un insert de copie de `Motion` pour l'inspecteur, pas le producteur
de la collection d'occupation. Ces faux positifs sont exclus du plan de hooks.

## Longueur agrégée et phases natives — analyse du 10 octobre 2026

Cette étape est une lecture statique des exports `.c`, `.asm` et `-edges.txt`
de `.validation/native-interlocking-proof-20261010/`, complétée par
`native-reservation-lifecycle/` de la campagne précédente. Le nouveau dossier
contient `provenance.txt` et `invocations.txt` ; le hash et la base d'image restent
ceux du périmètre ci-dessus. Aucune signature décompilée, barrière ou observation
de passage décrite ici n'a été validée dans le runtime du jeu.

### Faits établis par les instructions

| Fonction et VA | Callsite ou instruction de contrôle | Fait observé |
| --- | --- | --- |
| `0x44CF80`, VA `0x14044CF80` | Appel de `0x3E0D90` à la VA `0x14044D675` | Reconstruit le vecteur `Dynamics` de `Motion + 8`, avec les voitures propres et certains trains couplés. Les tuples de `CarSetup`, stride `0x20`, conservent modèle, train propriétaire et ordinal ; le modèle n'est pas l'identité d'une voiture. |
| `0x3E0D90`, VA `0x1403E0D90` | VA `0x1403E0F07` à `0x1403E0F15` | Pour chaque modèle résolu, ajoute `Model + 0xFC` au champ `Dynamics + 0x38`, donc `Motion + 0x40`. Un modèle introuvable est sauté. |
| `0x44B5B0`, VA `0x14044B5B0` | Appel de `0x43F670` à la VA `0x14044B6EE` | Passe l'identifiant complet du train et la longueur `Motion + 0x40`, avec l'historique `Motion + 0xE0/+0xE8`. Le producteur est agrégé ; aucun inventaire complet par voiture n'est émis par cet appel. |
| `0x44EF70`, VA `0x14044EF70` | VA `0x14044F18C` puis `0x14044F1AB` | Calcule l'occupation via `0x44B5B0` avant d'appeler le déplacement/replan `0x44A330` pour le même `Motion`. |
| `0x462EC0`, VA `0x140462EC0` | Appel à la VA `0x140462ED2`, état écrit à `0x140462F29` | Appelle `0x44EF70`, déplace ses résultats, puis écrit l'état 3 du task. Cet état concerne un worker, pas toutes les écritures de la simulation. |
| `0x44D6B0`, VA `0x14044D6B0` | VA `0x14044DDD3`, `0x14044ECC9`, `0x14044ED0D` et `0x14044ED20` | Traite les tasks avec `0x355120`, reconstruit ensuite certaines compositions avec `0x44CF80`, puis transfère les maps d'occupation avec `0x45A120`. |
| `0x355120`, VA `0x140355120` | Retour direct pour les états 3/4 ; appel indirect à `0x14035514D` | Pour les autres états, passe par une fonction TLS. Les deux chemins d'attente sont identifiés dans le complément ci-dessous ; cela ne transforme pas la fin d'une tâche en certificat de couverture complète. |
| `0x478000`, VA `0x140478000`, et `0x477CE0`, VA `0x140477CE0` | `140478000.c:70` ; appel de `0x44D6B0` à la VA `0x140477F00` | `Sim + 0x2128` est renseigné avant les calculs ; `Sim + 0x28` est avancé avant `0x44D6B0`. Aucun de ces champs, lu seul, n'est une preuve de fin de tick. |
| `0x459FC0`, VA `0x140459FC0` | Clé à la VA `0x14045A0CF`, valeurs aux VA `0x14045A106` et `0x14045A10A` | L'insertion atomique de la clé précède la copie des deux blocs de valeurs. Le nombre de lignes réservées ne certifie pas des lignes toutes achevées. |
| `0x47D410`, VA `0x14047D410` | `14047d410.c:44–49,75` | Efface notamment occupation et réservations, reconstruit des états de trains, puis incrémente `Sim + 0x2120`. Il reste à qualifier la couverture des autres chemins de reset/chargement. |

Le contexte construit par `0x442170` distingue la map d'occupation actuellement
lue de la map locale en cours de construction. Les transferts `0x45A120` en fin
de `0x44D6B0` placent l'ancienne occupation dans le buffer référencé par
`*(Sim + 0x2208) + 0xB8`, puis la map locale dans `Sim + 0x2C8`. `0x478000`
échange aussi les buffers `Sim + 0x2208/+0x2210` avant le traitement. Ces
transferts ne fournissent ni génération atomique propre aux occupations ni
durée de propriété immuable à un lecteur SDK.

Le contrôleur `0x3489D0`, exporté dans
`docs/research/reports/construction/1403489d0.c`, publie ensuite des buffers
après `0x47A5F0`, sous le mutex du contrôleur `+0x580` : échanges avec
`Sim + 0x2200/+0x2208`, puis flag de publication `+0x180` (`:685–699`, autre
branche `:841–862`). Le réemploi des buffers et la synchronisation nécessaire
pour en posséder une copie restent à qualifier ; lire deux fois les mêmes
pointeurs ne remplace pas cette qualification.

### Complétude et synchronisation encore à prouver

L'interprétation candidate est une occupation du corps entier du train assemblé,
reconstruite sur son historique. Elle peut fournir une preuve équivalente à
toutes les voitures seulement après qualification de sa complétude. Il ne faut
pas convertir chaque ligne en voiture fictive, ni exiger des identifiants de
voitures que ce producteur ne contient pas.

La reconstruction `0x44CF80` conditionne la copie de certaines compositions
couplées à une somme de tailles strictement inférieure à `0x1F`
(`14044cf80.c:249`). `0x3E0D90` ignore les modèles absents. Le calcul de longueur
doit donc être lié à la composition effectivement attendue, à sa révision et aux
trains couplés concernés ; une longueur positive n'est pas cette preuve.
Une reconstruction tardive de composition dans `0x44D6B0` peut en outre être
postérieure au calcul des occupations.

Le total natif est une accumulation **en float** (`ADDSS` aux VA
`0x1403E0F07–0x1403E0F15`), ensuite convertie en double pour `0x43F670`.
Une somme de modèles calculée en double ne reproduit pas automatiquement ce
résultat. Les tuples attendus doivent inclure propriétaire, ordinal,
configuration et orientation. La branche de couplage inversé parcourt les
voitures en ordre inverse et inverse leur orientation ; la révision ne peut
donc pas se limiter au nombre de voitures. Le correctif SDK du hash de dynamique
au-delà de 30 voitures ne modifie pas la branche native qui omet un couplage
lorsque le total dépasse 30.

`0x43F670` peut retourner avant consommation complète de la longueur en cas de
voie ou continuation introuvable ; il possède aussi une garde de progression
(`14043f670.c:79,85,177,181,377`). `0x44B5B0` ne distingue pas ces retours d'un
parcours complet et limite ensuite l'historique à 100 éléments
(`14044b5b0.c:91–101`). La présence d'un historique ou d'un résultat non nul ne
certifie donc pas sa continuité jusqu'à la queue. Une future preuve doit établir
que la totalité de la longueur qualifiée a été parcourue, avec budget borné et
absence explicite de troncature.

En particulier, la garde après au moins 50 transitions de faible progression
(`0x14043FC49–0x14043FC80`) remplit aussi un résultat de position arrière.
Un résultat non nul ne distingue pas ce retour tronqué du retour ayant consommé
toute la longueur (`0x14043F940 → 0x14043FCA4`). `0x44B5B0` saute aussi les
états `Motion + 0x1D0 = 0` ou `Motion + 0x1F0 != 0` : l'absence de ligne ne
doit pas être interprétée comme une rame entièrement dégagée.

Les occupations produites avant mouvement ne doivent pas être associées aux
positions ou compositions post-mouvement sous le seul prétexte que leur horloge
est égale. Une proposition de certificat doit nommer la phase d'entrée utilisée,
prouver la fin de ses producteurs et conserver une copie possédée des données
avec la même lignée de monde, les mêmes identifiants complets et révisions.
Les resets, le réemploi des buffers et les changements de composition restent
des obligations de preuve ; aucun certificat n'est déduit des seuls compteurs
ou de deux captures externes identiques. Le dispatch TLS de `0x355120` est
désormais identifié par l'étude statique suivante.

### Attente des tâches et limites de la fin de phase — complément du 10 octobre

Les nouveaux exports sont dans `.validation/native-tick-consist-20261010/`.
Ils proviennent du même projet Ghidra, toujours en lecture seule et sans nouvelle
analyse. `tls-evidence.json` conserve une lecture bornée de la directory TLS,
du template initial et des deux vtables de continuation, après vérification du
SHA-256 exact du jeu. Le label non déclaré comme fonction `0x140003F00` est
exporté par un script de lecture de 48 octets ; aucune fonction n'a été créée
dans le projet pour lui attribuer une signature supposée.

- Le template TLS initial contient `+0x20 = 0x1400028E0`. Sur un thread hors
  scheduler, cette fonction attache une continuation à la tâche et attend sa
  notification sous mutex avec `_Cnd_wait`.
  La chaîne de notification est qualifiée jusqu'à `0x0025C0` : écriture du bit
  1 dans `context + 0x98` à `0x140002601`, `_Cnd_signal` à `0x14000260B`, puis
  déverrouillage. `0x0021B0` utilise un CAS et le bit de terminaison pour traiter
  la course entre enregistrement de la continuation et tâche déjà terminée.
- Le démarrage d'un worker `0x003F30` installe à `TLS + 0x20` le label
  `0x003F00`. Celui-ci passe la tâche en `R8`, le scheduler depuis `TLS + 0x18`
  et l'ordinal du worker depuis `TLS + 0x48`, puis rejoint `0x0038E0`.
  L'attente est coopérative : elle peut exécuter d'autres tâches avant que la
  tâche attendue soit terminée ou que sa continuation soit notifiée.
- Les états 3 et 4 sont des états terminaux utilisés par `0x355120`.
  **Une tâche terminée en erreur n'atteste pas une occupation complète.**
  Le retour de l'attente n'autorise pas à convertir une erreur de producteur
  ou une liste partielle en preuve de dégagement.

Le retour de `0x44D6B0` est donc un candidat de **fin de phase des trains**, après
ses attentes et les traitements locaux ultérieurs. Ce n'est pas encore la fin
immuable de tout le tick : `0x477CE0` enchaîne `0x46CBD0`, et le contrôleur
enchaîne ensuite `0x478F40`, puis `0x47A5F0` et la publication des buffers.
`0x47A5F0` copie les états et lit l'occupation existante ; aucun appel direct
au producteur `0x44B5B0` ou `0x43F670` ne corrige le décalage de phase décrit
ci-dessus. La fin des tâches de trains ne rend donc pas cette occupation
antérieure cohérente avec la nouvelle présence.

Un autre trou de lignée est confirmé : `0x47D260`, appelé par `0x477800` et
`0x479BD0`, efface occupations, réservations et buffers sans incrémenter
`Sim + 0x2120`. Ce champ ne couvre pas tous les resets. La future lignée SDK
devra observer ces transitions qualifiées, sans prendre une horloge ou le seul
compteur natif pour une identité de monde complète.

Cette étude statique ne produit aucun certificat de fin de tick, preuve arrière
ou permission. L'observateur de fin de phase décrit plus bas ne possède pas une
publication immuable du tick. Le raccordement de la couverture de rame et de la
lignée reste à réaliser et à vérifier en jeu.

### Lignée, propriété native et écrivains — complément du 10 octobre

Les preuves complémentaires sont conservées dans
`.validation/native-final-phase-ownership-20261010/` et
`.validation/native-writer-load-lineage-20261010/`, avec leurs sous-dossiers.
Les exports Ghidra restent en lecture seule, sans nouvelle analyse, sur le
SHA-256 qualifié ci-dessus. `ancestry-hook-evidence.json` conserve une lecture
bornée des prologues et les callsites ; les signatures inférées ne sont pas des
ABI validées en jeu. **Ces preuves statiques n'activent aucune autorité. Toutes
les captures runtime actuelles restent sans certification.**

Une fenêtre locale de propriété est identifiée : dans `0x3489D0`, le contrôleur
possède son verrou `Shared` à `controller + 0x4C0`, de l'appel
`0x140349486` à l'appel `0x140349F9E`. Les deux appels à `0x478000` dans cette
fenêtre sont `0x1403495F9` et `0x140349B5F`, avec retours `0x1403495FE` et
`0x140349B64`. `0x478000` appelle `0x477CE0` à `0x1404784F4`, retour
`0x1404784F9` ; celui-ci appelle `0x44D6B0` à `0x140477F00`, retour
`0x140477F05`. Cette ascendance permettrait de nommer le verrou déjà détenu,
sans nouvelle acquisition, avec contrôle du même contrôleur, réseau et Sim.
Un autre caller de `0x478000`, côté serveur à `0x1404F3EA9`, n'hérite pas de
cette qualification. Le scope TLS diagnostique raccordé rejette les autres
callsites et la réentrance ; le seul callback `0x44D6B0` ne prouve pas cette
ascendance.

Le chargement `0x347B10` et la création de monde `0x347840` acquièrent
`Exclusive` sur le même `controller + 0x4C0`, remplacent les objets, puis rendent
un guard au caller. Dans le chemin de chargement observé, `0x740C40` libère ce
guard après le retour de `0x347B10`. Ces remplacements sont donc exclus pendant
la fenêtre `Shared` qualifiée ; ce verrou ne prouve pas à lui seul l'exclusion
de tous les autres écrivains. `0x479BD0` appelle `0x47D260` à `0x140479BFF`,
puis importe les Motion avec `0x450290` à `0x140479C63`. **Le retour du reset
n'est pas la fin du chargement.** Le compteur `Sim + 0x2120` et l'epoch local
de l'observateur restent des invalidations, pas des certificats de monde prêt.

| Chemin qualifié | Écriture ou copie observée | Conséquence pour une future capture |
| --- | --- | --- |
| Import `0x450290 → 0x45AC60` | Copie Dynamics, Presence, historique et liens de couplage ; appelle ensuite `0x3E0D90` pour recalculer la longueur. | Ne pas réutiliser une capture antérieure au chargement ou combiner ses champs avec les nouveaux objets. |
| Attelage `0x441070` | Modifie `Motion + 0x1D8/+0x1E0/+0x1E8/+0x1F0`. `0x441B20/0x441C00` soumettent la commande via la file `0x456A10`. | La composition complète et sa révision doivent inclure ces liens ; la sérialisation de tous les consommateurs reste à qualifier. |
| Configuration `0x440F50` | Invalide `Motion + 0x78` et change la sélection `+0x80`. | Un nombre de voitures inchangé ne suffit pas à conserver la qualification de composition. |
| Retrait `0x43E300`, via notamment `0x44C810` | Libère l'historique `+0xE0`, désactive `+0x1D0` et `+0x1F0`. | L'absence de présence ou d'historique conserve un dégagement inconnu ; elle ne libère pas un bail. |
| Replan `0x440840` | Réécrit Presence `+0xB8/+0xC0/+0xC8` et peut vider l'historique `+0xE8`. | Requalifier la route conservée et ses orientations ; ne pas substituer un nouveau parcours au bail original. |

Les copies `0x478820/0x4595B0` ont des destinations distinctes du pool source.
Les appels `0x4D7590/0x4D77F0` à `0x43E300` portent sur des vecteurs contigus
de leur objet (`+0x210/+0x218`) ; ils ne démontrent pas une écriture directe du
pool de la simulation active. Une même fonction de copie ou un même offset ne
suffit pas à qualifier un écrivain du monde actif. Les phases voyageurs étudiées
après `0x44D6B0` n'écrivent directement que leurs champs de voyageurs dans les
exports examinés ; leurs graphes indirects ne sont pas tous qualifiés.

Le prochain raccordement est une capture POD possédée et bornée dans cette
fenêtre locale, après validation de l'ascendance et de la lignée. Elle doit
conserver l'inventaire complet des couplés, les modèles résolus, les Dynamics et
la trace orientée commune aux seuls trains concernés, puis publier ses valeurs
hors de la fenêtre native. La couverture des commandes, scripts et écritures
asynchrones restantes doit être prouvée avant toute certification. Les essais
de chargement, retrait, attelage, inversion et serveur vérifient les chemins
qualifiés ; ils ne remplacent pas cette preuve de couverture. **Le fournisseur
de preuve arrière et l'enclenchement natif central restent non raccordés.**

### Passage et observation locale après mouvement

Dans `0x448710`, le dispatcher `event_signal_pass_by` `0x4275B0` peut être appelé
avant la fin des contrôles du déplacement. Le callsite VA `0x140448FD5` précède
`0x446650` à la VA `0x140449008` ; une branche restaure ensuite l'état de mouvement
sauvegardé et annule le déplacement (`140448710.c:350–384`). Un événement de
script observé à ce point n'est donc pas, à lui seul, une preuve d'entrée
physique définitive. L'autre callsite VA `0x1404489DA` ne suffit pas non plus à
prouver la couverture universelle de toutes les branches.

Le retour de l'original `0x448710` est un candidat pour une observation locale
bornée de la progression finalement conservée, avec les identités et la route
qualifiées de cet appel. Cela reste un prototype de diagnostic à qualifier ;
il ne livre ni autorité de passage, ni transaction de réservation, ni certificat
de dégagement arrière, ni libération de bail. Il ne faut ni rejouer un dispatcher
de script ni conserver un pointeur de contexte natif pour traiter cette
observation plus tard. La couverture des appels, du rollback, de la reprise,
du reversal et des mutations de monde demeure à vérifier dans le runtime.

## État présent dans le SDK

Le pont Windows possède aujourd'hui des hooks qualifiés sur le mouvement
`0x448710`, l'intégration `0x378600`, le scan `0x449700`, l'occupation
`0x4582D0`, le prédicat de réservation `0x458460`, le callback de permission
`0x458560` et le stage `0x451B60`. Il ne possède pas les nouveaux hooks de
transaction/retention/effacement du tableau ci-dessus.

L'entrée à vue déjà existante remplace seulement un refus de réservation
longitudinale dans une requête précisément reconnue, avec résultat privé.
Les contrôles croisés et le contrôleur virtuel restent natifs. Aucune portion
n'est retirée. Voir [l'entrée restrictive](restricted-entry.md).

La nouvelle observation `SignalRoute` utilise la trace ordonnée du scan natif,
revalide la tête, les identités complètes, les bornes et le contenu du `Path`, et
copie des sections qualifiées. Le résultat `NATIVE_CLEAR` représente le stage
observé de `0x451B60`, avant le veto `event_signal_check`. Il n'est jamais une
autorisation finale. La publication active dépend encore des consignes SDK et
l'index d'intérêt limite la capture aux trains gérés : le système n'a pas la
couverture universelle nécessaire à une suppression du mécanisme natif.

Les nouvelles lectures ciblées des conflits et des fins de voie sont disponibles
en tant que données optionnelles qualifiées. La capture externe n'est pas un
snapshot atomique du tick. Une fin de voie `Terminal` prouve une absence de
continuation native dirigée, pas un type graphique de heurtoir ni une permission.

`engine/route_resources.h` produit les intervalles propres et leurs alias croisés
à partir de portions qualifiées et de listes de conflits complètes.
`engine/route_interlocking.h` fournit l'acquisition atomique, le conflit, le
maintien après entrée et la libération sur preuve arrière. Les fixtures pures
valident ces comportements. **Ces moteurs ne sont pas encore branchés comme
autorité dans les hooks natifs et ne remplacent pas les réservations du jeu.**

### Observation expérimentale raccordée — 10 octobre 2026

Le SDK possède désormais un adaptateur de diagnostic parallèle dans
`platform/windows/runtime/route_shadow_observer.h`, appelé après la copie des
routes par l'opération interne 203 du parent SDK. Il s'active uniquement avec
`NRF_INTERLOCKING_SHADOW=1` dans l'environnement du processus ; il est désactivé
par défaut. Lorsque l'option est absente, l'adaptateur n'ajoute aucune lecture
du jeu. Cette option interne ne constitue ni une fonction de mod ni un mode
d'enclenchement utilisable par le joueur.

Son périmètre est `requested_mod_routes` : les itinéraires demandés par les
lecteurs de mods, issus des trains déjà suivis par le pont. Il ne couvre pas
tous les trains. Il copie les sections, lit les listes F8 ciblées et appelle
`engine/route_shadow_batch.h` puis `engine/route_interlocking_shadow.h`.
L'observation reste `non_atomic` : la route copiée et les conflits lus ensuite
ne sont pas un instantané commun d'un tick natif. Une expansion de ressources
complète n'est donc pas une permission, une réservation native ou une preuve
de passage.

Le coordinateur possède une file fixe de 64 événements, avec au plus 512
intervalles copiés par événement. Les soumissions ne font ni allocation ni
attente ; une file pleine ou une contention est comptabilisée sans effacer une
protection. Un seul propriétaire traite le moteur. Le calcul et les lectures
ne tiennent aucun verrou de la file, et les rapports sont mis en forme et écrits
après libération de la garde du worker. Les rapports du coordinateur sont
bornés à un par seconde en cas de changement, avec bilan après 30 secondes si
le traitement continue. Au plus quatre échantillons conservent les identités,
les motifs, le stage natif et les preuves d'entrée/arrière ; les refus sont
prioritaires sur les acquisitions ordinaires.

**L'adaptateur de baux shadow ne produit ni preuve de non-entrée, ni événement d'entrée,
ni preuve arrière natifs.** Ses demandes portent `entered=Unknown` : le noyau
refuse alors toute nouvelle acquisition, même si les ressources sont complètes.
Il produit donc actuellement des diagnostics de qualification et d'entrée
inconnue, sans nouveau bail central ni arbitrage de passage actif. Les tests du
coordinateur avec preuves synthétiques vérifient acquisition, conflit et
rétention ; ils ne prouvent pas ces producteurs dans le jeu. `NATIVE_CLEAR`
reste le stage natif observé avant les autres veto, distinct du résultat du
noyau et de la permission finale.

La continuité des métadonnées est contrôlée avant et après les lectures. Après
le premier repère, une identité absente ou changée verrouille cet observateur
en état indisponible jusqu'à la fin du processus, sans libérer ses protections.
Une liste F8 indisponible avec continuité encore lisible est simplement ignorée.
Les métadonnées de versioning ne constituent pas un événement de chargement de
monde : leur changement ne déclenche aucune réinitialisation automatique de
baux. L'absence d'une route dans un lot n'en libère aucune.

L'adaptateur ne modifie aucun résultat de permission, aucun index de réservation
ni aucune table du jeu, et n'ajoute aucun hook natif. Le remplacement central
des réservations, les événements de cycle de vie et la couverture universelle
restent à implémenter et à qualifier selon les étapes ci-dessous.

### Observation locale du passage après le pas natif — 10 octobre 2026

Un second observateur de diagnostic est raccordé dans
`automatic_driving_bridge.cpp`, derrière la même activation explicite
`NRF_INTERLOCKING_SHADOW=1`. Le bootstrap vérifie le SHA-256 du profil puis
les 16 premiers octets du dispatcher `0x4275B0` avant d'installer ce hook
supplémentaire. Le mode par défaut ne l'installe pas et ne réalise aucune
lecture, allocation ou mesure d'horloge propre à cet observateur.

Le wrapper `0x448710` possède une transaction locale au thread, également pour
les trains sans règle SDK. Le dispatcher copie ses candidats avant d'appeler
son original exactement une fois, avec les six arguments inchangés. Après le
retour complet du pas, l'observateur compare la présence dynamique conservée
avec son miroir publié `Motion + 0xB8/+0xC0/+0xC8/+0xC9`. Cette lecture est
postérieure au contrôle de collision et à sa restauration éventuelle, contrairement
au retour de l'intégrateur `0x378600`.

La qualification locale exige les identifiants complets du train et de la source,
un signal PATH, des positions finies et cohérentes, la même voie et le même sens,
des orientations compatibles, une source inchangée, et le même descripteur de
chemin avec la totalité de sa liste de voies copiée depuis le premier candidat,
pas depuis le début du pas. Un `nextafter` à la borne,
une attente sans déplacement ou un déplacement annulé ne constituent pas un
franchissement. Une transition vers une autre voie, un alias non qualifié ou une
orientation nécessitant une transformation reste indisponible. Le prototype ne
conserve pas les candidats entre deux pas : il ne couvre pas encore tous les
franchissements terminant exactement à une borne.

Les bornes sont fixes : huit candidats et 512 références de voie par pas, 64
lectures et 32 Kio par transaction. La limite coopérative de 2 ms porte sur le
temps des lectures SDK, sans inclure l'exécution native ; une lecture ou une
désynchronisation du système peut la dépasser avant son retour. Aucun pointeur
natif ne quitte la transaction. Une capture incomplète invalide les résultats
positifs de toute cette transaction.

Une mailbox MPSC préallouée de 16 lots de huit lignes (128 lignes, moins de
16 Kio) conserve uniquement des valeurs numériques. Chaque producteur tente
une acquisition unique sans attendre. Les pertes par contention ou saturation
restent visibles dans ses compteurs ; un drain ne les efface jamais. Les logs
sont formatés sur l'appelant SDK de copie des routes, après libération des
verrous et pins natifs : au plus une émission par seconde lors des changements,
avec un heartbeat de 30 secondes. Sans appel de copie des routes, aucun drain
ni log n'est garanti et la mailbox peut saturer.

Le log `Native signal passage observation` nomme les résultats `crossed`,
`at_boundary`, `before_source`, `unavailable`, leurs motifs, les identifiants et
les coordonnées qualifiées. `observer_sequence` est un ordinal du diagnostic,
pas un tick du jeu ni un epoch de partie. `numeric_complete` qualifie uniquement
la représentation des nombres. Les marqueurs `runtime_qualified=0`,
`central_entry_proof=0`, `rear_clear_proof=0` et
`native_permissions_changed=0` restent explicites.

**Ce résultat local n'est pas transmis à l'autorité centrale.** Il ne fournit
ni certificat de durée de vie du monde, ni barrière de tick, ni couverture des
autres producteurs de mouvement, ni preuve arrière. Les identités et lectures
identiques ne prouvent pas une absence de mutation intermédiaire. Aucun bail
n'est acquis, libéré ou transformé par cet observateur. Les signatures et
l'adaptateur restent à valider dans le jeu ; les fixtures synthétiques ne
remplacent pas cette validation.

Les contrats et fixtures ajoutés se trouvent dans `native_signal_passage`,
`native_passage_mailbox` et `tests/windows/native_passage_observer.cpp`.
Les octets vérifiés du binaire et les nouveaux exports Ghidra restent dans
`.validation/native-interlocking-proof-20261010/` à la racine du workspace.

### Observation de fin de phase des trains — 10 octobre 2026

`platform/windows/runtime/native_train_phase_observer.h` et son raccordement
dans `automatic_driving_bridge.cpp` ajoutent une observation locale après le
retour de l'original `0x44D6B0`, uniquement avec `NRF_INTERLOCKING_SHADOW=1`.
L'original reçoit ses quatre arguments inchangés et reste appelé une seule fois.
Le bootstrap exige le profil de binaire reconnu et les prologues qualifiés ; en
mode normal, ces hooks supplémentaires ne sont pas installés.

`native_phase_ownership.h` conserve une ascendance TLS fixe de quatre scopes :
`0x3489D0 → 0x478000 → 0x477CE0 → 0x44D6B0`. L'entrée du contrôleur ne possède
pas encore le verrou. Seuls les callsites descendants exacts dans sa fenêtre
`Shared(controller + 0x4C0)` qualifient le diagnostic. Les références du
contrôleur, de Sim et du réseau, les deux valeurs des références partagées
copiées et le descripteur de pas sont relus avant et après la phase native,
puis après la capture. Un appel serveur, un descendant hors contexte, une
réentrance ou un changement annule les résultats temporaires. Le contexte est
propre au thread ; un jeton copié n'est plus accepté après son scope. La séquence
reste monotone lors du rafraîchissement d'un profil entre invocations.

Les erreurs C++ de l'original continuent de se propager. Le code de diagnostic
conserve aussi l'erreur Windows reçue par l'original et celle rendue à son
caller. L'invalidation reste attachée au pas actif jusqu'à son unwind ; un
nouveau pas exact peut ensuite être requalifié. Les exceptions SEH qui
contournent les destructeurs C++ ne sont pas récupérées. Cette ascendance
identifie une fenêtre locale étudiée, sans prouver l'exclusion de tous les
écrivains, un tick global stable ou une composition physique exhaustive.

L'observateur échantillonne seulement la table SDK des trains déjà suivis,
lorsqu'une publication de conduite est active. Il ne parcourt pas tous les
trains natifs. Une passe visite au plus 8 192 slots et tente au plus 32 captures,
sans attente sur les slots occupés. Le stockage est fixe : 32 résultats locaux,
puis au plus huit échantillons retenus pour le prochain journal. Chaque capture
relit l'identité complète, la présence publiée, la longueur native et le
descripteur d'historique ; elle copie au plus 128 lignes d'historique de
16 octets. Elle copie aussi au plus 128 `CarSetup` conservés et 64 enregistrements
d'attelage, chacun de 32 octets, puis confirme leurs octets et descripteurs.
Le parent déclaré, l'ordre et l'inversion d'attelage sont relus avec le flag
attelée. Les anciens champs parent/ordre d'un train détaché restent des octets
diagnostiques ; ils ne créent pas une relation d'attelage. Une limite dépassée
reste indisponible, sans troncature déclarée complète et sans calcul d'arrière
à partir de la seule longueur. Aucun catalogue de modèles n'est consulté ici.

Le budget partagé limite les tentatives à 8 192 lectures, 4 Mio et 2 ms
coopératives de travail SDK. Les slots vides déclenchent une vérification
d'horloge tous les 32 slots ; les lectures de capture et son readback final de
contexte sont comptabilisés. Les lectures de contexte aux entrées des trois
wrappers et avant/après l'original sont des préfixes fixes bornés, hors de ce
budget de capture. Un appel
de lecture déjà commencé ou une suspension du système peut dépasser la limite
avant de rendre la main. Les résultats restent locaux jusqu'aux contrôles finaux
de budget, de session et d'epoch ; une déqualification les écarte tous.

Les wrappers `0x47D260` et `0x47D410` encadrent les resets observés avec un
compteur d'invalidation local. Ils ne réinitialisent aucun bail et ne changent
aucune écriture native. Ce compteur détecte ces appels même si l'adresse de
`Sim` est réutilisée, mais ne certifie ni l'identité du monde ni la fin d'un
chargement asynchrone. L'observateur ne possède pas les buffers natifs et ne
qualifie pas leur durée de vie entre deux phases.

Le journal `Native train phase observation` est drainé lors des appels SDK de
copie des routes, au plus une fois toutes les 30 secondes quand de nouvelles
phases ont été observées. Le formatage intervient après libération des verrous
et pins SDK. `ancestry_reasons` expose les qualifications ou refus de contexte.
`readback_attempt_reasons` compte les tentatives locales,
y compris celles dont le lot est ensuite écarté ; `locally_revalidated` compte
seulement les résultats conservés après contrôle final. Chaque échantillon porte
son `capture_epoch` et son `observer_phase`, distincts du compteur courant au
moment du drain. Ce sont des ordinaux de diagnostic, pas des certificats de tick.
Sans appel de copie des routes, aucun rapport n'est garanti ; la saturation du
stockage d'échantillons est comptabilisée.

Les échantillons donnent `retained_cars`, les nombres de voitures dont le
propriétaire est ce train, un autre train ou absent, `attachment_rows`,
`parent`, `coupling_order` et `coupling_reversed`. Les empreintes des octets de
composition et d'attelage facilitent la comparaison des observations. Elles
ne certifient ni une révision native ni un inventaire exhaustif : une liste
d'attelage vide peut omettre un enfant nouvellement attelé. Le champ
`complete_consist_certified=0` reste présent même pour une lecture cohérente.

Les marqueurs `diagnostic_only=1`, `stable_tick_certified=0`,
`complete_rear_coverage=0` et `native_permissions_changed=0` sont explicites.
**Cet observateur ne produit ni entrée centrale, ni preuve arrière complète,
ni acquisition/libération de bail, ni permission nouvelle.** Sa fixture
`tests/windows/native_train_phase_observer.cpp` vérifie le mode désactivé, les
arguments et appels uniques, les refus de lecture, les identités, les limites,
les resets imbriqués ou pendant une capture et l'expiration pendant la dernière
lecture. Ces contrôles synthétiques ne valident pas l'adaptateur en jeu.

### Inventaire physique des attelés : contre-exemples confirmés

L'audit ciblé de 13 fonctions conserve ses preuves dans
`retained-consist-evidence.json`, dans le même dossier de validation. La
normalisation `0x45CD50`, appelée à `0x14044D751`, précède les applications de
commandes virtuelles de la phase (`0x14044E5AA` et `0x14044EAEA`). Un attelage
`0x441070` peut donc modifier parent, ordre, inversion et présence attachée
avant l'agrégation `0x44CF80`, sans insérer le nouvel enfant dans la liste
`Motion + 0x88`. Cette liste n'est pas un inventaire inverse exhaustif.

L'agrégation omet aussi des enfants au garde de nombre de voitures supérieur
à 30 (`0x14044D3EE–400`), puis réécrit la liste avec les seuls enfants retenus.
Le collecteur local avant cette omission ne retrouve toujours pas un enfant
absent dès l'entrée. Les nouveaux exports de normalisation, tri, copie et
commandes n'ont pas établi un autre index inverse complet. Une liste lisible
et répétée reste donc `Unknown / NativeAttachmentListNotExhaustive` pour la
preuve de composition. La longueur native float seule ne corrige pas cette
absence. Le prochain producteur doit posséder un inventaire complet après les
commandes, avec les écrivains et la lignée qualifiés ; aucune nouvelle libération
de réservation n'est activée à partir de ces observations.

### Énumération de tous les slots Motion et écrivains asynchrones

La campagne en lecture seule
`.validation/native-consist-inventory-20261010/evidence.json` examine
118 fonctions du même profil. `0x45ECE0` parcourt tous les emplacements alloués
du pool `Sim + 0xA0`, avec 512 slots par bloc et un stride `0x638`. Les trains
attachés ou inactifs restent présents dans ce parcours ; une liste filtrée sur
les trains actifs ne suffit donc pas à reconstruire les parents et enfants.
L'identité complète porte le type, l'ordinal et la génération. Aucun maximum
global de slots ni index compact natif exhaustif n'est établi.

`0x4597B0 → 0x45F230` crée les objets Motion et `0x479530` les détruit avant de
marquer le slot libre. Le pool des définitions de train `DB + 0x200` possède
un autre cycle de création/suppression (`0x3E4AE0`, `0x3E6C60`) ; ses voitures
prévues ne sont pas une copie qualifiée des voitures physiques courantes.
Les détachements automatiques `0x44A330` font aussi partie des tâches de phase.
Un lecteur doit construire son index inverse depuis tous les parents copiés,
puis qualifier la totalité du parcours et la stabilité des valeurs. Une limite
de temps, de lectures ou de lignes rend le résultat incomplet ; elle ne crée
pas une liste complète à partir des seuls emplacements lus.

Les deux workers persistants restants, `0x358D50` et `0x359630`, écrivent dans
des candidats Tiles et leurs index dans les chaînes examinées. Le contexte
`Controller + 0x220/+0x228` est identifié à la construction comme un
`shared_ptr<nimby::gis::Map>`. Aucun écrivain de Motion, de Train ou des modèles
de voitures n'est démontré dans ces chaînes. Cela ne certifie pas l'exclusion
universelle des effets transitifs : sources GIS virtuelles, callbacks,
destructeurs et autres appelants partageant les verrous ne sont pas tous
qualifiés. La lignée des chargements/imports et l'observation en jeu restent
également à valider. L'égalité de deux headers n'est pas un certificat
d'immuabilité globale et ne permet aucune nouvelle autorité de passage.

### Collecteur borné de l'inventaire Motion : fixture autonome

`native_consist_inventory.h` traduit ce parcours en un lecteur privé autonome,
sans appeler le copieur natif et sans consulter la liste incomplète `+0x88`.
Il visite les slots, y compris les trains attachés, inactifs et non planifiés,
puis construit l'index inverse parent/enfants depuis leurs identités complètes.
Un parent absent, une génération différente, un cycle ou une chaîne d'attelage
non qualifiée refuse tout le résultat. Le diagnostic d'identité conserve le slot,
l'ID attendu et l'ID relu ; les refus de relation nomment les trains concernés.

Les deux parcours comparent les identités et métadonnées copiées et relisent
le pool et ses pointeurs. Le lecteur utilise des buffers fournis par le caller,
sans allocation de tas pendant la capture. Un refus laisse la sortie inchangée,
avec zéro ligne publiée. Le budget partagé est terminal ; les boucles CPU le
consultent tous les 32 éléments au plus. Les plafonds structurels sont 128 blocs,
65 536 slots et 4 096 lignes ; le plafond distinct de 8 192 lectures peut être
atteint bien avant. Une capacité de stockage ne garantit pas une capture réussie.

Le stockage sortie/scratch d'un caller à 1 024 lignes et 8 192 slots représente
160 Kio ; aux plafonds structurels, 896 Kio. Le payload temporaire borné sur la
pile est de 6 240 octets, hors scalaires, métadonnées locales et choix du compilateur.
Le statut `ObservedAllSlots` décrit un readback local complet, sans garantir
l'exclusion des écrivains ni la composition physique exhaustive. Les indicateurs
`ownershipQualifier`, `universalWriterExclusion`, `completeConsistCertified` et
`rearClearCertified` restent faux, même après succès. **Ce collecteur n'est pas
raccordé au callback du jeu et ne modifie aucune permission.**

### Rétention des consignes de conduite : prototype de validation différée

L'audit a aussi identifié une limite du contrôleur existant : en mode normal,
son `commitIntegration()` est exécuté au retour de `0x378600`, avant le contrôle
de collision du pas natif. Ses effets incluent des réceptions **et** des
libérations de consignes ; ils ne doivent donc pas servir de preuve d'entrée
centrale. Le nouvel observateur de passage ne modifie pas cette sémantique.

Le prototype privé `NRF_DRIVING_TRANSACTIONAL_COMMIT=1`, désactivé par défaut,
diffère ces effets dans un commit en attente lié à une frame `0x448710`. Il
copie les valeurs utiles pendant que le résultat temporaire existe, qualifie
la tentative précédente avant le prochain intégrateur, puis la dernière après
retour du pas et vérification du miroir publié. Une restauration exacte rejette
cette seule tentative sans effacer les consignes antérieurement validées.
`memory={}` serait un correctif incorrect. La frame conserve l'image de
publication et le guard du train jusqu'au retour natif ; un train devenant géré
pendant le pas acquiert ce guard avant celui de l'intégrateur.

Les identités complètes, la présence, le chemin et ses IDs, la révision de route,
les ancres et orientations des sources et voies, les incarnations de publication
et les instructions sont requalifiées. Le témoin de chemin est limité à 512
voies et les sources à 128 ; les lectures utilisent le budget coopératif existant
de travail SDK du pas. Une donnée non qualifiée, un chemin plus grand ou une
réentrance du même train rendent le résultat inconnu. Cela n'étend pas le modèle
à toutes les transitions natives, ni ne produit une preuve centrale de tick.

Dans ce mode expérimental, une incertitude conserve les consignes déjà reçues,
mémorise un défaut du train et applique des cibles de vitesse et distance nulles
ainsi qu'un veto aux signaux gérés. **Ces commandes ne prouvent pas un arrêt
physique** : le plancher de vitesse natif peut encore produire un déplacement,
et une présence inconnue ne permet pas d'inventer un verrou géométrique. Ce
mécanisme n'est pas une protection autonome : le bypass natif quand plus aucune
publication SDK n'est active reste une limite. Il n'existe pas encore
d'acquittement qualifié du défaut en cours de partie ; ce mode est réservé aux
essais, pas activé dans le jeu par cette campagne.

Un correctif indépendant, actif sans ce mode, empêche `check` d'accepter une
nouvelle commande de conduite à une tête projetée. L'intégrateur peut l'accepter
à sa tête avant mouvement : un rollback ne doit pas transformer une commande STOP en
commande annulée pour inversion supposée. Les tests synthétiques couvrent
notamment une réception valide au premier mouvement puis une collision au
second, les frames imbriquées, l'intérêt tardif du train et ce cas de commande.
Il reste à qualifier ces nouveaux chemins sur le binaire réel avant activation
du commit différé par défaut.

La revue statique complémentaire du profil qualifié ferme aussi la chaîne du
scan : `0x449700` est appelé par `0x45BD80/0x45B3D0` depuis `0x4570C0`, dont
l'appel dans `0x448710` se trouve à la VA `0x140449258`, après le contrôle de
collision à `0x140449008`. La branche de restauration sort avant ce scan.
La permission à la tête provisoire utilise en revanche
`0x448210 → 0x451B60 → 0x45DB00 → 0x460610/0x460FB0 → 0x458560`.
Le scan peut donc encore accepter une consigne à une tête conservée ; aucun
second chemin d'acceptation à la tête annulée n'est confirmé dans ces exports.
Cette preuve statique du correctif de consigne est distincte de la validation
en jeu du prototype de commit différé.

### Qualification pure du dégagement arrière

`engine/route_rear_clearance.h` fournit `assessRearClearance`, un évaluateur pur
et borné. Une preuve positive exige l'identité complète du train, la révision
de composition, toutes les voitures attendues et toutes leurs portions
d'occupation, une route physique conservée et qualifiée, une entrée confirmée
et un certificat de tick stable partagé par les données. Les alias de conflits
restent séparés des portions effectivement parcourues ; aucun train n'est censé
avoir circulé sur une voie simplement protégée comme alias. Un contact avec la
borne d'une portion reste une occupation.

Le certificat doit venir d'un producteur qui prouve un tick natif terminé et
immuable. La fraîcheur, deux lectures externes identiques, une pause du jeu ou
le calcul `head - length` ne le fabriquent pas. Une observation incomplète,
périmée ou de tick différent conserve `Unknown` ; une occupation détectée peut
prouver `No`. **Aucun producteur natif de ce certificat et de l'inventaire complet
par voiture n'est raccordé actuellement.** Les fixtures valident seulement le
contrat de l'évaluateur, qui ne lit pas le jeu et ne libère lui-même aucun bail.

Ce contrat par voiture ne décrit pas le format de la table native agrégée.
Un contrat pur distinct est maintenant fourni par `engine/native_rear_coverage.h`.
Il reçoit une composition attendue complète, sa longueur métrique et la longueur
native accumulée en float, un curseur et un historique orienté avec leurs
identités, révisions et publication commune. Le fournisseur doit explicitement
qualifier cette publication achevée et possédée, la composition et chaque
connexion réellement empruntée. Le contrat ne transforme pas des relectures
identiques en preuve et ne reconstitue pas des voitures fictives depuis les
lignes d'occupation natives.

`native_rear::assess` produit une enveloppe agrégée complète ou `Unknown`, sans
allocation et dans les budgets fournis. Il conserve la plus grande des deux
longueurs qualifiées et exige la consommation de toute distance restante ; une
tolérance numérique ne rend pas un historique trop court complet. La distance
consommée est minorée, le reste majoré et la queue partielle arrondie vers
l'extérieur de l'enveloppe. L'évaluation refuse les régimes arithmétiques non
qualifiés : arrondis différents, FTZ/DAZ, sous-normaux utiles à la preuve ou
produit trop petit pour qualifier son résidu. Elle lit les contrôles flottants
sans les modifier et ne dépend pas d'un `long double` plus précis. Les branches,
directions, orientations, séquences et métriques des voies revisitées sont
contrôlées. Un résultat `Complete` qualifie l'enveloppe fournie, **pas l'entrée
dans une zone ni son dégagement**.

`engine/route_aggregate_clearance.h` fournit le raccordement pur
`assessAggregateRearClearance`. Il compare cette enveloppe aux portions physiques
de l'itinéraire original conservé, requalifiées dans la même publication après
mouvement (`RetainedMovement`). Les alias restent validés pour les conflits,
sans prétendre que le train les a parcourus. Une intersection, y compris le
contact à une borne, donne `No`. L'absence d'intersection donne `Yes` seulement
avec enveloppe complète, entrée confirmée et preuves de publication, identité
et composition communes ; une donnée manquante ou un budget dépassé donne
`Unknown`. L'évaluateur ne modifie lui-même aucun bail.

**Aucun fournisseur natif ne raccorde encore ces contrats à la simulation.**
L'observateur de fin de phase ne fournit pas leurs preuves. Il reste notamment
à qualifier les modèles absents, les omissions de couplage, la reconstruction
tardive de composition, les historiques tronqués, les buffers réutilisés et la
lignée complète des chargements/reset asynchrones. Les booléens de qualification
sont les obligations d'un futur fournisseur ; les valeurs synthétiques des
tests ne les établissent pas pour le jeu.

### Résultats locaux relevés et revalidation — 10 octobre 2026

| Contrôle | Résultat observé | Portée |
| --- | --- | --- |
| `native_rear_coverage` | PASS, 501 vérifications après correction numérique | Enveloppe agrégée pure : identités/composition, branches et bornes, longueur float, historique incomplet, arithmétique conservative, limites et absence d'allocation. Aucun lecteur natif raccordé. |
| `route_aggregate_clearance` | PASS, 151 vérifications après correction numérique | `Yes/No/Unknown`, contact, distinction des alias, budget, maintien du bail original après entrée/replan et libération sur preuve synthétique complète. Aucun passage natif commandé. |
| Campagne CTest initiale | 132 tests PASS, zéro test ignoré, 50,03 s | Contrats, adaptateurs et fixtures locales. `NIMBY_TEST_GAME_EXE` était configuré pour les contrôles de profil et la fixture UI avec binaire mappé ; aucun processus de jeu n'a été lancé. |
| Revalidation après le dernier correctif numérique | 5 tests PASS, zéro test ignoré, 0,14 s | `native_rear_coverage`, `route_aggregate_clearance`, `route_interlocking`, `route_rear_clearance` et `route_resources`, recompilation Release et régressions d'arrondi/SSE. |
| Campagne complète de clôture | 134 tests PASS, zéro test ignoré, 50,93 s | Release : corrections mémoire et composition, quotas de capture/préparation, ascendance TLS et collecteur autonome. Aucun jeu lancé. |
| `native_consist_inventory` | PASS, revalidation ciblée après précision des identifiants | Parcours complet synthétique, générations, parent/enfants, limites, mutations, chaque refus de lecture/préflight, sortie inchangée et absence d'allocation. Aucun lecteur raccordé au jeu. |
| `native_train_phase_observer` enrichi | PASS, 328 vérifications | Wrappers réels avec mémoire synthétique, appels uniques, refus d'ascendance, composition/attelage et readback final, refus à chaque frontière de lecture sans allocation ni reprise, erreurs C++ et Windows conservées. Pas de qualification ABI en jeu. |
| `native_phase_ownership` final | PASS, 5 397 vérifications | Scopes fixes, callsites, ressources, descripteurs, réentrance, unwind, récupération entre pas et refus des jetons expirés/étrangers. Diagnostic uniquement. |

La campagne de clôture reprend toute la suite après les correctifs numériques,
de quotas et d'ascendance. La régression `.8 → .2` sur une voie de 1 000 m refuse désormais une
couverture insuffisante malgré une égalité créée par l'arrondi, conserve le bail
et couvre le reste positif sur la section suivante lorsqu'elle est qualifiée.
Les preuves et résultats de l'état final se trouvent dans
`.validation/native-final-phase-ownership-20261010/summary.json` et
`completion-tests.xml`, puis `inventory-diagnostic-tests.xml` pour les dernières
régressions de diagnostic. Les résultats intermédiaires sont conservés séparément.

La réussite du scénario de bail central prouve les transitions du moteur pur
pour les données fournies. **Elle ne remplace ni les réservations natives, ni
leur maintien/libération, ni les permissions du jeu.** La validation en partie,
la couverture universelle des trains et les producteurs natifs de preuves
restent des portes de validation distinctes avant toute activation autonome.

## Architecture cible

L'autorité appartient au SDK chargé dans le processus du jeu, indépendamment des
workers de mods. Les mods publient leurs règles et reçoivent des décisions de
haut niveau ; ils ne fournissent jamais des pointeurs, une liste supposée libre
ou une commande de suppression de réservation.

Une demande interne contient la lignée du monde, l'ID complet du train, le signal
source, une révision de route, la trace orientée qualifiée, sa fraîcheur et le
résultat distinct des stages natifs. Le SDK construit les ressources depuis le
réseau qualifié. Les demandes simultanées sont arbitrées dans une transaction
courte : l'une acquiert toutes les ressources ou aucune. Aucun appel natif, mod
ou script n'est effectué pendant cette transaction. Une contention ou un budget
épuisé refuse seulement la nouvelle permission et conserve les baux existants.

Les index sont organisés par ID complet de voie, avec intervalles triés et
recherche bornée. Les conflits `+0xF8` ne sont décodés qu'une fois pour les voies
nécessaires d'une révision de réseau. Le cache n'est réutilisé qu'avec preuve de
lignée/révision native et de stabilité des identités ; sinon il est revalidé.
Le tick de mouvement lit une décision native, sans RPC Kotlin, recalcul du graphe
entier, comparaison de toutes les paires ou attente d'un autre mod.

Le transport de lecture expose : route qualifiée ou indisponible, état
d'autorisation, raison du refus, propriétaire bloquant, source, révision et
fraîcheur. `native stage clear`, `SDK granted`, `entered` et `rear clear` sont des
faits séparés. Le transport d'écriture reste une publication de politique bornée,
avec propriétaire de mod et incarnation de source ; une fermeture ou un crash
de mod ferme ses nouvelles permissions sans effacer une protection occupée.

### Cycle de vie et arrière du train

1. **Observed** : une approche est reconnue sur l'itinéraire réellement suivi,
   y compris la branche choisie derrière une aiguille. Une donnée manquante ne
   constitue pas une demande complète.
2. **Pending** : tous les alias de conflit sont qualifiés, le stage physique est
   disponible et les veto compatibles sont conservés. Le SDK tente l'acquisition.
3. **Granted, before entry** : la permission est liée à cette révision et à cette
   source. Un nouveau chemin peut remplacer le bail uniquement avec une preuve
   fraîche de non-entrée.
4. **Entered / retained** : le passage réel de la source est confirmé. La zone
   reste protégée malgré arrêt, pause, changement de route, disparition de
   source, crash de mod ou observation devenue inconnue.
5. **Rear clear** : une preuve fraîche porte sur le train entier et tous les
   intervalles physiques de la zone détenue. Le SDK libère le bail. Le passage de
   la tête, un timer, un `Path` absent ou une observation vide non qualifiée ne
   libèrent rien.

Il faut qualifier le calcul arrière avant activation : position de chaque voiture
sur la route conservée, couverture complète de la zone et absence d'intersection
physique, ou une autre preuve native équivalente. Les alias géométriques utilisés
pour exclure des itinéraires ne prouvent pas que le train a physiquement parcouru
la voie alias. Ils ne doivent pas rendre la preuve arrière impossible. Une simple
valeur `head - length` exige une route historique continue et des unités/positions
validées ; elle ne suffit pas après reversal, téléportation, composition ou replan.

La réinitialisation inconditionnelle des baux n'est permise qu'après preuve d'un
nouveau monde chargé. La suppression vérifiée d'un train est un événement de
cycle de vie distinct ; l'absence de son slot dans une lecture partielle n'en est
pas une preuve. Un retour du même ordinal avec une autre génération ne réutilise
jamais l'autorité du précédent train.

## Remplacement progressif et suppression complète

1. **Observation universelle** : ajouter un mode central indépendant de
   `Publication::active()` et des règles de mods, qualifier tous les producteurs
   de mouvement et transactions, mesurer couverture/latence sur les trains
   vanilla, ceux avec signaux de mods, reprise, manuel, changement de service et
   reversal. Les fonctions originales continuent à décider et écrire.
2. **Autorité parallèle, sans substitution** : construire les demandes, acquérir
   et maintenir les baux en parallèle. Comparer les décisions et enregistrer
   leurs différences. Les baux SDK ne commandent encore aucun passage.
3. **Veto SDK** : ajouter uniquement une fermeture lorsque l'autorité SDK manque
   ou refuse. Valider le point final et toutes les branches de franchissement ;
   ne pas limiter ce veto à `0x451B60` ou `0x448210`.
4. **Substitution d'admission** : une fois l'acquisition engagée, remplacer
   uniquement le stage d'exclusivité native attendu, pour cette demande qualifiée.
   Les contrôles d'occupation physique, géométrie et politiques de scripts restent
   distincts. Les writers natifs restent temporairement présents pour observer les
   écarts et conserver les consommateurs du jeu. Ce n'est toujours pas une
   suppression complète des réservations natives.
5. **Remplacement des transactions** : qualifier l'ABI et le contrat de
   `0x451C90`, `0x450B40`, `0x4512D0` et `0x451820`, puis remplacer leur rôle de
   réservation par les événements SDK. Conserver les travaux natifs de route,
   occupation et mouvement. Le hook bas `0x451F70` sert au contrôle de couverture
   des créations ; le neutraliser seul produit une transaction incohérente.
6. **Compatibilité des lecteurs** : fournir les données SDK aux consommateurs
   de réservation encore nécessaires (Extrapolator, inspecteur, probes de scripts),
   ou qualifier explicitement leur absence d'usage. Le changement doit conserver
   les formats, types et sémantiques des événements pris en charge ; aucun mod
   tiers ne doit croire qu'une réservation native vide prouve une voie libre.
7. **Activation autonome** : uniquement après couverture et preuve arrière,
   démarrer sur une frontière de monde qualifiée avec bootstrap des trains déjà
   présents. Arrêter les créations/maintiens exclusifs natifs dans leur rôle
   remplacé, sans écrire directement dans leurs tables. Les anciennes données
   ne sont jamais effacées manuellement pour forcer une activation en plein tick.

La disparition d'un bail actif ne peut pas se traiter par un retour automatique
au comportement natif : celui-ci n'a peut-être plus les réservations nécessaires.
En mode autonome, une erreur ferme les nouvelles permissions, conserve les
protections occupées et passe à un état diagnostiqué de récupération. Le retour
au mode natif nécessite une barrière coordonnée et un bootstrap validé, ou un
nouveau chargement du monde. Un mélange par train sans règles de coexistence peut
autoriser deux itinéraires opposés et n'est pas un mode de secours suffisant.

## Vérifications nécessaires avant activation

La campagne doit prouver deux demandes opposées/sécantes dans le même tick, les
quatre bras d'une zone d'aiguille, un train long dont la tête a quitté la zone
mais l'arrière y reste, arrêt dans la zone, replan avant/après entrée, reversal,
suppression de train/source/voie, recharge de sauvegarde et recyclage d'IDs.
Ajouter les limites, contention, erreurs de lecture, mutation en place de listes,
scripts tiers avec veto/propriétaire virtuel, worker de mod bloqué ou mort, et
le jeu sans aucun signal de mod. Les incompatibilités de profil ferment la
capacité entière ; aucune adresse de ce document n'est essayée sur un profil
non reconnu.

Les journaux sont bornés et émis aux transitions : session, train/source complets,
révision de route/réseau, résultat de qualification, acquisition/refus, ressource
bloquante, entrée réelle, maintien, preuve arrière et libération. Les totaux et
durées agrégés permettent de voir les demandes non observées sans produire un
message par train et par tick. Un résultat de budget ou une lecture absente garde
sa raison précise et n'est jamais converti en liste vide valide.

Les fixtures des conflits et du moteur central constituent une fondation. Elles
ne valident ni ces nouveaux points d'interception ni le remplacement natif en
jeu. L'activation autonome reste à implémenter et à mesurer.

## Reprise du 11 octobre 2026 — corps courant possédé et admission finale

Cette étape remplace le constat du 10 octobre « aucun lecteur de composition
complète raccordé » pour la **photographie physique courante seulement**. Elle
ne remplace pas les exigences de progression, d'entrée et de dégagement
historique. Les preuves statiques restent limitées au binaire identifié dans
le périmètre. Les exports et leurs empreintes se trouvent dans
`.validation/native-world-lineage-20261011/evidence.json`, avec les exports
complémentaires dans `.validation/native-admission-20261011/` et
`.validation/native-consist-rear-provider-20261011/`.

### Frontière native de propriété

La copie ne s'appuie plus sur deux lectures identiques sous un verrou partagé
pour exclure les writers. Le thunk `0x983C2C` libère le verrou partagé au site
`CALL 0x349F9E`, retour `0x349FA3`, dans le runner `0x3489D0`. Le wrapper appelle
la libération originale exactement une fois, puis tente sans attente
`TryAcquireSRWLockExclusive` sur le même verrou : `Root+0xA00`, égal à
`Controller+0x4C0`. Il ne tente aucun upgrade d'un verrou partagé encore détenu.
La relation Root/Controller, DB/Sim, le thread et la session courante sont
revérifiés après l'acquisition ; une contention ou une identité non qualifiée
ne produit aucune capture.

La propriété exclusive couvre la copie des dépendances du corps : inventaire
Motion, composition retenue et liens d'attelage, modèles de voitures, longueurs
métriques des voies et historique géométrique nécessaire au corps présent.
Le catalogue est accepté seulement si `RulesDB+0x408` désigne le catalogue
possédé inline à `RulesDB+0xA80`. Les références natives ne sortent jamais de
la capture ; le visiteur reçoit uniquement des valeurs POD possédées, pendant
la durée de sa visite. Il ne doit effectuer aucun callback natif ou mod,
attente ou formatage de journal sous le verrou.

`native_world_lineage.h` distingue la lignée du monde de chaque publication
possédée. Les hooks de stop `0x348960`, start `0x3488C0`, runner `0x3489D0`,
merge `0x478B20` et réinitialisation invalident leurs jetons avant les opérations
natives concernées. Les générations ne sont pas réutilisées ; une exception,
une réentrance incohérente ou une perte de contexte rend la preuve indisponible.
La lignée est encore revalidée à la fin de la copie. Aucune égalité de pointeur,
d'horloge ou de hash ne crée une propriété exclusive.

Lorsque les hooks sont chargés après l'entrée dans un runner déjà actif, la
première frontière exclusive effectivement possédée peut adopter une nouvelle
lignée vide, après vérification du Root courant et du thread natif. Elle ne
reprend aucun ancien bail ni aucune preuve de mouvement antérieure. Ce
bootstrap ne déclare pas les données immuables entre deux publications.

### Inventaire complet et photographie physique

`captureMotionInventoryOwned` parcourt le pool Motion une fois sous cette
propriété. Il conserve les identifiants complets, y compris leurs générations,
les racines, les trains attachés et les entrées inactives. Un parcours tronqué,
un lien manquant/cyclique, une identité périmée ou un budget refusé reste
`Unknown`. L'index par ordinal et les liens `firstChild`/`nextSibling` sont
validés avant utilisation ; la recherche de racine ne rescane pas tout le pool
pour chaque rame.

`native_owned_rear_snapshot.h` réutilise cet inventaire de la même publication
et produit un corps `CurrentPhysicalSnapshot` : voitures physiques, portions
orientées et métadonnées de publication. Les refus ne livrent aucune portion
ancienne ou partielle au visiteur. Le résultat indique explicitement
`currentPhysicalQualified=true`, `progressionQualified=false` et
`leaseReleaseQualified=false`. L'observation historique générique est retirée
de l'adaptateur pour éviter son emploi accidentel comme preuve de libération.

Le flag `Motion+0x1D0` discrimine la présence d'un état physique construit,
indépendamment de la vitesse et du flag de marche. `0x45B2D0` construit cet
état avant de lever le flag ; `0x43E300` le détruit avant de l'effacer.
`0x44B5B0` ne produit l'occupation que pour une racine active et non attachée.
La copie de présentation `0x43E3B0` emploie la même distinction. Sous la
photographie actuelle qualifiée :

- une racine inactive et non attachée donne une **absence physique courante**
  explicite (`qualified_current_body_absent`, `physicallyAbsent=true`) seulement
  si sa chaîne inverse complète d'enfants attachés est cohérente et inactive ;
  les descripteurs d'historique éventuellement laissés après destruction ne
  sont pas déréférencés ;
- une racine inactive avec un élément attaché actif devient inconnue, avec le
  motif `inactive_root_has_active_attached_member` et l'identité de l'élément
  concerné. Cet état incohérent ne permet jamais de retirer son occupation ;
- une racine active arrêtée reste occupante ; une vitesse nulle ne prouve
  jamais l'absence ;
- un train attaché inactif reste compris dans le corps agrégé de sa racine
  active. Son propre flag ne permet pas de retirer les voitures attelées.

Cette absence vaut uniquement pour l'identité et la publication possédées.
Elle n'autorise ni passage, ni dégagement d'un bail historique, ni extrapolation
vers la publication suivante. Une continuité couvrant relocation, reversal,
replan et modification de composition doit encore être qualifiée séparément.

Le bridge sous `NRF_INTERLOCKING_SHADOW=1` capture l'inventaire complet puis
**une seule racine non attachée, choisie en rotation**, pour ses diagnostics.
Il ne publie pas encore un atlas exhaustif des corps de tous les trains.
L'inventaire exhaustif n'est pas un certificat d'occupation globale lorsque
seul un corps a été copié. Ce mode est désactivé par défaut.

Le test du fournisseur reproduit ce dernier cas avec deux enfants attachés,
dont le premier visité est inactif et le suivant actif. L'ancienne implémentation
publiait à tort l'absence ; la correction refuse cette publication sans lire
les anciens descripteurs ni modifier les sorties. Le reçu avant/après figure
dans `.validation/inactive-root-member-20261011/receipt.json`.

### Collecteur interne d'occupation physique complète

`native_physical_atlas.h` ajoute un collecteur interne indépendant, vérifié
sur mémoire synthétique. Il capture le roster détenu puis le corps de chaque
racine, y compris les trains vanilla, arrêtés et sans ligne. Les éléments
attelés appartiennent uniquement à l'agrégat de leur parent exact. Toute
lecture, composition, identité, capacité ou qualification incohérente refuse
l'atlas entier ; aucun préfixe incomplet ne devient requêtable.

Les intervalles canoniques conservent leurs trous et leurs extrémités fermées.
Une requête exclut uniquement l'identité complète de la racine demanderesse,
jamais un ordinal ou une génération masqués. Capture et requête utilisent
le même budget terminal et requalifient ownership et contrôles flottants,
sans modifier ceux du caller. Une ancienne publication, un autre thread,
un owner libéré, la réentrance, DAZ/FTZ et les sous-normaux sont refusés.

Sa qualification ne vit que pendant Root Exclusive. `Clear` signifie absence
du corps d'un autre parent sur cet identifiant complet de voie ; cela ne prouve
ni l'existence actuelle de la voie, ni les aliases traversants, ni la fraîcheur
d'un itinéraire. Ce résultat n'accorde aucune permission, continuité historique
ou libération arrière. **Ce collecteur n'est raccordé ni au bridge ni à
l'admission centrale.** Le bridge conserve le diagnostic d'une racine décrit
ci-dessus. Il reste à qualifier le consommateur et sa fenêtre d'ownership.

Le stockage défaut réserve 3 084 400 octets au bootstrap, hors pile des workers,
pour 1 024 lignes, 8 192 slots observables et 16 384 intervalles. Les limites
locales des producteurs, le quota global et un délai externe peuvent refuser
une capture plus tôt. Il n'y a aucune promesse de capacité ou de temps en jeu.
Le test `native_physical_atlas` vérifie les vraies captures SDK sur une mémoire
synthétique scellée, avec des pannes de lecture, d'ownership et de budget,
ainsi que zéro tentative d'allocation sous manque de mémoire persistant.
La revue indépendante et les résultats bruts sont conservés dans
`.validation/native-physical-atlas-20261011/`.

### Admission finale et prototype de commit

Le wrapper `0x448210` observe la décision finale originale : stage physique et
réservations `0x451B60`, puis veto de script `0x4273D0`. L'original est exécuté
une seule fois. Un refus natif reste un refus. Pour une source gérée, le retour
est requalifié contre le monde/session, l'identité complète du train et du
signal, l'incarnation du propriétaire, la consigne fraîche inchangée et le
stage effectivement observé. Une publication pendant le callback ne réutilise
pas la permission précédente et aucun script n'est rejoué.

Le wrapper `0x447310` neutralise uniquement deux raccourcis positifs, aux
retours `0x448B29` et `0x4493E9`, pour une source gérée dans son pas courant.
Cela force le chemin original d'admission finale au lieu de réutiliser le seul
fait d'appartenance à une réservation. Les autres appelants restent inchangés.
Ces deux hooks sont compilés dans l'installation normale du bridge ; ils
conservent les contrôles natifs et ne constituent pas une autorité centrale.

Le prototype séparé `NRF_DRIVING_TRANSACTIONAL_COMMIT=1`, désactivé par défaut,
diffère le commit SDK jusqu'à conservation du résultat après le nettoyage natif.
Une tentative restaurée n'efface pas les protections antérieures. Une
qualification perdue peut verrouiller le mouvement de ce train : le wrapper
de `0x448710` consomme alors le **budget natif original du pas entier**, après
qualification du caller `0x44B36C`, du contexte, de la session et des identités.
Il appelle encore l'original exactement une fois. Les fixtures du vrai wrapper
couvrent également un chemin sans callback de signal ou d'intégrateur ; un
zéro passé dans une copie privée du budget ne suffit pas et n'est pas utilisé.
Ce verrouillage expérimental ne remplace ni réservation, ni preuve d'entrée,
ni preuve arrière. Son comportement réel en partie reste à vérifier.

### Vérification locale et suite de la recette

Les fixtures locales couvrent la vraie séquence release partagé / tentative
exclusive / libération, la lignée et ses pertes, le roster possédé, le corps
courant, les callbacks originaux uniques et leurs erreurs Windows, les refus
de lectures et de préflight, la saturation et l'absence d'allocation pendant
la copie. La première revalidation privée stricte du fournisseur du 11 octobre donne
**25 054 vérifications PASS**, dont les racines inactives, les rames arrêtées,
les attachés, les historiques détruits et les publications perdues. Ce nombre
décrit cette fixture précise, pas une nouvelle campagne CTest complète ni une
session de jeu.

La capture d'approche conserve maintenant le préfixe réellement émis par le
scan natif lorsqu'un premier signal géré apparaît après des signaux vanilla.
Le stockage POD est borné et propre au pas courant ; sa reprise revalide le
contexte, les identités complètes, la session, la tête, le chemin, la topologie
et les sources. Une limite, une mutation ou un contexte étranger laisse la
route indisponible, sans inventer un préfixe à partir du `Path`. La fixture
privée `firstManagedDiscoveryPrefix` vérifie le cas nominal, les invalidations,
les quotas et la réentrance. Ces résultats hors jeu ne mesurent aucun délai
d'ouverture en partie ; la recette d'ouverture reste à effectuer.

La prochaine recette en jeu doit contrôler bifurcation et convergence de
plusieurs branches, itinéraires sécants et opposés, rame attelée dont l'arrière
reste dans la zone, arrêt/reprise, replan, changement de sauvegarde et chargement
du bridge pendant une partie déjà active. Les compteurs doivent distinguer
inventaire complet, corps courant qualifié, corps absent, admission finale,
veto natif et preuve historique indisponible. **Aucun de ces nouveaux scénarios
n'a encore été exécuté en partie pour cet état du 11 octobre.** Le raccordement
d'un atlas courant complet et d'une progression historique fiable, puis le
maintien/libération des baux centraux, restent des étapes distinctes.

### Journal des mutations observées — complément du 11 octobre

Le diagnostic privé ajoute un journal borné des changements de composition et
un producteur avant le batch natif `0x30E020`. Ce wrapper conserve ses cinq
arguments, ses effets de bord, l’erreur Windows et son unique appel original,
y compris lorsqu’il lève une exception. Les retours `0x348F07`, `0x34928B` et
`0x3493D5` ne sont qualifiés qu’avec le root, le contrôleur, DB, Sim et le thread
du runner actuels. L’appel serveur `0x4F3A6F` ne reçoit pas cette propriété.
Le wrapper est utilisé seulement lorsque le diagnostic de phases est activé.

Les tuples conservés comparent les identités complètes, parent, ordre, mode,
activité, attachement et inversion, puis chaque CarSetup et les bits exacts
de sa longueur. Une révision de composition ne dépend pas du numéro de phase.
Le batch invalide le journal avant son original, sans décoder ni rejouer les
commandes opaques. Ni son succès, ni le retour à des octets précédents ne
réparent un ancien stamp. Perte de propriété, réentrance, quota dépassé et
erreur d’observation révoquent aussi la continuité concernée. Le journal
n’attend pas une autre capture et n’appelle aucun mod.

Le consommateur suit maintenant l’ordre décroissant de l’index inverse des
attelages produit par `captureMotionInventoryOwned`, comme le fournisseur du
corps physique. L’ancien contrôle acceptait un enfant seul, mais rejetait la
chaîne réelle lorsque plusieurs rames étaient attelées au même parent. Le test
`native_mutation_continuity` construit désormais ce catalogue avec le vrai
producteur d’inventaire et un lecteur de mémoire synthétique : deux enfants,
dont un inactif, restent présents ; l’inversion change la révision ; une chaîne
croissante invalide le stamp. Le témoin avant correction échoue, puis les
3 527 contrôles passent sans création de preuve historique ou de permission.

**Ce journal ne couvre pas tous les writers natifs.** SimCmd, les autres replans,
relocation, import et remplacement de génération demandent encore des
producteurs qualifiés. Son résultat `observedMutationsUnbroken` porte seulement
sur les mutations observées ; il ne produit aucune preuve de progression ni
de libération arrière. Il n’est pas raccordé à une autorité centrale active.

### Invalidation avant le recalcul `0x440840`

Le diagnostic observe désormais aussi ce recalcul, avant son appel original.
Les deux instructions CALL ont été relues directement dans le PE reconnu :
`0x447C21 → 0x440840`, retour `0x447C26`, et
`0x44816F → 0x440840`, retour `0x448174`. Le prologue de seize octets est
contrôlé par le bootstrap avant installation du hook, exclusivement sous
`NRF_INTERLOCKING_SHADOW=1`. Le binaire de référence conserve le SHA-256
`fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae`.

La signature transmet huit arguments opaques et un résultat `uintptr_t`.
Elle conserve les écritures natives, le refus natif, LastError et l'exception,
avec un seul appel original. Le recalcul peut s'exécuter sur un worker distinct
du runner : il n'obtient donc aucun scope Exclusive ou certificat de lecture.
Le journal invalide globalement son historique avant ce writer. La concordance
du monde courant, des deux identités complètes et du callsite distingue un
writer connu d'un contexte ambigu ; elle ne prouve pas une progression physique.

Une paire DB/Sim étrangère ne certifie pas des destinations étrangères : les
arguments Motion ou reservationBase peuvent encore viser le monde suivi. Ce
cas devient Lost, comme un contexte incomplet, une lecture refusée ou une
réentrance. Un journal occupé reçoit une invalidation différée plutôt que de
faire passer le writer pour un monde non initialisé. La réussite de l'original
et le retour d'octets identiques ne réparent aucun ancien stamp.

Les compteurs nommés `replan_440840` sont drainés avec les diagnostics de phases,
sans mise en forme pendant le callback natif. La fixture du wrapper réel passe
6 153 vérifications ; elle couvre notamment le worker distinct, les générations
recyclées, les destinations mixtes, le refus, l'exception et la contention.
Mille appels sur des régions synthétiques détenues utilisent 7 000 vrais appels
ReadProcessMemory et aucune allocation new/new[] instrumentée. Le mode
désactivé n'inspecte ni le caller ni la mémoire native. Ces essais ne prouvent
pas l'ABI en partie, la complétude des writers ou la libération arrière.

Les exports, instructions relues, sources et mesures sont conservés dans
`.validation/native-replan-continuity-20261011/validation.json`. Le test
`native_replan_continuity` compile désormais le vrai bridge via CMake ; aucune
autorité de réservation centrale n'est activée par cette étape.

### Revalidation de l'autorité et atlas groupé — 11 octobre 2026

L'autorité interne exige maintenant un validateur de possession courante dans
`PublicationWitness`. Les drapeaux déclaratifs seuls ne permettent plus de
préparer un monde ou d'accorder une permission. Le validateur appartient au SDK,
reste sans allocation, attente, RPC ou appel de mod et doit vérifier la fenêtre
exacte pendant toute la transaction. Il est revérifié avant la publication d'un
bail, son renouvellement, la validation d'entrée et la suppression des ressources.
La libération arrière le vérifie avant l'évaluation de couverture, sous la gate,
puis juste avant la suppression. Une perte laisse les anciennes protections
intactes. Ce contrat interne ne crée pas un producteur natif de possession.

La classification des fractions utilise leurs bits avant toute comparaison :
les subnormaux, infinis et NaN sont refusés. L'admission contrôle aussi l'arrondi,
DAZ et FTZ sans modifier les contrôles du jeu. Une reproduction privée accordait
trois admissions incorrectes avec DAZ/FTZ avant correction ; aucune n'est accordée
après correction. La fixture complète de l'autorité passe 340 contrôles. Une
fixture distincte passe 32 contrôles de possession, dont la perte pendant la
préparation, le thread étranger, la libération arrière et le changement de monde.
Le témoin de possession ajoute seulement le schéma du validateur à l'ancien
contrat : il reproduit l'acceptation déclarative avant ajout des contrôles, sans
revendiquer une ancienne connexion de production.

L'atlas interne ajoute `queryProtected`, qui copie et valide jusqu'à 512 portions
fermées avant de chercher un conflit. Un conflit sur la première portion ne peut
donc masquer une portion suivante invalide. La requête conserve la même preuve,
la racine avec génération complète, la gate et le même budget global terminal.
Le budget local vaut 65 536 opérations par défaut, avec plafond de 262 144.
Les identités périmées, pertes de possession, contrôles arithmétiques inconnus et
quotas dépassés rendent le résultat inconnu. Les portions propres de la racine
sont exclues ; les portions des autres rames restent prises en compte.

Cette nouvelle requête évalue seulement les portions fournies. Elle ne certifie
ni l'existence actuelle d'une voie, ni la complétude d'un itinéraire ou de ses
aliases. Elle ne produit ni permission ni libération arrière. Sa publication
reste explicitement `RetainedMovement`. Le stockage fixe construit au bootstrap
contient 12 288 octets de scratch supplémentaires ; aucune allocation ni lecture
native par portion n'est ajoutée pendant une requête.

**Le remplacement complet reste non raccordé.** La capture de production sous
`PostSharedExclusive` se termine avant la prochaine admission. Son propriétaire
et son atlas courant ne peuvent pas être réutilisés comme une capture
`BeforeMovement`. Les déclarations suivantes restent fausses :
`nativeReplacementConnected`, `nativeWritersComplete` et `leaseReleaseQualified`.
Ni `NativeAdmissionAuthority` ni `FinalAdmissionScope` ne sont instanciés dans
`sdk/src`. Le refus natif précédant le script ne peut pas être traité comme une
preuve de veto script terminé.

Pour raccorder réellement le remplacement, il manque une fenêtre possédée avant
le mouvement et maintenue jusqu'au commit, la qualification de toutes les
ressources et aliases, le résultat complet du script, une preuve d'entrée
conservée et une continuité historique couvrant tous les writers jusqu'à la
libération arrière. Les lecteurs et transactions exclusifs natifs ne sont pas
tous redirigés. Aucun changement de drapeau, renommage de phase ou simple
branchement de l'atlas ne résout ces exigences.

Les reçus privés sont conservés dans
`.validation/admission-arithmetic-20261011/`,
`.validation/admission-ownership-20261011/` et
`.validation/atlas-protected-spans-20261011/`. Ces essais hors jeu portent sur
des contrats SDK et des lecteurs synthétiques ; ils ne mesurent pas la latence
en partie et ne prouvent pas le remplacement natif.

### Réservation provisoire et admission finale — 11 octobre 2026

Le contrat interne sépare désormais la protection des ressources de la permission
de mouvement. `claimProvisional` copie une route et ses conflits complets dans
un bail provisoire, sous une fenêtre `BeforeMovement` possédée et courante.
La protection peut précéder la lecture physique et le script, mais son résultat
ne permet jamais le passage ni le remplacement d'un refus exclusif natif.
Le jeton conserve les identités complètes, la révision du train et de la route,
le numéro unique du bail, la publication et la continuité exigée.

Après acquisition de la gate et reconnaissance du jeton exact,
`finalizeProvisional` consomme sa tentative avant les vérifications nouvelles.
Un refus `Busy` ne consomme pas un jeton créé directement par `claimProvisional`.
Le coordinateur décrit ci-dessous ferme cette possibilité dès la publication.
La finalisation exige une nouvelle capture complète dans la même fenêtre, la même route, les mêmes
ressources et composition, une occupation libre et un veto script terminé.
La possession courante et les contrôles arithmétiques sont vérifiés avant la
permission. Un résultat natif nul reste nul : il ne certifie pas que le script
a été exécuté. Une exception, un changement d'identité, une perte de possession
ou un budget dépassé conserve les ressources déjà protégées.

Le coordinateur `two_phase_admission::invoke` appelle l'original et collecte les
faits au plus une fois chacun, hors de la gate de l'autorité. Un échec de
réservation ne supprime pas les effets normaux de l'appel original, mais ferme
la permission SDK. Sa publication consomme déjà la tentative publique avant
l'original et arme un permis privé, non copiable, limité à cette invocation.
Le jugement consomme ce permis avant de tenter la gate. Un refus natif, une
exception ou une contention après l'original ne laisse donc aucun jeton
réutilisable pour une deuxième finalisation. Une clôture tardive qui pouvait
elle-même perdre la gate n'est pas utilisée. Le coordinateur n'installe aucun
hook et ne fabrique ni propriétaire de phase ni preuve historique native.

L'annulation avant entrée est une opération distincte. `cancelBeforeEntry`
exige une preuve positive de non-entrée, le retour de l'original, l'absence de
permission encore utilisable et un historique complet de tous les writers
d'entrée depuis le jeton jusqu'à la fenêtre courante. Un délai, un refus natif,
une position actuelle inchangée ou le journal d'observations existant ne
remplacent pas cet historique. Une entrée effectivement confirmée interdit
cette annulation ; la libération requiert alors la preuve du passage de la queue.

Ces types restent privés au SDK. Aucune instance de l'autorité, de son
coordinateur ou de sa fenêtre `BeforeMovement` n'est raccordée dans `sdk/src`.
`nativeReplacementConnected` reste faux. Le contrat exécutable prépare la
transaction, mais le remplacement des lecteurs et writers natifs demeure à faire.

### Index borné des propriétaires et des ressources

Les lectures d'un bail existant utilisent un index du propriétaire et une chaîne
locale de ses portions. Les identités de session, train et source restent entières
sur 64 bits ; seuls les indices internes des buffers fixes utilisent 16 bits.
Les plafonds du constructeur restent de 8 192 baux et 32 768 lignes. La sentinelle
des chaînes reste distincte de toute ligne valide.

Le cache ne fait pas autorité sur l'absence d'un propriétaire : un échec de
recherche contrôle aussi tous les baux. Un propriétaire absent de l'index alors
que son bail existe ferme le résultat avec `IndexInvalid`, sans créer de doublon
ni réparer silencieusement le cache. Chaque bail conserve ses cardinalités
originales, contrôlées avec celles de sa chaîne. Une chaîne tronquée, périmée,
cyclique ou attribuée à un autre bail ne peut produire une permission ni un
instantané partiel. La sortie d'un instantané refusé reste inchangée.

La recherche commence par au plus 64 sondes de hash. Atteindre ce seuil ne
signifie jamais que le propriétaire est absent : le scan complet des baux est
alors nécessaire. S'il retrouve le propriétaire, la recherche reprend le hash
au point d'arrêt et exige son nœud exact. Les collisions au-delà de 64 restent
donc valides, tandis qu'un nœud effectivement manquant reste refusé. Si aucun
bail n'existe, une vacance déjà qualifiée peut servir au staging sous la même
gate, sans reparcourir tous les tombstones. Une vacance ne prouve aucune liberté
physique des voies. La qualification porte sur les cellules parcourues, pas sur
l'intégrité globale des cellules étrangères non lues.

Les mutations préparent les lignes et les têtes dans des buffers distincts.
Toutes les chaînes touchées sont vérifiées avant l'échange final, sous la même
gate, après revalidation de la possession. Un refus de budget laisse les lignes,
propriétaires et protections publiés intacts. L'époque du scratch avance même
après un refus pour empêcher une réutilisation de données intermédiaires ; son
épuisement interdit une nouvelle mutation. Les publications et permis privés de
la transaction en deux phases conservent leurs contrôles précédents.

Les ressources sont allouées au bootstrap. Les opérations n'ajoutent aucune
allocation ni attente. Les collisions et tombstones restent soumis au budget
global : une table fortement remuée peut exiger un parcours complet et rendre
un budget trop court insuffisant. Les acquisitions nouvelles et suppressions
conservent aussi une préparation proportionnelle à l'index global ; cette
optimisation n'en fait pas des opérations à coût constant.
