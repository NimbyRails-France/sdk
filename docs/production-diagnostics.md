# Diagnostic en production — Windows

Dans le Hub : **Téléchargements → Exporter les logs NRF**. Choisir un nouveau
fichier ZIP, puis transmettre ce fichier avec l'heure du problème, le comportement
attendu et les étapes pour le reproduire. L'export est local : rien n'est envoyé
automatiquement. Les journaux peuvent contenir des chemins personnels et des
identifiants d'objets du jeu. Les sauvegardes et fichiers de réglages ne sont pas
inclus.

## Emplacements

| Composant | Dossier sous `%LOCALAPPDATA%` |
|---|---|
| Hub | `NimbyRailsFrance/logs/hub` |
| SDK natif et passerelles Windows | `NimbyRailsFrance/logs/sdk` |
| Chargeur SDL et chargeur externe | `NimbyRailsFrance/logs/loader` |
| Mods Kotlin et adaptateur, dont SFR | `NimbyRailsFrance/logs/mods` |
| Client Kotlin du SDK | `NimbyRailsFrance/logs/sdk-client` |
| TCO | `NimbyRailsFrance/logs/tco` |

Le TCO propose **Journaux**, le banc **Fichiers de logs**. Les noms natifs
contiennent le nom de la DLL/exécutable ; les noms JVM contiennent le composant
et le PID. Chaque entrée porte une heure UTC et les informations de processus.
Le Hub conserve aussi ses journaux après fermeture.

`NRF_LOG_DIR` peut remplacer la racine `NimbyRailsFrance/logs` pour une session de
validation. Créer ce dossier avant de lancer les composants et définir la même
valeur pour le jeu, le Hub et les outils. `--data-dir` du Hub déplace ses réglages, sans déplacer
les journaux. Aucun journal n'est stocké dans le répertoire Steam.

## Contenu et rétention

- Connexions, chargements, arrêts, opérations en échec, codes du SDK et contexte
  de l'appel. Les applications JVM enregistrent les causes et exceptions
  secondaires, ainsi que leurs exceptions non interceptées.
- Les erreurs Kotlin/Native ne sont plus réduites à `-1` : l'adaptateur lit le
  message et la pile sur le thread fautif, puis les écrit dans le log du mod.
  L'export privé est facultatif pour conserver la lecture des anciens modules.
  Les anciens binaires doivent toutefois être recompilés pour produire ces détails.
- Les anomalies de signalisation SFR sont enregistrées dans les logs du mod,
  avec la couverture physique, les identifiants et les motifs observés. L'ancien
  fichier temporaire `sfr-bal-faults.jsonl` n'est plus le journal de production.
- Pas d'écriture à chaque lecture réussie du jeu. Les erreurs consécutives
  identiques sont regroupées pendant 30 secondes, avec un compteur à l'entrée
  suivante. Une fermeture brutale peut empêcher l'émission du dernier compteur.
- Natif : rotation autour de 1 Mio, trois archives par module ; message borné à
  64 Kio. Les attentes entre processus sont bornées, les messages simultanés
  omis sont comptés à la prochaine écriture. Le logger préserve `GetLastError`.
- Client/TCO/banc : rotation autour de 2 Mio et trois archives par processus ;
  nettoyage des anciens fichiers du même composant au démarrage. Hub : cinq
  archives. Les échecs d'écriture sont signalés sur stderr ou le canal de debug
  natif et n'annulent pas une restauration ou une commande.

Le ZIP ajoute un résumé des versions enregistrées dans le Hub, du profil et du
système. Il inclut les anciens journaux de chargeur connus lorsqu'ils existent.
`technical.json` ajoute la version/build/révision Windows, architecture, mémoire,
heure/fuseau/démarrage système, GPU et pilote, chemin du jeu, build Steam,
versions des projets installés/actifs, DLL et exécutables avec taille, date,
version produit et SHA-256. Si le jeu est déjà ouvert, les modules chargés dans
ce seul exécutable sont listés. Le collecteur ne lance pas le jeu et ne charge
aucune DLL inventoriée ; il exécute uniquement un helper PowerShell en lecture.
Les chemins locaux peuvent contenir le nom d’utilisateur. Aucun identifiant de
propriétaire Steam, variable d’environnement complète ou ligne de commande
n’est collecté. Les DLL elles-mêmes ne sont pas incluses.

