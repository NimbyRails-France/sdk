# Runtime générique de signalisation

Cette note décrit l'adaptateur natif interne du SDK. Depuis 0.8.0, les auteurs
de mods utilisent l'API Kotlin/Native ; les types C++ ci-dessous ne sont pas une
API publique à intégrer dans leur projet.

`<nimby/signalling_runtime.hpp>` fournit `nimby::SignallingRuntime<Rules>`.
Le point d'entrée d'un mod retourne simplement :

```cpp
nimby::Mod nimby::createMod() {
    return nimby::SignallingRuntime<MyRules>::createMod();
}
```

Le SDK fournit les messages typés, les sept commandes (évaluation, conduite
calculée, lecture du train, rendu, réseau, occupation fournie, occupation réelle),
le raccordement aux lectures haut niveau et l'enregistrement dans l'adaptateur.
Aucun aspect national n'est défini ici. Les exports et le cycle de vie restent
dans l'adaptateur SDK existant.

## Contrat du profil

Le profil fournit les types `Settings`, `Observation`, `Decision`, `Signal`,
`Vehicle`, `DrivingSettings`, `DrivingInput`, `Constraint` et `Plan`.
Les messages échangés doivent respecter le contrat de valeurs trivialement
copiables des [commandes de mod](mod-commands.md).

Il fournit aussi :

- `evaluate(settings, observation)` : décision nationale d'un signal.
- `decide(signal, optionalNextDecision)` : décision pour l'évaluation du réseau.
- `unknownDecision()` et `invalidNetworkDecision()` : diagnostics métier.
- `texture(decision, simulationMs, halfPeriodMs)` : chemin relatif dans le catalogue.
- `plan(vehicle, settings, input, constraints)` : modèle de conduite du mod.
- `withDynamics(configuredVehicle, observedDynamics)` : adaptation du matériel.
- `maxSignals`, `textureSet` et `names` (`SignallingCommandNames`) : configuration statique.

`DrivingInput` accepte, dans cet ordre, tête en mètres, vitesse, limite de ligne,
fraîcheur, route connue, marche à vue et distance visible optionnelle. Son champ
`visibleClearM` reçoit cette distance. Les distances viennent de l'appelant.
Les listes sont bornées à 4096 contraintes, 512 portions et 4096 emprises.
L'observation Kotlin automatique est limitée à 4096 signaux. Les messages C++
internes gardent leur capacité fixe `Rules::maxSignals`, distincte de la limite
`maxLiveSignals` de l'observation automatique.

Les messages sont des types imbriqués du runtime. Un adaptateur natif interne
peut les réexporter par alias, sans recopier les structures ni le transport.
Le nom/version d'une commande doit changer si sa disposition binaire change.

## Observation automatique optionnelle

Un profil qui fournit `fromLive(const nimby::LiveSignalState&)` active le service
d'observation de l'adaptateur. Cette fonction convertit les faits SDK (ID, aval,
occupation, fraîcheur, limite résolue) en entrée métier du mod. Le SDK sélectionne
les signaux de chemin portant le catalogue `Rules::textureSet` et construit leurs
cantons. Une frontière vers un signal extérieur au profil reste inconnue.

Le service tourne dans le worker isolé du mod lorsqu'il a été démarré par le SDK
du jeu. La résolution interne du PID désigne alors le jeu cible identifié, pas
le PID du worker. Le chemin résident dans `NIMBYRails.exe` reste reconnu. Un hôte
de diagnostic ordinaire n'acquiert pas ce contexte et ne lance pas ce service
contre un processus choisi arbitrairement. L'adaptateur attend une simulation lisible,
invalide les affichages sur erreur et joint son thread lors de l'arrêt explicite.
`modObservationStatus()` expose disponibilité, succès et échecs de cette instance.

Le rendu automatique utilise des baux de 2,5 secondes, renouvelés après lecture.
Les deux phases graphiques sont obtenues via `Rules::texture` puis animées par le
pont de textures au temps simulé lu lors du rendu, indépendamment de la fréquence des
captures. Les substitutions expirent même si la restauration échoue. La lecture
du temps simulé dans le pont est propre au binaire identifié ; aucun train ni
champ de sauvegarde n'est écrit. Les anciens ponts résidents sont refusés.

## Limites et tests

