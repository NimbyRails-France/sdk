# Construction et annulation des signaux — Windows

État au 27 septembre 2026 : **recherche statique et observation dans le jeu sous
Windows, aucun pont de pose SDK activé**. Trois poses manuelles et une annulation
ont été observées dans une nouvelle partie en pause. La file de l'éditeur et
l'exécution sur le thread de simulation sont distinctes. Cela ne valide pas
encore une API de pose ni une transaction de plusieurs signaux.

## Provenance et reproduction

Binaire 1.19.10.5bfaea3, SHA-256 :
`fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae`.
Analyse Ghidra 12.1.3 sur le VPS, depuis une copie privée du projet analysé,
avec `-readOnly -noanalysis`. Cette première passe n'a pas attaché le jeu.
Le projet Ghidra d'origine n'a pas été modifié.

Les exports sont dans [reports/construction](reports/construction/).
Ils comprennent le pseudo-C, les instructions et les références des fonctions.
Le script [InspectConstruction.java](../../tools/InspectConstruction.java)
refuse un autre hash et limite à 64 fonctions chaque export. Il accepte :

```text
-postScript InspectConstruction.java /chemin/rapports \
  vtable:cmd::tn::CreateSignal vtable:cmd::tn::Undo \
  1407bc7e0 1407832e0 140339140 140786320 1407afe40 140762a70
```

`data:VA` suit les références, `symbols:fragment` liste les symboles et
`offset:hex:débutVA:finVA` liste les instructions candidates sans les décompiler.
Les recherches d'offsets excluent RSP/RBP : c'est une heuristique, jamais une
preuve de complétude. `invocations.txt` conserve les sélecteurs des exécutions
effectuées avec la dernière version du script.

Toutes les valeurs du tableau suivant sont des **RVA**. Les noms de fichiers
utilisent les VA Ghidra, avec la base préférée `0x140000000`. Une adresse en
processus dépend de l'ASLR. Les signatures décompilées restent des hypothèses,
pas des déclarations de fonctions à recopier dans le runtime.

## Chaîne de création retrouvée

| RVA | Rôle observé | Preuve |
| --- | --- | --- |
| `0x2fda10` | Fabrique enregistrée pour `tn::CreateSignal` | Appelle le constructeur et transfère la propriété du pointeur |
| `0x31bea0` | Allocation et initialisation de la commande | Allocation de `0xe8` octets, vtable et données initialisées |
| `0x7bc7e0` | Création et ajout à la file de l'éditeur | Transfert de propriété dans le vecteur de commandes |
| `0x7832e0` | Action de l'outil de pose | Enfile, prépare un signal temporaire puis déplace ses données dans la commande |
| `0x2fda60` | Exécution, slot `+8` de la vtable | Résout la voie, alloue le signal, copie et actualise le réseau |
| `0x339140` | Copie profonde d'un signal | Copies des chaînes et collections, pas seulement des champs scalaires |
| `0x3a81f0` | Normalisation du signal créé | Consultation des catalogues et reconstruction de données liées au type |

La vtable CreateSignal est à `0xa6dde8`. Ses autres slots identifiés sont le
destructeur `0x321e90`, la sérialisation `0x2fd8e0`, la désérialisation
`0x2fd970`, le tag `0x2fda00` et la catégorie `0x2f3830` (valeur 3).
Le nom de bibliothèque attribué au destructeur par Ghidra est trompeur ; les
références de vtable et les instructions sont les preuves utilisées ici.

L'objet contient un en-tête de `0x20` octets puis un signal de `0xc8` octets.
Dans cette commande, la position commence à `+0x60` : voie, fraction à `+0x68`,
direction à `+0x70` (un octet signé), orientation de la voie à `+0x71`.
L'orientation doit venir de la voie de destination (`Track+0x2c`), comme pour
l'aperçu natif décrit dans `include/platform/windows/signal_preview.h`.
Écrire un `int32_t` à `+0x70` écrase cette orientation et les deux octets suivants :
le pont utilise désormais la même écriture de position que l'aperçu.
Ces données contiennent aussi des allocations appartenant
au jeu. **Copier les octets du modèle avec `memcpy` serait incorrect** : les
chaînes et collections deviendraient partagées avec des durées de vie erronées.
Le futur adaptateur doit respecter construction, copie et destruction natives.

Dans le contexte reçu par `0x7bc7e0`, le sous-objet à `+0x228` fournit deux
valeurs recopiées dans l'en-tête, puis le vecteur de pointeurs de commandes à
`+0x238/+0x240/+0x248`. Une valeur issue du contexte `+0x250`, indexée par
catégorie, est incrémentée et recopiée en `commande+0x18`. Son rôle exact dans
la corrélation, le réseau ou l'ordonnancement n'est pas établi : ne pas la
traiter comme un identifiant de transaction SDK.

