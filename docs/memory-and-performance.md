# Mémoire, ressources et performances

## Propriété et durée de vie

| Ressource | Propriétaire | Libération / validité |
| --- | --- | --- |
| Connexion processus | Session du registre d'observation | Retrait de session puis fin de ses opérations en cours |
| Handle Windows du processus / snapshot de modules | `windows::UniqueHandle` | Destruction ou `reset`, y compris sorties anticipées |
| Données d'une capture | Capture du registre | `NimbyInternal_ReleaseSnapshot` |
| Copie C++/JVM d'une observation | Objet consommateur | Destruction de l'objet / collecte JVM |
| Buffers temporaires de lecture | Parcours/capture courant | Fin de l'opération |
| Versions des commandes de textures | Runtime du pont | Rendu par version immuable épinglée ; retrait/destruction sur le producteur |
| Publications de conduite | Runtime du pont | Version immuable épinglée par callback ; retrait/destruction sur le producteur |
| États de conduite des trains | Monde de la publication | Emplacements stables, identité complète et verrou propre à chaque train |
| Bibliothèque native chargée | Appelant de `load` ou `existing` | Un `unload` par référence acquise |
| Bibliothèque épinglée | Processus | Fin du processus ; requis pour certains hooks et threads Kotlin |

Une ressource volontairement conservée jusqu'à la fin du processus doit avoir un
commentaire expliquant pourquoi, et un arrêt logique distinct de son déchargement.
Ne pas remplacer ces rétentions par des destructeurs qui attendent un thread dans
`DllMain`. Ne pas employer `UniqueHandle` pour un `HMODULE`, une clé de registre
ou un handle de recherche `FindFirstFile` : leurs fonctions de libération diffèrent.

Le callback `ReadMemory` copie vers un buffer de l'appelant. Les pointeurs lus dans
le jeu sont des adresses à vérifier, jamais des objets C++ possédés par le SDK.
Ne jamais détruire un `std::string` ou `std::vector` du jeu avec notre runtime.

## Changements de cette refonte

Les collections de captures réservent leur capacité à partir de tailles déjà
vérifiées. Les itinéraires et arrêts de ligne locaux sont transférés vers la
capture après validation, au lieu de créer une copie supplémentaire. Les tableaux
d'occupations, de réservations et certains résultats C++ réservent également leur
taille connue. Le décodeur des réglages réserve seulement après contrôle du nombre
d'éléments.

Ces changements réduisent des réallocations et copies identifiables dans le code.
Ils ne constituent pas une mesure de gain en jeu. Ils ne mettent pas en cache un
ancien monde et ne suppriment aucun contrôle de cohérence. Après une tentative
incohérente, les données sont relues, jamais fusionnées avec une ancienne capture.

## Concurrence

Le mutex de registre protège les identifiants de sessions et captures. Les lectures
distantes utilisent un verrou par session : une capture lente ne garde pas le
registre global verrouillé. La fermeture invalide la session ; une capture déjà
en cours ne peut pas publier son résultat après cette fermeture. Chaque session
dispose de son propre quota de 16 captures retenues, préparation comprise ; le
registre de cette instance du SDK accepte 32 sessions ouvertes. Les captures
publiées avant fermeture restent valides et doivent être libérées explicitement. Ouvrir puis fermer des
sessions sans libérer leurs captures peut encore accumuler de la mémoire : ces
quotas ne sont pas un plafond global en octets ni en captures historiques.

Les callbacks du jeu doivent rester courts et ne doivent pas laisser remonter
d'exception. Pour les textures, la préparation et la destruction des versions de
table restent hors du rendu. Le callback consulte une version immuable sans
attendre le verrou d'un producteur. Un producteur peut être refusé lorsque le
nombre de versions retirées atteint huit pendant qu'un lecteur les épingle.
Une publication garde au plus 4096 images par propriétaire, 64 propriétaires et
65536 images vivantes au total. Chaque préparation clone la version courante :
même un renouvellement d'un seul propriétaire a donc un coût de copie dépendant
de la taille globale. Ce travail reste hors du rendu, mais son allocation et sa
bande passante mémoire ne sont pas gratuites. La dernière version visible n'est
pas perdue sur contention ; ses gardes de monde et son expiration restent actives.

Les hooks de conduite lisent une publication immuable sans acquérir le verrou
des producteurs. Un court compteur d'entrée protège le passage du pointeur
publié à son compteur de lecteurs ; chaque callback épingle ensuite uniquement
sa version. Au plus 32 versions retirées sont conservées. Une saturation refuse
la nouvelle publication en conservant la version courante, ses baux et ses
restrictions. La préparation des tables et la destruction des versions retirées
restent sur le producteur, hors de sa section critique.

L'état mutable possède un verrou par train, acquis une seule fois sans attente.
Le garde reste détenu pendant l'intégration native et sa validation finale ; un
callback réentrant du même thread emprunte le garde existant. Un train voisin
peut donc progresser indépendamment. Une contention sur le même train géré reste
restrictive, sans transformer cette contention en arrêt d'un train hors mod.
Les trains jamais concernés par une consigne n'allouent pas d'état de conduite.
Un index d'intérêt par monde conserve leur distinction ; chaque scan et chaque
vérification de signal consultent les publications courantes, y compris après
l'ajout d'une consigne sur un parcours auparavant non géré. Le registre possède
8192 emplacements stables et limite chaque recherche à 128 cases. Une saturation
ne libère pas une restriction d'un train géré.
L'index couvre 1 048 576 ordinaux natifs. Un ordinal hors borne, ou devenu ambigu
après des observations concurrentes de générations différentes, reste considéré
comme potentiellement géré jusqu'au prochain monde. Si le registre est aussi
saturé, ce cas reste restrictif pour ce train ; l'absence d'état ne prouve pas
l'absence d'une consigne.
Sur la compilation Windows x64 validée le 10 octobre 2026, la structure de monde
occupe 21 299 200 octets (20,3125 Mio), dont 8 Mio pour l'index d'intérêt ; s'y ajoutent les vecteurs
des trains effectivement observés. Les publications successives du même monde
partagent cette structure. Un callback encore épinglé lors d'un changement de
monde peut prolonger la vie de l'ancienne structure. La borne de 32 versions
retirées ne constitue donc pas, à elle seule, un plafond global de mémoire du
processus ou des préparations concurrentes.

Les producteurs seuls partagent, pour chaque appel, un budget de réessai de
4096 échecs ou 2 ms observées sur leurs acquisitions, selon la première limite
atteinte. Cela ne plafonne pas la durée totale de l'appel, qui inclut préparation
et ordonnancement Windows. L'expiration d'origine est revérifiée au commit et
n'est jamais prolongée par un réessai. Un `ResourceLimit` ne doit pas être masqué
comme une nouvelle décision valide : une ancienne permission peut différer de
celle qui vient d'être refusée.

