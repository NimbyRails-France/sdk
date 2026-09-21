# Runtime générique de signalisation

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
Le réseau est également soumis à la limite SDK de 4096 signaux.

Les messages sont des types imbriqués du runtime. Un mod peut les réexporter par
alias, sans recopier les structures ni implémenter des fonctions de transport.
Le nom/version d'une commande doit changer si sa disposition binaire change.

## Observation automatique optionnelle

Un profil qui fournit `fromLive(const nimby::LiveSignalState&)` active le service
d'observation de l'adaptateur. Cette fonction convertit les faits SDK (ID, aval,
occupation, fraîcheur, limite résolue) en entrée métier du mod. Le SDK sélectionne
les signaux de chemin portant le catalogue `Rules::textureSet` et construit leurs
cantons. Une frontière vers un signal extérieur au profil reste inconnue.

Le service tourne uniquement dans `NIMBYRails.exe`, avec connexion au PID courant
et identification du binaire par le SDK. Les hôtes de diagnostic ne lancent pas
ce service contre un autre processus. L'adaptateur attend une simulation lisible,
invalide les affichages sur erreur et joint son thread lors de l'arrêt explicite.
`modObservationStatus()` expose disponibilité, succès et échecs de cette instance.

Le rendu automatique utilise des baux de 2,5 secondes, renouvelés après lecture.
Les deux phases graphiques sont obtenues via `Rules::texture` puis animées par le
pont v4 au temps simulé lu lors du rendu, indépendamment de la fréquence des
captures. Les substitutions expirent même si la restauration échoue. La lecture
du temps simulé dans le pont est propre au binaire identifié ; aucun train ni
champ de sauvegarde n'est écrit. Les anciens ponts résidents sont refusés.

## Limites et tests

Le rendu ne change aucune permission native. `commandApplied` reste faux :
aucun actionneur de freinage n'est ajouté. Une donnée absente ou une vitesse par
défaut ne constitue pas une mesure. La lecture réelle des cantons conserve les
[limites de couverture](blocks.md). Les profils sans `fromLive` restent uniquement
pilotés par commandes. Les fonctions sont appelées hors `DllMain`.

La capture complète et la résolution du catalogue par affichage restent coûteuses
sur les grands réseaux. L'envoi groupé, les réglages live et les itinéraires aux
bifurcations ne sont pas encore raccordés. L'expiration peut rendre le contrôle au
jeu si une capture prend trop longtemps : elle ne constitue pas une décision BAL.

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

Client::captureSignalling() (ou capture(SnapshotScope::Signalling)) conserve les identites des trains, leur presence physique verifiee, les occupations completes, les voies et bifurcations, les signaux et references de catalogues, l'horloge et l'identite de session. Le controle global de couverture reste obligatoire avant de declarer un canton libre.

Ce profil omet les noms/vitesses/positions de tete des trains, services, horaires, voyageurs, plans de ligne et chemins de trains, reservations, stations/quais et resolution des fichiers de textures. Ces donnees ne doivent pas etre interpretees comme une observation complete du jeu. Utiliser capture() pour les diagnostics complets, readTrain(id) pour la conduite ciblee. Les controles des IDs complets, des racines et de la presence sont conserves. Les relectures Motion restent necessaires.

SignallingRuntime choisit automatiquement ce profil ; les autres consommateurs Mod restent par defaut sur Complete. readBlocks() utilise aussi la capture dediee. Aucun changement de regles BAL requis dans le mod.

La topologie est toujours relue : aucun cache inter-captures n'est introduit sans invalidation fiable des modifications de voies. Cette passe reduit les donnees lues, elle ne limite pas encore le parcours natif aux seuls cantons geres.

### Scoped fast signalling

`Client::captureSignalling(textureSet)` rereads signal discovery and a bounded set of native track records up to the next boundaries. It returns only the scoped topology/catalogue, with global train presence and occupations for coverage checks. It never reuses geometry across captures. Track IDs, connections and branch records remain validated; omitted neighbors stay explicit so incomplete geometry cannot masquerade as an end of track. The unscoped overload remains available for general diagnostics.

SignallingRuntime selects this scope and requests a 20 ms observation interval. Actual cadence includes capture, settings and render time; missed deadlines are skipped. Unchanged texture leases renew after 500 ms, changed indications are sent immediately. This remains a non-atomic polling observer, not a simulation-tick hook or an unlimited-speed guarantee. The mod owns no native scheduling or memory access.