L'exécuteur utilise la base référencée par `contexte+0x428`, alloue un nouvel
enregistrement dans le pool des signaux, conserve son nouvel ID, copie les
données du modèle et restaure cet ID. Il ajoute le signal à la voie, appelle
les actualisations natives et remplit un résultat. Une voie absente laisse
l'ID local à zéro, qui passe encore dans la construction du résultat.
**Le retour d'un pointeur de résultat ne prouve donc pas une pose réussie.**

## Annulation et retour à l'éditeur

La fabrique Undo est `0x2f3a40`, sa vtable `0xa6c998` et son exécuteur
`0x2f3ab0`. La commande fait `0x620` octets : en-tête puis delta de `0x600`.
L'exécuteur transmet le delta à `0x3ad950`, qui traite plusieurs catégories
d'objets, valide et remappe leurs identités avant application. Ce n'est pas une
fonction sans argument qui annule automatiquement la dernière action.

Le traitement clavier dans `0x786320` montre le chemin réel de Ctrl+Z :

1. Vérifier que l'historique de l'éditeur n'est pas vide (`+0xd80`).
2. Enfiler une commande Undo via `0x7b57c0`.
3. Copier les huit groupes du dernier delta (`éditeur+0xd78`) vers la commande.
4. Retirer cette entrée de l'historique via `0x7c16d0`.

Dans `0x7afe40`, l'éditeur parcourt les résultats reçus, filtre un identifiant
par rapport au contexte et retient ceux du type reconnu par `0x7bba50`
(`résultat+0x1f0 == 1`). Les données retournées servent notamment à actualiser
la sélection des signaux. Selon des indicateurs du résultat, le delta à
`résultat+0x1f8` est copié via `0x3547f0` et ajouté à l'historique par
`0x762a70`, ou l'historique est vidé. Le sens complet de ces indicateurs reste
à qualifier. La fonction d'ajout conserve au plus 100 entrées et applique aussi
un budget mémoire au-delà de 10 entrées.

Ces observations établissent un aller-retour commande/résultat/historique.
Elles ne démontrent pas que plusieurs CreateSignal enfilés ensemble produisent
une seule entrée d'annulation, ni qu'une série est atomique en cas de refus.

## Conditions restant à établir avant le pont SDK

- Identifier la durée de vie du contexte d'édition et le transfert complet de sa
  file au simulateur ; les threads et la consommation sont maintenant observés
  dans une partie en pause, mais pas lors d'un changement de partie.
- Établir la corrélation d'une opération SDK avec ses résultats, sans absorber
  les commandes ou sélections manuelles de l'utilisateur.
- Vérifier permissions, modes de construction, orientation et reproduction des
  paramètres d'un signal du jeu puis d'un signal SFR.
- Déterminer le mécanisme natif de regroupement et le comportement d'un refus
  au milieu de la série ; aucun succès partiel ne doit être masqué.
- Conserver un delta appartenant à cette série et refuser une annulation devenue
  incompatible avec des modifications ultérieures. Ne pas simuler Ctrl+Z.
- Valider la fraîcheur de session, du modèle et de la topologie au moment de
  l'exécution ; l'horodatage de la prévisualisation actuelle n'est pas une révision.

Les mécanismes natifs et adresses resteront dans l'adaptateur Windows du SDK.
L'espacement, l'arrêt aux aiguilles et la décision de proposer une série restent
dans le projet Kotlin `signal-placement`. Aucun offset ne doit y être exposé.

## Observation locale : trois poses, puis annulation

Le 27 septembre, sur le PC Windows, une nouvelle partie vide a servi à construire
une courte voie et à poser trois signaux natifs Path depuis l'éditeur. La
simulation était en pause à 16:01:15. Aucun appel autonome à un exécuteur natif
n'a été utilisé : la sonde accompagne les appels normaux du jeu et transmet les
arguments et les retours sans modification.

Résultats vérifiés par une capture SDK indépendante, en lecture seule :

| Vérification | Observation |
| --- | --- |
| Créations | Trois appels CreateSignal terminés, trois signaux dans la capture |
| Historique | Compteur après chaque pose : 1, 2, 3 |
| Thread d'enfilage et d'historique | 29708 dans ce processus |
| Thread d'exécution CreateSignal et Undo | 35124 dans ce processus |
| Annulation native | Un appel Undo terminé, passage de trois à deux signaux |
| Objets conservés | Identifiants, voie, fraction, direction et type des deux premiers signaux inchangés |
| Voies | Quatre enregistrements, métrique 819,7292 m chacun, inchangés après Undo |
| Perte d'événements de la sonde | Aucune dans cette séquence |

Le raccourci envoyé avec le contrôle gauche n'a produit aucun Undo observable ;
celui avec le contrôle droit l'a produit. Cela décrit l'automatisation de cette
session, pas une règle des raccourcis du jeu. Ne pas implémenter l'annulation du
mod en envoyant une touche.

La preuve synthétique, sans pointeurs de processus ni stockage brut des variantes,
est [live-20260927-summary.json](reports/construction/live-20260927-summary.json).
Les captures complètes de travail restent sous `build/construction-research`
à la racine du workspace ; elles ne sont pas des fichiers de distribution.
Les identifiants de threads ne doivent jamais être codés en dur.