Le nettoyage est limité au propriétaire et conserve les demandes échouées pour
réessai. Les services indépendants sont tous nettoyés même si l'un échoue ; les
baux non renouvelés continuent d'expirer. Un retour au rendu natif à expiration
ne prouve pas une indication restrictive.

Le retrait d'une inscription d'options reconstruit un instantané immuable et
peut échouer faute de mémoire. Le SDK conserve alors son identifiant dans une
file fixe de 64 emplacements, réservés avant toute inscription. Un seul timer
SDK poursuit le nettoyage même après l'arrêt du dernier mod, avec au plus
quatre tentatives par tick de 10 ms. Il ne garde aucun pointeur ou callback du
mod et appelle le registre hors du verrou de la file. Les journaux décrivent
le premier report, la saturation et la reprise complète, sans message à chaque
réessai. Une pression mémoire persistante peut retarder la suppression ; la
file ne transforme pas cet échec en réussite et refuse les nouvelles
inscriptions lorsqu'elle est pleine. Les tests injectent un véritable échec
d'allocation dans le registre et vérifient la reprise du même mod ainsi que les
options, raccourcis et événements d'un autre mod.

## Ressources des processus de mods Windows

Le chargeur calcule les limites avant de démarrer le lot de mods. Avec `N` mods,
chaque processus reçoit au plus un processeur logique et `25 % / N` du CPU de la
machine. Sa priorité reste inférieure à celle du jeu. Ces allocations sont fixes :
le nombre de mods activés influe sur les limites au démarrage, mais un mod en
boucle ne prélève pas de jetons de requêtes ou de CPU dans le quota d'un voisin.

