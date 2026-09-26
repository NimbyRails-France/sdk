# Mémoire, ressources et performances

## Propriété et durée de vie

| Ressource | Propriétaire | Libération / validité |
| --- | --- | --- |
| Connexion processus | Session du registre d'observation | Fermeture de session, même après un échec d'ouverture |
| Handle Windows du processus / snapshot de modules | `windows::UniqueHandle` | Destruction ou `reset`, y compris sorties anticipées |
| Données d'une capture | Capture du registre | `NimbyInternal_ReleaseSnapshot` |
| Copie C++/JVM d'une observation | Objet consommateur | Destruction de l'objet / collecte JVM |
| Buffers temporaires de lecture | Parcours/capture courant | Fin de l'opération |
| Table de commandes de textures | Runtime du pont | Durée de vie du pont ; accès protégé |
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

Le mutex de registre protège les sessions et captures. Le runtime de diagnostic
possède son propre verrou de cycle de vie. Le backend d'observation n'ajoute pas
de thread ni de verrou supplémentaire ; ses appels sont sérialisés par le runtime.
La durée du verrou de registre inclut encore des lectures distantes. Une évolution
vers des verrous par session exige des tests de fermeture concurrente et une
mesure de contention ; elle ne doit pas être introduite par simple intuition.

Les callbacks du jeu doivent rester courts et ne doivent pas laisser remonter
d'exception. Pour les textures, les modifications de table restent côté worker ;
le callback de rendu copie une entrée sous verrou et libère rapidement ce verrou.

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