Le rendu de textures ne change aucune permission native. Les profils fournissant
`drivingRule` publient séparément leurs règles numériques au pont de conduite,
avant les textures, avec un bail réel d'une seconde. `commandApplied` dans le
résultat de calcul reste faux : ce résultat n'est pas une preuve d'application
native ni un actionneur de freinage supplémentaire. Une donnée absente ou une vitesse par
défaut ne constitue pas une mesure. La lecture réelle des cantons conserve les
[limites de couverture](blocks.md). Les profils sans `fromLive` restent uniquement
pilotés par commandes. Les fonctions sont appelées hors `DllMain`.

La capture de signalisation ciblée, les publications groupées, les réglages live
et les raccordements de bifurcations sont pris en charge. Les lectures de présence
et d'occupation restent globales pour prouver la couverture ; leur coût dépend
de la partie. Les captures complètes restent disponibles aux outils qui demandent
ces données. L'expiration d'une texture peut rendre la main au rendu natif si une
capture prend trop longtemps : elle ne constitue pas une décision BAL ni la
garantie d'une indication restrictive.

`mod_observation_loop` vérifie perte/reprise, arrêt en cours de traitement,
redémarrage et interruption d'une longue attente. `mod_entry_adapter` vérifie que
l'hôte de diagnostic ne déclenche aucune observation automatique. Les tests du
pont couvrent le choix des phases et l'expiration ; ils ne remplacent pas un essai
visuel de toutes les indications dans la partie.

La demi-période lumineuse doit être entre 100 et 10000 ms ; ce contrôle technique
ne définit pas une fréquence réglementaire.

`generic_signalling_runtime` utilise un profil numérique indépendant de BAL et
des lectures simulées. Il vérifie conversion du matériel, refus des données
absentes/par défaut, messages bornés, réseau, occupations et commandes.
Les tests des règles nationales restent dans les mods.

`modObservationStatus()` also reports consecutive failures, last poll duration
in milliseconds, and bounded `lastError` text. The last failure text is retained
after recovery for diagnosis and cleared on restart; `available` describes the
latest completed poll. Lost-observation cleanup still runs after each failure.
These diagnostics do not relax freshness or replay an old observation.

Snapshot exceptions now include the consistency stage (live roots, trains,
network, roots after network, roots after occupations, train track reference,
or roots after textures). The original private capture entry point remains;
the diagnostic variant additionally reports a stage, zero on success. This
instrumentation does not retry or relax consistency checks.

### Capture dediee a la signalisation

La méthode privée `detail::ObservationSession::captureSignalling()` (ou
`capture(SnapshotScope::Signalling)`) conserve les identités des trains, leur
présence physique vérifiée, les occupations complètes, les voies et bifurcations,
les signaux et références de catalogues, l'horloge et l'identité de session. Le
contrôle global de couverture reste obligatoire avant de déclarer un canton libre.

Ce profil omet les noms, vitesses et positions de tête des trains, services,
horaires, voyageurs, plans de ligne et chemins de trains, réservations,
stations/quais et résolution des fichiers de textures. Ces données ne forment
pas une observation complète du jeu. La capture complète et la lecture ciblée
d'un train restent des opérations distinctes. Les contrôles des IDs complets,
des racines et de la présence sont conservés ; les relectures Motion restent nécessaires.

SignallingRuntime choisit automatiquement ce profil ; les autres consommateurs Mod restent par defaut sur Complete. readBlocks() utilise aussi la capture dediee. Aucun changement de regles BAL requis dans le mod.

La variante non ciblée relit la topologie générale. La variante choisie par le
runtime ci-dessous limite aussi le parcours des voies aux cantons concernés.
Aucune géométrie ancienne n'est réutilisée sans preuve de validité.

### Scoped fast signalling

`detail::ObservationSession::captureSignalling(textureSet)` rereads signal discovery and a bounded set of native track records up to the next boundaries. It returns only the scoped topology/catalogue, with global train presence and occupations for coverage checks. It never reuses geometry across captures. Track IDs, connections and branch records remain validated; omitted neighbors stay explicit so incomplete geometry cannot masquerade as an end of track. The unscoped overload remains available for general diagnostics.