L'admission vérifie séparément la RAM disponible au démarrage et la capacité
d'engagement. Le plafond `ProcessMemoryLimit` d'un Job Windows limite la mémoire
virtuelle engagée ; ce plafond n'est pas une allocation physique déjà effectuée.
Voir les définitions Microsoft de
[ProcessMemoryLimit](https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-jobobject_extended_limit_information)
et de [MEMORYSTATUSEX](https://learn.microsoft.com/en-us/windows/win32/api/sysinfoapi/ns-sysinfoapi-memorystatusex).

Le budget physique est le plus petit du quart de la RAM totale et de la moitié
de la RAM immédiatement disponible. Il doit couvrir, par mod, le canal partagé,
deux copies maximales de message et une marge de démarrage privée de **128 Mio**.
Le canal et ses copies représentent environ **48,19 Mio** avec le protocole
Windows x64 actuel. La marge de 128 Mio est un choix de politique conservateur,
pas une mesure du working set et pas une limite de RAM imposée pendant toute
la session. Elle laisse au moins la moitié de la RAM disponible hors du budget
de démarrage.

Le budget d'engagement est le plus petit du quart de la RAM totale et de la
moitié de l'engagement disponible. Il couvre les mêmes canaux/copies et les
plafonds privés des enfants. Chaque processus reçoit une part égale, arrondie
vers le bas à 64 Kio, comprise entre **192 Mio et 1 Gio**. Le minimum de 192 Mio
sert à l'admission d'engagement, sans être considéré comme de la RAM déjà
occupée. Quatre mods avec 2 305 Mio disponibles pour chacun des deux budgets
reçoivent toujours environ 240 Mio de plafond privé chacun. Une disponibilité
d'engagement supérieure peut augmenter ce plafond sans modifier la marge de
démarrage physique.

Le refus observé le 9 octobre 2026 à 20:38:44 UTC portait sur quatre mods avec
1 578 536 960 octets de RAM disponible et 14 560 391 168 octets d'engagement
disponible. L'ancien calcul refusait le lot en imputant les minima privés au
budget physique. Le nouveau calcul vérifie une réserve physique de
738 994 720 octets dans un budget de 789 268 480 octets ; le budget d'engagement
autorise les quatre plafonds privés de 1 Gio. Un test reprend ces valeurs
exactes. Cela vérifie la décision d'admission, pas les performances d'une
session de jeu sous cette pression mémoire.

Le test hosted du mod Kotlin de déclaration seule a mesuré un pic privé de
6 066 176 octets lors de la validation locale ABI12 du 10 octobre 2026. Ce
scénario couvre options, démarrage et arrêt ; il ne mesure pas les captures d'un
grand réseau ni les besoins de tous les mods. Les journaux du superviseur
ajoutent au plus un échantillon à l'état `startup_ready` et un à la sortie
normale ou anormale du processus : RAM résidente, pic résident, engagement
privé/pic, pic du Job et plafond configuré. Les échecs de mesure sont explicites.
Le working set contient aussi des pages partagées ; ne pas l'ajouter à la mémoire
privée comme deux allocations indépendantes.

Si l'un des budgets ne tient pas, ou si le lot dépasse 32 mods, aucun processus
du lot n'est lancé et le chargeur signale `ResourceLimit`. Les diagnostics
distinguent `physical_memory_budget`, `commit_memory_budget` et un échec de
mesure Windows. Ils conservent les deux budgets et leurs minima, la marge de
démarrage et le plafond privé attribué. Le retour Options V1 affiche la paire
du budget réellement refusé. `ullAvailPageFile` est la capacité d'engagement
du processus appelant, éventuellement inférieure à celle du système entier.

Ces calculs ne réservent ni RAM ni engagement, ne plafonnent pas toutes les
allocations du jeu et ne garantissent pas l'absence de pagination. Un mod peut
croître après le démarrage jusqu'à son plafond privé et la disponibilité du
système peut changer. Fermer des applications ou activer moins de mods peut
réduire cette pression. Les quotas CPU/RPC et l'isolation entre processus restent
indépendants des deux budgets mémoire.
L'onglet NRF Hub est installé indépendamment du démarrage des mods et affiche
la cause d'un refus du lot, lorsque le pont Options est disponible pour le jeu.
Chaque manifeste `nrf-mod.ini` est lu avec une limite de 64 Kio, BOM compris,
avant son décodage UTF-8 ou UTF-16 ; un dépassement est refusé et journalisé.

Les requêtes ont des quotas indépendants, répartis au démarrage : au plus
6 000 requêtes et 128 Mio par seconde pour le lot, avec des plafonds individuels
de 1 500 requêtes et 32 Mio par seconde. Les capacités de réponse demandées
comptent dans les octets. Un canal garde une réserve ponctuelle de deux messages
maximums, soit environ 32 Mio, afin de laisser passer une grande opération.
Les courtes rafales de requêtes sont bornées à 1 024 pour le lot.

Le temps CPU de traitement des requêtes est également débité au canal appelant :
au plus un dixième de processeur logique par canal et 5 % du CPU de la machine
pour le lot, avec une réserve ponctuelle de 50 ms par canal. Un traitement déjà
commencé doit terminer ; le courtier ne préempte pas son propre code SDK. Les
traitements SDK doivent donc rester bornés. Les rejets et signaux de requête
incorrects sont temporisés seulement sur le canal concerné.

Ces limites réduisent la contention ; elles ne garantissent pas une latence
nulle. Les caches matériels, la bande passante mémoire, les pilotes et les autres
applications restent partagés. La disponibilité mémoire peut changer après
l'admission. Les tests `isolated_mod_faults` vérifient les limites appliquées par
Windows, les refus, plusieurs producteurs fautifs simultanés et la progression
d'un processus sain, sans lancer le jeu.

## Campagne de mesure reproductible sous Windows

Utiliser une compilation Release et conserver la version du SDK, du jeu, son
SHA-256, la sauvegarde et le scénario. Répéter le même scénario avant/après :
démarrage, observation pendant plusieurs minutes, changement de sauvegarde,
arrêt/reprise des mods, puis fermeture du jeu.

Relever la mémoire privée et le pic, le nombre de handles, la durée des captures
(médiane et percentiles élevés), les allocations par capture et le temps des
callbacks. Distinguer mémoire conservée par l'allocateur, données vivantes et
ressources explicitement épinglées. Une hausse du working set seule ne prouve pas
une fuite.

Ne pas optimiser un point sans scénario qui l'exerce. Garder les mêmes options de
compilation et comparer aussi les erreurs/données indisponibles : accélérer une
capture en omettant des validations n'est pas un résultat acceptable.

## Tests utiles

`windows_resource_lifetime` vérifie les transferts de handles et la stabilité du
nombre de handles après des ouvertures refusées. `calendar_update` vérifie les
annulations, y compris une écriture partiellement réussie. `texture_runtime` teste
une table importante et les changements de monde. Les tests réseau utilisent une
mémoire simulée changeante pour vérifier les lectures et leurs refus.

Ces tests ne prouvent ni l'absence générale de fuite ni la performance d'une
session réelle. La validation longue en jeu reste à réaliser séparément.

## Diagnostic de fin de phase des trains — état du 10 octobre 2026

Cette section conserve les tailles et mesures du diagnostic initial. Le
raccordement possédé, les buffers et les limites du 11 octobre sont décrits
dans la section suivante datée du 11 octobre ; les mesures ci-dessous ne sont
pas celles de cette nouvelle capture.

Le mode privé `NRF_INTERLOCKING_SHADOW=1` ajoute une capture après la phase
native des trains, uniquement pour le profil binaire reconnu. Il est désactivé
par défaut. L'ascendance TLS vérifie les quatre fonctions et leurs callsites
exacts, puis les références de contexte avant et après la capture. Un autre
thread ne peut pas emprunter cette ascendance. Une capture visite au plus
8 192 emplacements SDK et tente au plus
32 trains suivis ; elle ne parcourt pas tous les trains du jeu et ne prétend
pas couvrir une composition complète. Un verrou occupé est ignoré immédiatement.

Les valeurs copiées sont conservées dans des tableaux fixes : 128 entrées
d'historique, 128 voitures conservées et 64 enregistrements d'attelage au plus
par train, 32 résultats temporaires et huit échantillons en attente. Les
copies de composition et d'attelage sont confirmées octet par octet avant
publication ; les deux copies de ces trois listes occupent au plus 16 Kio de
stockage fixe temporaire. Une liste trop grande est refusée. Les tentatives
partagent un budget de 8 192 lectures, 4 Mio et
2 ms observées. Chaque lecture consomme ce budget ; la vérification des
emplacements vides ne consulte l'horloge que tous les 32 emplacements.
Les vérifications de contexte aux entrées des wrappers, avant et après
l'original, restent des préfixes de lecture de taille fixe hors de ce budget ;
le readback final de la capture partage son quota.
Ce budget est coopératif et ne borne pas une suspension par le système.

Les résultats ne deviennent disponibles qu'après les dernières vérifications
du budget, du contexte et de l'epoch de réinitialisation. Une capture abandonnée
ne publie aucun échantillon. Les compteurs de raisons décrivent les tentatives,
y compris celles abandonnées après une lecture localement cohérente. Les
échantillons portent leur propre epoch et numéro de phase d'observation.
Le formatage et l'écriture du journal restent dans le lecteur d'observation,
après libération des verrous des trains. Les journaux sont espacés de 30 s.

Le test synthétique `native_train_phase_observer` vérifie notamment l'absence
d'allocation de tas pendant la capture, l'appel natif effectué une seule fois,
les lectures instables, la saturation et l'invalidation pendant une
réinitialisation. Cela ne qualifie pas l'ABI en jeu ni un tick global immuable.
Ces données restent diagnostiques : elles ne changent aucune permission native
et ne fournissent pas de preuve de dégagement de l'arrière d'un train.
Les compteurs de voitures et les liens d'attelage décrivent uniquement les
listes locales du jeu ; ils ne les transforment pas en inventaire exhaustif.

Le collecteur privé autonome `native_consist_inventory.h` prépare séparément
le parcours de tous les slots Motion, sans filtrer les trains attachés ou inactifs.
Il ne s'exécute pas dans le callback actuel du jeu. Ses buffers appartiennent au
caller : 160 Kio pour 1 024 lignes et 8 192 slots, au plus 896 Kio aux plafonds
structurels. Son payload temporaire borné sur la pile est de 6 240 octets, hors
scalaires, métadonnées locales et choix du compilateur. La capture n'alloue pas
sur le tas et conserve la sortie précédente en cas de refus. Les plafonds de
slots/lignes restent distincts du budget de lectures : un grand pool peut être
refusé avant la fin du parcours. Les tests ne prouvent pas de coût en jeu ni
une composition physique complète ; aucun nouveau budget n'est ajouté au tick.

### Mesure isolée de la capture

Un microbenchmark du 10 octobre 2026 utilise `ReadProcessMemory` sur des données
synthétiques résidentes dans son propre processus : 200 captures d'échauffement,
puis cinq répétitions de 500 captures pour chaque forme. Sur un Core Ultra 9
275HX, Windows 11 10.0.26300, avec GCC `-O2`, les médianes des percentiles de
ces répétitions sont :

| Données locales | Lectures / octets par capture | p50 | p95 | p99 | Plus grand pic observé |
| --- | --- | --- | --- | --- | --- |
| Listes vides | 33 / 381 | 6,3 µs | 6,7 µs | 6,8 µs | 9,8 µs |
| 3 voitures, 1 attelage, 1 section historique | 39 / 669 | 7,6 µs | 8,1 µs | 8,2 µs | 129,4 µs |
| 128 voitures, 64 attelages, 128 sections historiques | 39 / 16 765 | 21,7 µs | 24,6 µs | 60,2 µs | 96,8 µs |

Le scratch n'est pas initialisé en entier : seuls les préfixes effectivement
copiés et validés sont utilisés. La variante qui initialise les six tableaux
donne des p50 de 6,3 / 7,7 / 22,0 µs. Cet essai ne démontre pas un gain
statistique et ses pics varient ; il ne justifie aucune promesse de latence en
jeu. Il n'inclut ni le budget et l'ascendance du callback complet, ni les verrous,
les appels natifs, les rechargements ou les grandes cartes. Les résultats bruts,
la machine, les sources et les empreintes des exécutables sont conservés dans
`.validation/native-consist-inventory-20261010/capture-benchmark/evidence.json`.

## Coût du diagnostic des conflits de voies

Le même mode privé possède un budget coopératif distinct de 4 ms, 8 192
lectures et 2 Mio pour une capture d'itinéraires. Un préflight interne vérifie
le travail et la taille prévue avant les allocations ou lectures. Un refus
est terminal : une petite lecture ultérieure ne peut pas rouvrir le budget.
Le résultat incomplet du lot est abandonné ; il ne déclare aucune voie libre.
Un défaut de lecture ordinaire reste local si le budget est encore disponible.

Le décodeur réutilise un seul scratch contenant deux copies de la plus grande
liste native demandée, au plus deux fois 4 096 lignes de 48 octets. Il réutilise
aussi ses lignes décodées entre les voies, et sa table de readback utilise un
tableau borné. Une liste dont la lecture dépasserait le quota ne crée pas son
scratch. Ces buffers vivent seulement pendant la requête, sans cache permanent.

La préparation des ressources, le tri et la soumission du plan partagent le
préflight. Les boucles sans allocation le consultent au plus tous les 32
éléments ; chaque allocation reçoit un contrôle supplémentaire. Le tri
budgeté utilise un heapsort sans scratch et peut s'arrêter dans son parcours.
L'expiration annule le plan entier, conserve les baux déjà entrés et les
événements en attente ; elle ne réinitialise pas la lignée. Les allocations,
lectures et interruptions système déjà commencées ne sont pas préemptées.

Les fixtures vérifient notamment le refus avant toute allocation, la réutilisation
des buffers, l'arrêt au milieu d'une liste ou du tri, l'abandon d'un résultat
précédent devenu partiel et la rétention des protections existantes. Elles ne
mesurent pas la latence d'une grande carte dans le jeu.

## Capture possédée et admission — état du 11 octobre 2026

Le mode privé `NRF_INTERLOCKING_SHADOW=1`, désactivé par défaut, ajoute maintenant
une capture après la libération native du verrou partagé, au retour `0x349FA3`.
Le wrapper libère d'abord le verrou original exactement une fois, puis tente
sans attente de prendre le même verrou en exclusif. Une contention abandonne
la tentative ; aucun upgrade ni attente supplémentaire n'est effectué. La
copie dépend de cette propriété réelle, du thread et de la lignée du monde
revalidés, pas de deux lectures identiques. Le détail de la qualification est
dans [les notes d'itinéraires](research/sdk-route-reservations.md).

La tentative exclusive de diagnostic est espacée d'au moins 100 ms de temps
réel par thread du runner. Inventaire et corps courant partagent le budget
coopératif de **2 ms, 8 192 lectures et 4 Mio** ; les dépendances, contrôles
finaux et refus de lecture sont débités. Une fois refusé, le budget ne peut
pas reprendre sur une petite lecture. Ce quota ne préempte ni un appel mémoire
déjà commencé ni une suspension du thread par Windows, et n'est donc pas une
garantie de temps de verrouillage maximal.

Le stockage `nimby::windows::owned_rear::Storage` est stable et alloué au
bootstrap du bridge. Sur la fixture Windows x64 compilée, il occupe
**2 248 376 octets**. Il contient le scratch et la sortie POD séparés ; il
n'est ni alloué par train, ni placé sur la pile d'un worker natif. S'y ajoutent
les buffers persistants d'inventaire : deux tableaux de 4 096 lignes de
56 octets et un tableau de 8 192 IDs, soit 524 288 octets hors métadonnées.
Ces tailles ne représentent pas la mémoire totale du SDK ou du jeu.

Le parcours possédé du pool n'effectue qu'une passe. Il n'exclut ni les trains
vanilla, ni les attachés, ni les entrées inactives. Les relations validées
`firstChild`/`nextSibling` et la recherche de racine par ordinal évitent un
nouveau scan de tous les slots pour chaque rame. Un inventaire partiel reste
indisponible ; dépasser le nombre de lignes, les reads ou le temps n'est jamais
présenté comme l'absence des trains omis.

Le callback de diagnostic actuel copie cet inventaire, puis **un seul corps de
racine en rotation**. Il ne reconstruit pas les corps de tous les trains à chaque
capture, et ne publie pas un atlas global d'occupation. Le scratch est réutilisé
et le visiteur traite seulement des données POD possédées. Aucun callback de
mod, attente ou formatage de journal n'a sa place sous le verrou exclusif.
Une sortie antérieure ou partielle ne peut pas devenir une nouvelle preuve.

La racine inactive non attachée dispose d'une voie de copie courte : après
validation de l'inventaire, du scope et de l'inactivité de tous ses enfants
attachés, elle publie une absence physique **courante** explicite sans lire
les descripteurs d'historique potentiellement
détruits. La fixture avec roster fourni utilise 12 lectures et 268 octets pour
ce chemin ; le contrôle du catalogue et le parcours du roster sont des coûts
supplémentaires de l'adaptateur complet. Une rame active arrêtée demeure
occupante, et les enfants inactifs attachés restent compris dans sa composition.
Une racine inactive avec un enfant attaché actif reste inconnue ; le diagnostic
identifie cet enfant au lieu d'omettre son corps. Le parcours de cette chaîne
complète utilise le même budget, sans allocation ni lecture d'ancien historique.
Une absence actuelle ne certifie pas une progression historique ou la
libération d'un bail.

La première fixture privée stricte du 11 octobre couvre 25 054 vérifications
après cet ajout : pas d'allocation pendant la capture, quotas exacts et trop courts,
refus/exception à chaque lecture, perte de scope, IDs recyclés, historiques
détruits, racines arrêtées et attelages. Il s'agit de mesures et vérifications
sur mémoire synthétique, sans jeu lancé. Elles ne mesurent pas le coût de
`ReadProcessMemory` sur la carte réelle, la contention native ou le temps
effectivement tenu sous verrou.

La régression ultérieure du parent inactif avec enfant actif porte le test du
fournisseur à 112 143 contrôles PASS, avec un témoin erroné avant correction.
Ces nombres désignent des fixtures distinctes, pas des tests CTest ni des
essais en partie. L'atlas interne est testé séparément sur mémoire synthétique :
il n'est raccordé ni au bridge ni à l'admission et ne certifie aucune progression
historique ou libération arrière. Son stockage de 3 084 400 octets est prévu au
bootstrap, hors pile des workers ; aucune instance n'est ajoutée au bridge actuel.

Le prototype `NRF_DRIVING_TRANSACTIONAL_COMMIT=1`, également désactivé par
défaut, peut consommer le budget natif original du pas entier pour un train dont
la qualification de commit est perdue. La fixture du vrai wrapper vérifie ce
chemin, y compris sans appel ultérieur à l'intégrateur ou à un signal, et
l'appel original unique. Il ne suffit pas de passer une copie locale du budget
à zéro. Ce mécanisme expérimental et ses compteurs ne constituent ni un
remplacement des réservations natives ni une performance validée en partie.

Les hooks d'admission finale `0x448210`/`0x447310` conservent le veto natif et
celui des scripts. Ils ne calculent pas un atlas physique ou un graphe complet
pendant le passage, ne font aucun RPC de mod et ne rejouent pas les callbacks
pour obtenir une nouvelle permission. Les tests du bridge couvrent leurs
requalifications locales ; la couverture et le coût dans une vraie partie
restent à mesurer. Aucun chiffre de latence en jeu n'est revendiqué pour
cette nouvelle étape.

## Capture de signalisation vide — optimisation locale du 11 octobre 2026

L'adaptateur Kotlin des mods de signalisation utilise désormais un transport
interne distinct, `NimbyInternal_CaptureRuntimeSignallingScope`. Les captures
publiques complète, signalisation, signalisation filtrée, données de trains et
topologie conservent leur comportement. Aucun nouveau réglage de mod ou délai
de polling n'est ajouté : la cadence demandée reste de 20 ms.

Après lecture fraîche des catalogues demandés et découverte de tous les signaux
natifs, si le réseau sélectionné est vide, ce transport omet les occupations,
leur réconciliation avec la présence et la table globale des états de texture.
Il conserve les identités et observations de présence des trains pour le service
de contrôle. Les occupations omises retournent **indisponible**, jamais une
collection disponible vide ni une preuve de voie libre. Dès qu'un modèle demandé
est découvert, le chemin ordinaire lit à nouveau les occupations et textures.

Ce chemin est limité au profil Windows 1.19 qualifié. Les racines, l'horloge,
l'identité du monde et les descripteurs du pool de signaux et du catalogue sont
revalidés avant publication. Ces readbacks détectent des remplacements ; ils
ne créent pas un tick atomique et ne prouvent pas l'absence d'une mutation en
place. Une modification en place après découverte peut apparaître seulement
au poll suivant ; aucune absence n'est mise en cache et la sortie courante
reste indisponible pour les occupations.

La fixture exécute le vrai moteur de lecture et la vraie ABI de capture ; seule
la connexion mémoire du processus est synthétique. Le compteur du Reader
mesure toutes les tentatives de lecture et leurs octets demandés. Les deux
captures utilisent exactement la même scène : 1 000 signaux natifs étrangers,
des pools de trains en blocs de 512 slots, une capacité d'occupation de seize
slots par train et une table de rendu de 65 535 slots. Ces capacités servent
à mesurer le travail sur les entrées inutilisées ; elles ne représentent pas
une mesure de la carte du joueur.

| Trains | Capture publique : lectures / octets | Runtime vide : lectures / octets |
| --- | ---: | ---: |
| 50 | 246 / 3 587 022 | 200 / 1 294 448 |
| 500 | 696 / 4 879 422 | 650 / 2 010 848 |
| 5 000 | 5 214 / 26 872 254 | 5 168 / 18 243 680 |

La capture runtime vide n'effectue aucune lecture des racines d'occupation ou
de rendu. La présence globale demeure le principal travail résiduel : avec
5 000 trains, les copies des pools DBTrain et Motion et la seconde lecture
individuelle des Motion représentent déjà 18 036 160 octets. Cette étape ne
supprime pas ces lectures et ne promet pas une latence nulle. Lorsqu'un modèle
est présent, les données restent celles du chemin ordinaire ; les deux reads
supplémentaires de descripteurs demandent 80 octets au total.

Les contrats couvrent les sorties publiques préservées, les identités/services
identiques, l'arrivée puis la suppression d'un modèle, les occupations toujours
indisponibles quand omises, les catalogues corrompus, la perte d'horloge ou de
monde et les remplacements après découverte. Un test ajoute aussi un modèle en
place pendant la capture et vérifie sa découverte au poll suivant. Le reçu et
les mesures de temps hors jeu se trouvent dans
`.validation/native-runtime-20261011-optimized/empty-signalling-private/` à la
racine du workspace. Ils ne mesurent ni `ReadProcessMemory` réel, ni les limites
CPU des workers, ni la latence d'une partie. La recette en jeu avec des signaux
des modèles actuels reste à effectuer après reconstruction du kit.

### Préfixe de présence des trains

Hors regroupement décrit ci-dessous, la seconde lecture individuelle d’un Motion vivant utilise un préfixe
de `0x4B1` octets pour le mode interne `presenceOnly`, au lieu de `0x638`.
Le dernier octet consommé pour ce mode est `0x4B0`. Les captures de matériel,
service et données complètes conservent `0x638`, comme les Motion retirés.
Le tampon et les allocations restent inchangés ; les contrôles d’identité
complète, de tête et des trois indicateurs de présence restent obligatoires.
Une lecture partielle ou incohérente ne devient pas une présence valide.

La même fixture à 5 000 trains passe de **18 243 680 à 16 288 680 octets**
demandés par capture runtime vide, avec **5 168 lectures** dans les deux cas.
L’économie est de 391 octets par Motion vivant, soit 1 955 000 octets pour ce
scénario. Cinq exécutions appariées de chaque variante, construites avec les
mêmes sources sauf ce changement, passent les contrats de capture. La médiane
synthétique passe de 6 931,5 à 6 783,7 microsecondes ; les petits scénarios ne
montrent pas de gain temporel stable. Ces mesures ne prédisent pas les FPS ni
le coût des lectures d’un processus réel. Le reçu, les sorties et les commandes
sont conservés dans
`.validation/native-runtime-20261011-optimized/train-presence-prefix/`.

### Transactions d'inscription et démarrage partiel sous manque de mémoire

Les options préparent désormais le profil et leur catalogue immuable avant de
publier le jeton d'inscription. Après publication, l'insertion utilise des
nœuds de `std::map` déjà alloués. Un échec d'allocation ne nécessite plus une
désinscription qui pourrait, elle aussi, manquer de mémoire et laisser un
identifiant inaccessible. Le remplacement d'un profil en attente conserve ses
dernières valeurs acceptées ; les autres propriétaires restent inscrits.

Si le démarrage d'un lot de processus échoue, tous les processus déjà démarrés
sont arrêtés et leurs canaux sont retirés avant la construction du diagnostic.
L'échec de la mise en forme du message ne conserve donc plus un lot partiel
qui empêcherait la prochaine tentative de démarrage.

Les deux régressions utilisent de vrais échecs consécutifs de `operator new`,
limités au thread du test. Chaque ancienne implémentation échoue au témoin,
chaque correction permet une nouvelle tentative, et le test du lot lance de
vrais processus enfants jetables. Le reçu est conservé dans
`.validation/oom-host-options-20261011/receipt-two-fixes.json`. Ces essais
ne déclenchent aucune panne dans le jeu ou dans les processus du joueur.

L'export `NRFMod_Start` publie aussi ses callbacks par un transfert garanti
sans exception après le démarrage des services. L'ancienne copie pouvait
encore allouer à cet endroit, hors de la transaction de rollback, et laisser
des services actifs avec un état non initialisé. La régression du véritable
export prépare un catalogue non vide, démarre réellement le callback puis
arme un manque de mémoire simple ou persistant. L'ancien code échoue ; le
transfert final des callbacks n'alloue plus et le test conserve le
dispatch, le refus d'un double démarrage, l'arrêt équilibré et le redémarrage.
Le reçu est conservé dans
`.validation/mod-entry-allocation-20261011/receipt.json`.
Les étapes précédentes de connexion des services ou de démarrage des
observations peuvent encore allouer : elles restent à l'intérieur du rollback.
La garantie sans exception porte sur la publication finale des callbacks,
pas sur un démarrage complet sans allocation.

### Répétition du cycle du vrai mod AB

Cinq processus hors jeu chargent le SDK, l'adapter et le core Kotlin AB du
paquet local `alpha4-coupled-final`. Chacun conserve les contrôles du loader
strict puis exécute 200 cycles démarrage/arrêt, 256 signaux groupés et 256
callbacks par cycle, ainsi qu'un callback invalide rejeté. Les cinq essais
passent. Sur les échantillons après échauffement, les threads restent à six
et les handles à 117 ou 118 selon le processus. La mémoire privée ne présente
pas de croissance progressive dans ces essais ; la variation de l'allocateur
et du GC n'est pas un indicateur de fuite à elle seule.

Le code Kotlin/Native reste volontairement chargé jusqu'à la fin du processus,
car ses threads de GC peuvent encore y exécuter du code. Les cycles arrêtent
les services logiques du mod ; ils ne certifient pas un déchargement physique
de ce runtime. Ce test fini ne prouve ni l'absence de toute fuite, ni la mémoire
du jeu, ni l'isolation par processus du mod host. Il précède le correctif de
publication finale décrit ci-dessus et ne suffit pas à le valider. Les hashes,
les 110 échantillons et les résultats bruts sont conservés dans
`.validation/packaged-mod-lifecycle-20261011/result.json`.

### Destruction des documents JSON sous manque de mémoire

La bibliothèque JSON générale utilise une pile allouée pour détruire les
conteneurs imbriqués. Un second échec d'allocation pendant le déroulement
d'une exception peut donc appeler `terminate` avant le retour de la frontière
SDK. Ce comportement a été reproduit dans le véritable export d'inscription
des options ; le seul `catch` extérieur ne protège pas cette destruction.

Les parsers contrôlés des options, traductions et messages de l'éditeur utilisent
désormais un profil interne dont la destruction ne fait aucune allocation.
Leurs callbacks refusent une profondeur supérieure à huit avant la création
du conteneur suivant ; une entrée incomplète peut donc contenir au maximum
neuf niveaux de conteneurs. Leur destruction récursive reste bornée par ce
contrôle. Les documents construits par l'hôte sont également de profondeur
bornée. La bibliothèque générale conserve son algorithme itératif pour les
documents de profondeur arbitraire. Ce profil ne constitue pas un parseur
général et ne doit jamais servir à une entrée sans contrôle de profondeur.

Deux cibles dédiées vérifient le code de production :
`bounded_json_allocation_failure` couvre 200 frontières d'allocation, les
documents partiels ou trop profonds, la destruction sans allocation et un
document général de 50 000 niveaux ; `mod_options_export_allocation_failure`
couvre 274 pannes doubles ou persistantes d'inscription et 18 de lecture.
Les sorties d'échec restent vides, les autres propriétaires restent inscrits
et la tentative suivante réussit. L'hôte du test possède un répertoire
temporaire neuf et rejoint son worker avant de le supprimer ; le singleton
de production et les préférences du joueur ne sont pas utilisés.

Le client IPC des options et les deux builders de déclarations de l'adapter
Kotlin utilisent aussi ce profil. Le client refuse la profondeur excessive
avant la création du prochain conteneur et conserve la révision non acquittée
après un échec ; une nouvelle lecture du même propriétaire reste possible.
Les builders produisent des arbres de structure bornée (messages ou champs
et choix), sans parser une arborescence arbitraire provenant du mod.
Le contrat existant du client inclut 50 essais doubles ou persistants sous
manque de mémoire. Les méthodes réelles des builders, extraites dans une
fixture privée avec leurs exports simulés, passent 408 essais ; leurs témoins
originaux terminaient pendant la destruction JSON. Ces chemins vivent
normalement dans le processus isolé du mod, pas dans un nouveau parser du
parent jeu. La fixture extraite ne remplace pas le test du véritable adapter
reconstruit et de son core Kotlin. L'inventaire et les traces sont conservés
dans `.validation/oom-host-options-20261011/remaining-json/receipt.json`.

### Regroupement limité des secondes lectures de présence

La capture interne des mods de signalisation peut regrouper ces préfixes sur
le profil Windows 1.19. Le lecteur générique conserve `allowPresenceBatch=false`.
Le regroupement exige au moins 4 096 trains dans le roster DBTrain, puis au
moins 128 Motion vivants connus dans chaque bloc, au maximum 512 slots par
bloc et une densité d'au moins 50 % entre les premières et dernières lignes
connues. Les captures publiques, données complètes, profils Linux, petits
rosters et blocs trop clairsemés conservent leurs lectures et allocations.

Le scratch local contient au maximum 814 713 octets, n'est pas initialisé
inutilement et est réutilisé entre les blocs d'une invocation de `read_trains`.
Une seule tentative d'allocation est autorisée par invocation ; un échec laisse
le chemin individuel disponible. La capture peut appeler de nouveau
`read_trains` pendant sa réconciliation : il ne s'agit pas d'une allocation
unique garantie pour toute la capture. Le scratch est libéré entre invocations.

Un refus de lecture groupée autorise les lectures individuelles, même lorsque
le lecteur a écrit partiellement le tampon. Un succès dont les données révèlent
une identité, un indicateur de présence ou une position incohérente n'est
jamais relu pour masquer ce désaccord. Les copies initiales, tombstones,
contrôles des descripteurs et données de service restent inchangés. Le
regroupement ne crée aucune publication atomique ni autorité de circulation.

Cinq paires alternées, avec douze échantillons chronométrés chacune, utilisent
`ReadProcessMemory(GetCurrentProcess())` sur des régions synthétiques détenues.
L'échauffement précède les mesures ; les injections de panne les suivent.

| Roster dense | Ancien p50 / p95 (µs) | Regroupé p50 / p95 (µs) | Lectures |
| --- | ---: | ---: | ---: |
| 4 096 | 6 696 / 7 292 | 5 086 / 5 665 | 4 136 → 48 |
| 5 000 | 9 102 / 9 985 | 6 901 / 7 506 | 5 044 → 54 |

Ce gain nécessite davantage d'octets copiés : à 5 000 lignes, les lectures
directes passent de 16 081 800 à 18 032 890 octets. La variante `memcpy` seule
est plus lente, de 5 989 / 6 368 à 7 098 / 7 998 µs ; la vraie ABI avec une
connexion synthétique fondée sur la copie peut également être plus lente.
Ces résultats justifient le chemin limité Windows/RPM, pas un gain général de
FPS. La mesure dans le processus du jeu et sur la carte du joueur reste à faire.

La vraie ABI vérifie également les scopes vides et sélectionnés, les services,
les positions valides quand le réseau les expose, les changements pendant la
lecture et le maintien d'une information indisponible. À 5 000 trains sans
modèle sélectionné, elle demande 178 lectures et 18 239 770 octets, contre
5 168 et 16 288 680 avec le préfixe seul. Les sorties publiques restent
inchangées. Les sources, 30 exécutions, résultats bruts et limites sont conservés
dans `.validation/native-runtime-20261011-alpha4-candidate/presence-batch-promotion/promotion-receipt.json`.

### Autorité de réservation interne : lectures locales et index compact

Le contrat de réservation en deux phases utilise désormais un index borné des
propriétaires et des chaînes de portions par bail. Un renouvellement ou un
instantané d'un propriétaire connu ne parcourt plus systématiquement tous les baux et toutes les
lignes. Les clés conservent leurs 64 bits ; les indices internes sont comprimés
dans les deux buffers de lignes fixes. Les acquisitions nouvelles et suppressions
préparent toujours l'index global avant publication. Aucun thread n'attend la gate :
une contention ferme la requête et conserve les protections déjà publiées.

Une première comparaison de cinq paires de processus oppose le contrat final sans cache (`68AB6860…`) et
le même contrat avec l'index durci (`B01FE58E…`). Le scénario synthétique contient
10, 500 puis 5 000 trains, une ressource et une portion physique par train, sur
des voies distinctes. Il vérifie les permissions et identités pendant les mesures,
puis une libération avec un historique arrière complet sur une voie disjointe.
Les phases stables comprennent huit passages mélangés avec une graine fixe.
Chaque variante produit 104 690 échantillons par exécution. L'allocation initiale,
la préparation des données, les instantanés préparatoires à la libération et
l'écriture des CSV ne sont pas chronométrés. Un scénario de 64 trains précède
les mesures ; il ne représente pas un échauffement de tous les volumes.

Les statistiques ci-dessous sont les médianes des cinq statistiques par
exécution, en microsecondes, pour 5 000 trains. Les temps les plus courts sont
proches de la résolution du chronomètre ; les totaux des passages complètent
ces percentiles.

| Opération | Sans cache p50 / p95 / p99 | Index compact p50 / p95 / p99 |
| --- | ---: | ---: |
| Réservation provisoire | 62,7 / 114,3 / 136,0 | 49,1 / 88,7 / 116,7 |
| Renouvellement | 8,0 / 11,9 / 18,1 | 0,1 / 0,2 / 0,3 |
| Instantané | 6,6 / 10,6 / 15,9 | 0,4 / 0,7 / 0,8 |
| Libération arrière | 18,9 / 34,0 / 46,7 | 14,0 / 23,9 / 40,3 |

Les huit passages de renouvellement passent de 340,83 à 5,00 ms ; ceux des
instantanés de 283,86 à 19,03 ms. Les budgets restent identiques : 65 536 unités
par opération. Les maxima mesurés du nouveau contrat sont de 30 018 unités pour
une acquisition et 15 009 pour une libération dans ce scénario. Ces unités ne
sont pas une borne de durée en microsecondes.

Le stockage des buffers pour 5 000 baux et 10 000 lignes passe de 1 640 000 à
2 335 360 octets (+695 360). Ce décompte couvre les structures fixes de l'autorité,
pas la mémoire du processus ni les temporaires appelants. La variation dépend
du rapport entre nombre de baux et nombre de lignes ; la compression des lignes
ne suffit pas à compenser le cache pour ce scénario. Six allocations sont faites
au constructeur, puis aucune dans les opérations vérifiées.

Une entrée absente du cache déclenche aussi un contrôle de tous les baux pour
ne pas fabriquer un doublon. Les collisions et tombstones restent bornés par
le budget, avec un coût qui peut atteindre celui d'un parcours complet. Les
tests injectent des chaînes tronquées ou cycliques, des propriétaires manquants,
des indices périmés et des refus de budget pendant la préparation et la
suppression. Ils vérifient le refus de permission, la conservation des autres
baux et l'absence d'instantané partiel. L'épuisement de l'époque du scratch ferme
les mutations ; une erreur de préparation ne remet jamais son compteur en arrière.

Les sources gelées, cinq paires, échantillons bruts et résultats sont conservés
dans `.validation/admission-performance-20261011/`. Les premiers prototypes et
leurs mesures y restent distincts de cette comparaison finale. Ce banc ne mesure
ni conflits d'itinéraires réels, ni lectures mémoire du jeu, ni callbacks de mods,
ni performances en partie. L'autorité n'est pas raccordée en production :
`nativeReplacementConnected` reste faux. Ces gains ne constituent donc pas une
mesure de FPS ni un remplacement terminé des réservations natives.

### Suppressions répétées : éviter les scans redondants du hash

Un second scénario a révélé une régression de la première variante compacte
quand toute la table contenait des tombstones. Il configure 5 000 baux et
16 384 cellules de hash, remplit ces cellules par de vraies réservations et
annulations successives, puis mesure 20 000 cycles avec au maximum un bail
vivant simultanément. Ce volume configuré ne représente donc pas 5 000 trains
en circulation. La comparaison initiale avec le contrat sans cache donnait
3,1 contre 26,1 µs au p50 pour une réservation.

Le correctif (`88BA1D24…`) limite la première recherche à 64 sondes. Une
recherche infructueuse vérifie toujours tous les baux avant de conclure à
l'absence. Si un bail existe, elle reprend les sondes restantes et exige
le nœud exact : elle ne répare aucun propriétaire manquant. Pour un propriétaire
réellement absent, la vacance déjà qualifiée reste locale à la même gate et
évite un second scan des tombstones avant staging. Les gardes de publication,
permis et contrôles numériques restent identiques.

Cinq nouvelles paires comparent directement les deux variantes compactes,
sur le même scénario de suppressions :

| Réservation après suppressions | Première variante | Correctif |
| --- | ---: | ---: |
| p50 / p95 / p99 (µs) | 24,5 / 35,3 / 59,3 | 3,5 / 4,9 / 8,6 |
| Total des 20 000 appels (ms) | 534,54 | 77,44 |
| Maximum d'unités de travail | 37 776 | 5 073 |

Les valeurs résument les médianes des cinq exécutions. Le gain sur le total
est d'environ 6,9 fois ; les unités restent soumises au même plafond de
65 536 et le stockage fixe ne change pas. Dans les cinq paires avec 5 000 baux
simultanément vivants, les huit passages de renouvellement restent proches
de 5,18 ms et ceux d'instantanés de 18,8 ms. Ces faibles écarts ne démontrent
pas un gain supplémentaire sur les lectures stables. Les acquisitions restent
proportionnelles aux baux et aux lignes, et les collisions longues peuvent
encore imposer une reprise complète du hash.

La fixture indépendante vérifie les propriétaires aux sondes 63, 64, 65 et 80,
les tombstones intermédiaires, les corruptions de propriétaire, les quotas
aux frontières de reprise, le staging refusé, une table sans vacance et le
recyclage du monde. Une création peut ne pas lire une cellule étrangère
corrompue au-delà du préfixe si une vacance est déjà qualifiée et l'absence
prouvée : le contrat ne prétend aucune vérification globale du cache. Les
protections et conflits restent fondés sur les baux et leurs portions.

La comparaison précédente demeure intacte. Les nouvelles sources gelées,
vingt exécutions, résultats bruts et limites sont conservés séparément dans
`.validation/admission-performance-20261011/probe-cap/`. Ces deux scénarios
restent hors jeu et ne raccordent aucune réservation native.

## Reprise avec les réservations du jeu — 16 octobre 2026

Le remplacement global des réservations est abandonné. Le suivi de conduite
par passage et dégagement arrière reste inchangé. Le nouveau transport Kotlin
des occupants réutilise `LiveSignalState.trains` et `trainListComplete` ; il
n'ajoute ni capture, ni interrogation native, ni appel Kotlin par train. Les
exports enrichis remplacent les appels existants. La préparation du réseau
ne transporte cette liste que pour les modèles déclarant `observeBlockTrains()`.
Sans demande dans le mod, les anciens appels sont utilisés et aucune liste
d'occupants n'est copiée vers Kotlin. Lorsque cette observation est demandée, elle
regroupe les identifiants dans un lot borné à 131 072 entrées, avec au plus
4 096 par signal. Une limite dépassée est refusée, jamais tronquée en une liste
prétendument complète. Cette extension garde la structure triviale des commandes
`v1` du build courant ; les données possédées supplémentaires restent dans le
type live distinct. Ce constat ne garantit pas l'ABI brute entre des headers SDK
de versions différentes.

`BlockTrains` conserve une copie triée sans doublons. Une liste déjà ordonnée
n'est pas retriée ; les recherches répétées de présence sont binaires et sans
allocation. L'observation Kotlin masque les présences périmées et abaisse la
complétude lorsque la frontière du canton est inconnue.

Le suivi diagnostic des conflits remplace le résumé par hash. Il associe les
faits à la session, au signal, au train et à la révision du parcours. Les pertes
et remplacements s'accumulent avant émission, avec huit échantillons au plus et
une émission par seconde au maximum. Le cycle de vie publie des invalidations
atomiques ; le worker consomme ces invalidations avant sa prochaine observation.
Un arrêt définitif du processus peut donc précéder l'émission du dernier lot.
La remise au journal du SDK ne constitue pas un accusé de persistance disque.
L'historique ne rejoint jamais les règles ni une autorisation de mouvement.

Un microbenchmark local de `SignalRouteDiagnostic::observe`, compilé en `-O2`,
utilise 64 observations de préchauffage et trois répétitions par cas. Il couvre
50, 500 et 4 096 sources avec une ou seize lignes par source, des états stables
et des remplacements. Sur cette machine, le cas stable à 500 sources et 500
lignes mesure environ 16,4 µs de médiane et 25,5 µs au p95 ; à 4 096 sources et
65 536 lignes, environ 0,85 ms et 1,56 ms. Les changements simultanés de toutes
les révisions à cette dernière taille atteignent environ 2,46 ms et 3,75 ms.

Le stockage initial des deux tableaux du helper coûte 1 Mio. Après préchauffage,
aucun `new` ou `new[]` n'est mesuré dans ses observations. Ces chiffres incluent
le bruit d'une machine Windows non exclusive ; ils excluent capture, transport
Kotlin, formatage des journaux, disque, rendu et circulation réelle. Ils ne
mesurent ni un gain de FPS ni la latence du jeu. Les données brutes et les
conditions sont conservées dans
`sdk/.validation/native-compatible-sdk-20261016/diagnostic-performance/`
depuis la racine du workspace.
