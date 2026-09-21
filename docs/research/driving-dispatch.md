# Recherche du point de commande de conduite — 20 septembre 2026

État : recherche statique, aucun actionneur ajouté ni injecté dans le jeu.
Le calculateur BAL du mod propose déjà une enveloppe et une accélération ;
`SignallingRuntime::DrivingResult::commandApplied` reste faux.

## Preuves et limites

Binaire étudié : SHA-256
`fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae`.
Les adresses ci-dessous sont les VA de ce binaire dans Ghidra, pas des adresses
à utiliser dans un processus avec ASLR.

La [documentation du jeu](https://wiki.nimbyrails.com/NimbyScript) décrit :

- `event_signal_lookahead`, avec distance sur le parcours du train et résultat
  `max_speed` en m/s. Zéro est ignoré : ce champ ne suffit pas à demander l'arrêt.
- `event_signal_check`, qui peut rendre un contrôle plus restrictif.
- `control_train`, toutes les six secondes simulées, après extrapolation C++
  et avant traitement de la file de commandes.

Ces interfaces documentées servent à identifier les mécanismes natifs.
Elles ne constituent pas une API du SDK C++ ni une implémentation NimbyScript
du mod. Un plafond natif ne prouve pas que la décélération suivra notre modèle.

Décompilation de `140424c00` : enregistrement des noms et signatures,
par `140426a00`, dans un objet référencé par `*param_1` :

| Événement | Offset dans cet objet | Destination associée dans `plVar28` |
| --- | --- | --- |
| `event_signal_check` | `0xd48` | `+2` éléments de 8 octets |
| `event_signal_lookahead` | `0xe20` | `+3` éléments de 8 octets |
| `event_signal_texture_state` | `0x1180` | `+7` éléments de 8 octets |
| `control_train` | `0x1258` | `+8` éléments de 8 octets |

**Ce sont des emplacements d'enregistrement, pas des points de commande.**
Ne pas les appeler comme des fonctions de freinage. Les types et durées de vie
des objets de cette table ne sont pas établis ici.

Le balayage des instructions portant `0xe20` ou `0x1258` retrouve
`140424c00`, la construction des signatures dans `140428790`, ainsi que
la construction/destruction dans `140437d30` et `14043a010`.
Il produit aussi des occurrences sans lien démontré avec ces objets.
Il ne révèle pas directement un appel de conduite. L'absence d'un accès à ces
offsets ne démontre pas l'absence d'un événement : son exécution peut passer
par la table associée ou un accès indirect.

## Reproduction

Outil SDK : `tools/InspectDrivingDispatch.java`. Paramètre unique : dossier
de sortie. Lancer avec Ghidra `-readOnly -noanalysis`, sans attacher le jeu.
Il enregistre le hash, les occurrences et les décompilations candidates.
Les variables locales sur RSP/RBP sont listées mais exclues de la sélection
automatique des fonctions à décompiler ; cette heuristique peut aussi manquer
des accès pertinents et n'est pas une preuve d'exhaustivité.

Résultats de cette passe dans le projet SFR :
`build/driving-native/140424c00.c`, `build/driving-native-refs.txt` et
`build/driving-dispatch/dispatch-offsets.txt`.

## Travail restant avant une commande réelle

1. Suivre la destination associée à l'événement pour identifier son appelant
   pendant la simulation, ses arguments, son thread et sa durée de vie.
2. Relever la distance réelle au signal et le franchissement, avec identité
   complète du train et de la session. Une fraction de voie ne donne pas seule
   une distance en mètres.
3. Observer la consommation du plafond, l'arbitrage avec les limitations natives
   et le traitement d'une cible d'arrêt. Garder l'arrêt distinct du plafond nul.
4. Valider un point d'application synchronisé au temps simulé. Le test BAL a
   mesuré environ 426 secondes simulées par seconde réelle ; une interrogation
   externe même rapide ne garantit pas la progression d'une courbe de conduite.
5. Mesurer vitesse et déplacement lors d'un essai borné : freinage, arrêt,
   reprise, pause, temps accéléré, perte des données et retrait du mod.

Le SDK doit porter l'observation, la synchronisation et l'application native.
Le mod conserve les règles françaises, les cibles et leur mémoire. Aucune
écriture répétée de vitesse instantanée n'est retenue comme actionneur.

## Live fixes, 2026-09-20

The native TER 2NNG 070 motion had material maximum 44.4444466 m/s,
a timetable cruise value of 40 m/s at Motion+0x478, and a 160 km/h track.
The integrator at RVA 0x378600 independently bounds speed by both material
and track speed, and retains its distance/target braking arguments.
The opt-in maximum-line-speed mode replaces the cruise ceiling while retaining
the native scripted ceiling at +0x49c when +0x4a0 is active.

Provisional lookahead now computes envelopes without retaining stop/held
instructions. Retention requires a measured native movement across the source;
reopening before crossing no longer imposes a fictitious 30 km/h approach.
Telemetry V2 includes native and chosen ceilings.

A separate physical occupation bug caused a deadlock at signal 11319.2.
R2N R079 footprint began at 0.16125698806003208 while the signal fraction was
0.16125698806003211. Closed intervals therefore counted a train waiting at the
signal as already in its protected block. Oriented BlockEntry excludes contact
at the entry with an eight-double-epsilon tolerance; exit contact remains
occupied. Tests reproduce these measured coordinates and both directions.

The next live run exposed a second retention bug: TER 2NNG 071 had a stop
target 176 m behind its head, after the native lookahead dropped that signal.
Verified native forward movement now releases passed targets independently
of continued lookahead membership. A provisional boundary record handles
integration steps ending exactly on a signal; its current rule is rechecked
before a later actual crossing. Regression tests cover both cases.

The signalling worker retries the entire observation up to two times when
presence/occupation coverage is incomplete. It never combines successive
tables or treats old clear data as fresh. Persistent uncertainty remains
restrictive. Final installed live run (PID 26136): 232 post-load observations,
14 trains in the BAL zone, S/A/VL texture commands observed and no fault
texture on the five active signals. This is bounded validation, not a guarantee
against every future incomplete read. Detailed evidence is in the SFR project,
`docs/bal-validation-2026-09-20.md`.