SignallingRuntime selects this scope and requests a 20 ms observation interval.
Each next start is eligible at the later of the actual previous start plus the
interval and the completed cycle, including error cleanup. A 23 ms cycle need
not wait for a 40 ms grid slot; no missed poll is queued for catch-up. An already
due deadline skips the timed wait. Windows scheduling and the cycle's work still
determine actual cadence: a requested 20 ms interval is not proof of sustained
50 Hz. Wake delay and actual start intervals are recorded separately from the poll callback,
cleanup and cycle duration. Unchanged texture leases renew after 500 ms, changed
indications are sent immediately. This remains a non-atomic polling observer,
not a simulation-tick hook or an unlimited-speed guarantee. The mod owns no native
scheduling or memory access.

### Batched rendering and texture ownership

The live runtime now publishes all changed/renewed textures in one bounded batch.
The SDK reads one catalogue, validates every signal and the world roots, and only
then commits the batch. Invalid members or a world change leave the previous
render table intact. Signal lookup and ownership bookkeeping are linear in the
observed signal count; the control mutex is released before diagnostics/rendering.
Numeric driving instructions are published before consumer diagnostics.

Windows retains a process connection per SDK thread, with live roots reread on
every call. In-process publication and bulk status queries call the resident SDK
bridge directly, without the shared legacy mailbox or its per-reply polling wait.
Legacy external diagnostics hold the mailbox mutex only while sending a request,
never while reading the catalogue. A batch of up to 32 status queries uses one
direct bridge call. A resident bridge without these exports must be replaced by
restarting the game with the rebuilt SDK.

Isolated mod hosts use the texture broker operations 300–304. The broker assigns
the owner; child-supplied tokens cannot clear another mod's images. Live conflicting
signal ownership, more than 4096 images per owner, more than 64 owners, or more
than 65536 live images are rejected before mutation. Changed images and renewals
with stable ownership modify only their owner's entries in a prepared immutable
copy; preparation still copies the shared table outside the render path. The
render hook pins the current publication without waiting for a writer or freeing
old vectors. Bounded retirement can reject a writer while preserving the current
publication and its original expiry. Removing an owner or restoring selected IDs
preserves every other owner's overrides. Scoped restoration succeeds for absent
IDs and IDs subsequently owned by a peer; invalid inputs and transport errors
remain failures. Expired or invalid-world images fall back to the native renderer,
whose indication is not guaranteed restrictive.

`texture_batch_client` exercises the production resolver with controlled readers:
46 individual updates require 46 catalogue reads/publications; one 46-image batch
requires one of each. It also covers atomic rejection and one-call bulk status.
`texture_publications`, `texture_native_publication` and `mod_host_textures` cover
ownership conflicts, bounded floods, world changes, cleanup and IPC bounds. These
tests establish work reduction and isolation; they do not establish live-game
end-to-end latency.

### Récupération des textures et plafond courant

L'observation automatique Kotlin est limitée à 4096 signaux simultanés par
mod. Le suivi des substitutions, y compris les nettoyages en attente après
une erreur de transport, est lui aussi borné à 4096 identifiants. Les anciens
identifiants sont restaurés avant l'admission des nouveaux. La restauration
utilise des lots de 4096 au maximum ; chaque lot réussi est retiré du suivi,
les lots échoués restent à réessayer. Une rotation d'identifiants ne doit donc
ni faire croître indéfiniment ce suivi, ni bloquer définitivement le nettoyage
avec un lot trop grand.

Si le suivi est plein pendant un échec de nettoyage, les substitutions déjà
admises continuent d'être renouvelées et les règles de conduite continuent
d'être publiées. Les nouvelles substitutions sont refusées avec un diagnostic
explicite dans le journal du mod. Leur indication native n'est **pas prouvée
restrictive** : cette situation n'est pas annoncée comme un repli visuel sûr.
La récupération du nettoyage permet de réadmettre les nouveaux identifiants.
Aucune restauration globale des textures d'autres propriétaires n'est utilisée.

Après une erreur de poll, l'invalidation essaie chaque nettoyage indépendant
même si l'un échoue. Elle ne transforme pas une publication refusée en réussite
et ne renouvelle pas ses anciennes permissions : les règles de signal conservent
seulement leur bail d'origine, puis expirent. L'arrêt explicite demande aussi
leur retrait. Les erreurs conservées et les durées de nettoyage permettent de
distinguer un échec du cycle complet d'un échec de lecture de la capture.