### Répartiteur identifié grâce aux piles réelles

La pile des exécuteurs mène au RVA `0x30e020`, appelé depuis `0x3489d0`
(`SimRunner::run`). Les exports correspondants sont dans le dossier des rapports.
Le répartiteur boucle sur les commandes et crée un enregistrement de retour par
commande. Il y copie l'enveloppe de corrélation : `commande+8` vers `retour+0x7f8`,
`commande+0x10` vers `retour+0x800`, la catégorie vers `retour+0x808` et
`commande+0x18` vers `retour+0x810`. Le sens métier de toutes ces valeurs reste à
qualifier avant d'en faire un contrat SDK.

Il invoque ensuite le slot d'exécution avec un stockage temporaire. **Le retour
de l'exécuteur n'est pas l'enregistrement complet remis à l'éditeur.** Après
conversion du résultat temporaire et mises à jour natives, `0x324750` remplit le
delta à `retour+0x1f8`, puis `0x479530` le traite. Le worker appelle ce répartiteur
sous le verrou exclusif du simulateur. Ni ce verrou ni l'exécuteur ne doivent
être appelés arbitrairement depuis le thread de réception du SDK.

La première sonde lisait à tort `temporaire+0x1f8` comme un delta. Ces octets
n'étaient pas le delta final et ne sont utilisés dans aucune conclusion. La
source a été corrigée : observation du discriminant initialisé seulement,
delta lu dans Undo avant exécution ou lors de l'ajout à l'historique. À l'enfilage,
seul l'en-tête est lu, puisque l'éditeur remplit ensuite le payload du signal.
Cette correction a été recompilée sur le VPS ; la DLL déjà chargée lors de cette
séquence était la première version et reste épinglée jusqu'à la fermeture du jeu.

### Utiliser la sonde de recherche

Les cibles Windows `nimby_construction_probe` et
`nimby_construction_probe_bridge` sont des outils privés, non installés et non
publiés avec le SDK. Le client exige un PID explicite, le chemin du jeu reconnu,
la DLL de sonde et un fichier de sortie ouvrable avant attachement :

```text
nimby_construction_probe.exe GAME.exe NimbyConstructionProbe-v1.dll PID 300 observation.jsonl
```

La DLL vérifie le hash du jeu et les octets d'entrée des quatre fonctions avant
d'installer ses hooks. Le journal ajoute une entête à chaque session ; les
événements comprennent compteur, thread et temps monotone Windows. Un anneau
de 128 événements borne la mémoire ; tout dépassement détecté est journalisé.
La durée autorisée est de 1 à 1800 secondes. À expiration, les hooks ne capturent
plus et transmettent les appels ; le module reste chargé jusqu'à la fermeture
du jeu. Ne pas remplacer une DLL chargée pour prétendre tester une nouvelle
version : redémarrer le jeu pour une nouvelle qualification.

Cette recette valide les poses manuelles et l'annulation native d'une seule
pose. Elle ne valide pas une série automatique, son regroupement en un seul
Undo, la sauvegarde/relecture, une courbe, une aiguille ou un modèle SFR.

## Qualification du pont de série — 27 septembre 2026

Une seconde expérience, distincte de la sonde d’observation ci-dessus, répète
l’exécuteur CreateSignal avant la finalisation de son delta. Trois objets ont
été créés depuis un seul clic. Le pont SDK expérimental a ensuite été testé dans
un processus neuf, en rechargeant la sauvegarde manuelle du réseau minimal.

Le cycle **PREPARE → CREATE(3) → UNDO** a réussi par l’API externe. Les captures
SDK trouvent cinq signaux après la création, puis exactement les deux signaux
initiaux après une seule annulation. Les trois nouveaux objets ont les fractions
0,171 / 0,271 / 0,371 sur la voie demandée et reprennent le type/sens du modèle.
Cette preuve ne dépend pas du raccourci clavier automatique, resté peu fiable.

Les essais supplémentaires refusent un ancien jeton, préservent le nouveau
jeton après ce refus, permettent une nouvelle création/annulation, refusent une
seconde annulation et refusent une voie absente. Le réseau final conserve deux
signaux. Les captures de travail sont `sdk-created.json`, `sdk-undone.json` et
`sdk-guards-final.json` sous `build/construction-research` du workspace.

Le suivi d’historique nécessite aussi le déplacement natif du delta `0x3547f0` :
le pointeur passé à l’insertion `0x762a70` est celui d’un temporaire, différent
du champ delta dans le résultat. Le pont suit ce déplacement avant d’attribuer
le nœud de liste à son opération. Il ne suppose pas que la queue de liste est
forcément son entrée.

La cible `nimby_construction_bridge` reste un artefact de qualification. Elle est
incluse explicitement dans les kits reconstruits localement par le Hub, mais
reste absente des distributions publiques. Le transport depuis un mod utilise
un chargement local, puis la même file que le client externe.
[Contrat, limitations et conditions de livraison](../construction.md).
