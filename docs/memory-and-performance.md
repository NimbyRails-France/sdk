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
Sur la compilation Windows x64 validée, la structure de monde occupe 14 417 920
octets (13,75 Mio), dont 8 Mio pour l'index d'intérêt ; s'y ajoutent les vecteurs
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

## Ressources des processus de mods Windows

Le chargeur calcule les limites avant de démarrer le lot de mods. Avec `N` mods,
chaque processus reçoit au plus un processeur logique et `25 % / N` du CPU de la
machine. Sa priorité reste inférieure à celle du jeu. Ces allocations sont fixes :
le nombre de mods activés influe sur les limites au démarrage, mais un mod en
boucle ne prélève pas de jetons de requêtes ou de CPU dans le quota d'un voisin.

L'admission réserve au plus le quart de la RAM physique totale et la moitié de
la mémoire encore disponible (physique et engagement). Elle compte le canal de
communication, deux copies de message et la limite de mémoire privée de chaque
processus. La limite privée va de 192 Mio à 1 Gio selon la place restante. Le
minimum sert seulement à l'admission : chaque mod reçoit toute sa part égale du
budget restant. Ainsi, quatre mods avec 2 305 Mio disponibles reçoivent environ
240 Mio chacun, sans élargir l'enveloppe globale. Si le
lot ne tient pas, ou dépasse 32 mods, aucun processus du lot n'est lancé et le
chargeur signale `ResourceLimit` dans son journal. Fermer d'autres applications
ou activer moins de mods permet une nouvelle tentative. Cette admission ne
réserve pas physiquement la RAM et ne plafonne pas toutes les allocations du jeu.
Le diagnostic distingue le refus du calcul de budget d'un échec de mesure
Windows ; il conserve la mémoire physique et l'engagement disponibles, le
budget calculé, le minimum nécessaire et la limite privée attribuée.
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