L’inventaire parcourt le dossier du jeu (un niveau) et les projets enregistrés
(trois niveaux), sans suivre les liens. Il est limité à 1024 fichiers/65 secondes ;
les fichiers de plus de 256 Mio ne sont pas hachés. Les limites/erreurs figurent
dans `notes`, ou `collectionError` si le collecteur ne peut pas terminer.

Il est borné à 128 fichiers, 8 Mio par fichier et 64 Mio de contenu de logs :
`collection.txt` indique les fichiers absents, tronqués ou omis. L'export peut
se faire pendant que les applications tournent ; il reflète les octets lisibles
à cet instant, pas une capture atomique de tous les processus.

Un journal n'est pas un dump mémoire : une violation d'accès native, un arrêt
forcé du processus ou une panne système peuvent interrompre l'écriture. Joindre
alors également le message de plantage Windows et l'heure exacte. Ces fichiers
permettent un diagnostic ; ils ne garantissent pas que tout défaut sera
reproductible à partir du seul log.

## Maintenance

### Blocage d'un mod et disparition des textures

Le chargeur écrit `Mod watchdog context` avant d'arrêter un processus qui ne
répond plus. Cette entrée conserve le mod, le PID/TID, l'étape en cours
(`stage`), son contexte numérique (`detail`), la durée du traitement et de
l'étape, le dernier échange RPC avec son code de retour, ainsi que les limites
et un relevé de mémoire/CPU. Ces relevés décrivent l'état au moment du diagnostic :
une mémoire élevée ne prouve pas, à elle seule, la cause du blocage.

Les marqueurs distinguent capture du jeu, réglages, calcul des signaux,
publication des règles de conduite, diagnostic du mod et publication/nettoyage
des textures. `detail` représente, selon l'étape, le nombre d'éléments, l'index
du panneau (0 pour le panneau principal) ou l'identifiant du signal d'une
action. `stage=unspecified` peut correspondre à un ancien mod non recompilé ou
à un marqueur momentanément illisible ; ce n'est pas une étape du jeu.

Un traitement encore actif après une seconde produit `Mod callback slow`.
Un traitement lent qui termine produit `Mod callback completed slowly`, avec
son étape la plus longue. Chaque type de résumé est limité à une entrée par
processus mod toutes les 30 secondes, avec compteur de répétitions omises.
Les marqueurs ne renouvellent jamais la limite du watchdog et aucune écriture
n'est ajoutée aux cycles rapides réussis. L'horloge exclut veille/hibernation.

Si le serveur RPC lui-même reste bloqué, son superviseur peut être retenu dans
l'appel. Le côté mod écrit alors `Mod RPC channel unavailable` après son attente
bornée, avec l'opération, l'étape, la durée et la raison (`reply_timeout`, arrêt
du jeu, échec d'attente ou réponse invalide). Ce message permet de distinguer
ce cas d'un blocage dans le calcul local du mod, sans ajouter un thread de
surveillance à chaque mod.

`Texture publication rejected`, dans les logs SDK, précise la première cause
du rejet : signal absent, doublon, chemin manquant/ambigu, catalogue invalide,
changement de monde ou refus du pont. Le message contient le propriétaire,
le PID cible, la ligne et le signal concernés, le lot et les chemins bornés.
Au maximum quatre entrées sont émises par thread de publication sur cinq
secondes ; une publication réussie n'ajoute aucun log.

Les anciens mods restent chargeables, mais les étapes détaillées nécessitent
de les reconstruire avec l'adaptateur actuel. Le SDK et son exécutable hôte
doivent provenir du même paquet (protocole privé 3) ; le protocole public des
mods reste V1. Le Hub inclut déjà ces dossiers dans son export diagnostic.

Le sink Windows est dans `include/nimby/detail/platform/windows/diagnostics.hpp`.
Il est compatible avec le proxy compilé sans exceptions et n'est jamais appelé
depuis `DllMain`. Les points d'appel communs utilisent un sélecteur indépendant
de l'OS ; aucun développement ni paquet Linux n'est ajouté.

Le client JVM fournit `fr.nimby.sdk.DiagnosticLog.forComponent("mon-outil")`.
Utiliser `write("contexte", exception)` aux frontières des opérations ; ne pas
journaliser toutes les observations ni intercepter silencieusement une erreur.
`startApplication(version)` appartient à l'entrée d'un exécutable autonome,
jamais à une bibliothèque chargée dans une autre application.

Tests autonomes : persistance UTF-8, rotation, regroupement, échec d'écriture,
exceptions Kotlin transportées par ABI, conservation des codes Win32, export
des seuls logs et refus d'écraser un ZIP existant. Aucun jeu n'est lancé.
