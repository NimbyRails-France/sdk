# Commandes de recette SDK + mod

Cette couche permet de piloter le mod chargé dans un processus de jeu donné,
avant de construire l'interface du banc de test. L'outil est
`nimby_sdk_control.exe` ; `help` affiche sa syntaxe complète. Le client JVM
expose les mêmes opérations dans `NimbyClient` et `ModControlSession`.

## Inventaire

| Fonction | Appel Kotlin JVM / commande CLI | Effet |
|---|---|---|
| État et capacités du mod | `modControl(Status)` / `status` | Génération observée, validité restante, nombre de surcharges |
| Prendre le contrôle | `acquireModControl` / `acquire` | Bail exclusif sur ce mod, de 1 à 60 secondes réelles |
| Prolonger le contrôle | `renew` / `renew` | Renouvellement explicite ; aucun thread caché de renouvellement |
| Forcer une indication | `forceSignal` / `force-signal` | Décision validée par le mod, réinjectée dans la résolution du réseau |
| Restituer un signal | `restoreSignal` / `restore-signal` | Retrait de sa surcharge ; recalcul normal au prochain cycle |
| Réglage temporaire | `setSetting` / `setting` | Surcharge d'une case du schéma du mod, sans écrire le fichier de réglages |
| Restituer un réglage | `restoreSetting` / `restore-setting` | Retrait d'une seule surcharge |
| Plafond par train | `constrainTrain(..., SpeedLimit)` / `train ... 0 ...` | Plafond numérique jusqu'au signal choisi ou au prochain signal |
| Marche sous contrôle de distance libre | `constrainTrain(..., PhysicalClearance)` / `train ... 1 ...` | Plafond fourni par l'appelant, abaissé selon la distance libre observée |
| Arrêt demandé | `constrainTrain(..., Stop)` / `train ... 2 ...` | Plafond nul ; vitesse demandée obligatoirement nulle |
| Restituer un train | `restoreTrain` / `restore-train` | Retrait de la contrainte de recette de ce train |
| Lire la décision calculée | `readSignal` / `read-signal` | Aspect et motif du dernier cycle terminé |
| Lire l'exécution d'un train | `readTrain` / `read-train` | En attente, actif ou terminé ; signal de sortie lié |
| Effacer les surcharges | `clear` / `clear` | Conserve le bail mais retire toutes les surcharges |
| Rendre le contrôle | `close` / `release` | Retire les surcharges et libère le bail |
| Lire l'horloge | `capture().clock` / `clock-read` | Époque et ticks observés |
| Changer la date | `setSimulationDateTime` / `clock-set` | Secondes UTC entières ; phase native conservée |
| Changer la date et recalculer les services | `setSimulationDateTime(..., true)` / `clock-set ... recalculate` | Résultat d'horloge et nombre d'interventions natives |
| Forçage visuel seul | `showSignalTextureFor`, `restoreSignalTexture` | API séparée : n'autorise aucun mouvement et ne change pas la décision du mod |
| Observations de recette | `capture(trainId)` | Trains, voies, signaux, services, occupations, réservations et chemin disponible |

Le changement de date est une mutation du monde, pas une surcharge temporaire :
`release` ne remet pas l'ancienne date. Les données absentes restent absentes ;
elles ne constituent jamais une validation réussie.

## Séquence d'utilisation

1. Se connecter au PID explicite avec la DLL SDK reconstruite.
2. Lire `status`. Une génération nulle signifie qu'aucun monde suffisamment
   récent n'est disponible dans le mod.
3. Acquérir un bail pour cette génération. Un autre propriétaire est refusé.
4. Envoyer les commandes puis observer leurs effets ; renouveler le bail pendant
   la recette. Lire seulement un accusé de réception ne valide pas le comportement.
5. Restituer les objets concernés ou libérer le bail, puis vérifier les observations.

Exemple Kotlin, dont les paramètres viennent du scénario/mod :

```kotlin
fun applyRecipe(client: NimbyClient, modId: String, train: Long,
                signal: Long, aspectCode: Int, maximumMps: Double) {
    client.acquireModControl(modId).use { recipe ->
        recipe.forceSignal(signal, aspectCode)
        recipe.constrainTrain(train, maximumMps,
            TrainControlMode.PhysicalClearance, exitSignal = signal)
        // La boucle de recette doit maintenant renouveler le bail, lire les
        // observations et vérifier le résultat attendu AVANT de sortir du use.
    }
}
```

Le fragment illustre la soumission et la restitution ; il n'attend pas le passage
du train et n'est donc pas, à lui seul, une recette complète.

Pour le CLI, les identifiants, génération et propriétaire sont des entiers
décimaux. `status` fournit la génération ; choisir un propriétaire non nul propre
à la recette et le conserver dans les commandes suivantes. La vitesse est en m/s.
Le CLI affiche une réponse JSON et renvoie un code non nul en cas d'échec.

## Contrat d'exécution

- La réception modifie une surcharge. L'application aux décisions, textures et
  publications natives intervient au prochain cycle d'observation réussi.
- `read-signal.active` vaut 0 sans forçage demandé, 1 lorsque la dernière décision
  diffère encore du forçage, 2 lorsqu'elle correspond. Cela ne prouve pas encore
  que le rendu du jeu ou le mouvement du train a été observé.
- `read-train.active` vaut 0 sans contrainte native, 1 en attente de liaison au
  signal de sortie, 2 actif, 3 terminé après passage, 4 annulé par changement de
  mouvement ou remise à zéro du parcours. Une annulation ne valide pas un passage.
  Renouveler la publication d'une commande
  terminée ne la réarme pas ; une nouvelle commande explicite crée une révision.
- `exitSignal = 0` lie le prochain signal natif observé strictement devant la
  tête. Un identifiant explicite lie ce signal dans le parcours observé. Tant que
  cette limite est inconnue, le plafond est nul. Un signal hors parcours reste
  en attente ; aucune distance de canton n'est inventée.
- La libération utilise le passage mesuré de la tête, ou de la queue si demandé.
  Être exactement sur la limite ne constitue pas encore un franchissement.
  Le remplacement du mouvement ou un recul de sa distance invalide la commande.
- Les contraintes par train sont des restrictions supplémentaires. Elles
  conservent les limitations plus strictes et les refus natifs de réservation.
  Le mode de distance libre ne constitue pas une autorisation de franchir un
  signal fermé : cette autorisation reste une consigne du mod.
- Sans couverture physique récente, le mode de distance libre demande l'arrêt.
  Les paramètres physiques actuellement employés par le moteur restent ceux de
  son modèle existant ; la vitesse maximale vient toujours de l'appelant.
- Un seul producteur de contraintes par train est accepté simultanément dans
  le pont natif. Un autre producteur est refusé et ne peut pas effacer le premier.
- Le bail emploie une horloge monotone réelle : il expire même si le jeu est en
  pause. Changement de monde, perte d'observation ou arrêt du mod retirent les
  surcharges. Une publication native de contraintes expire aussi après une
  seconde sans renouvellement. La restitution reste soumise au cycle du jeu.
- Après une panne du mod, les mémoires restrictives ordinaires de signalisation
  gardent leur comportement conservateur existant ; retirer une surcharge ne
  garantit pas une reprise immédiate de la marche.
- Aucun appel de mutation n'est automatiquement répété. En cas d'erreur de
  transport après émission, son résultat est incertain : relire état et
  observations avant de décider d'une nouvelle action.

## Extension par un mod

`SignallingMod.forcedDecision(aspect)` valide le code et fournit l'aspect **et le
motif**. Son implémentation par défaut refuse le forçage. `drivingRule` traduit
ensuite cette décision selon les règles du mod. Le SDK n'interprète ni couleur
BAL ni vitesse nationale. Les indices de réglages sont ceux de `checkboxes`.

Le runtime applique le forçage avant la résolution des liens aval : les signaux
amont voient la décision forcée. Les commandes de calcul hors jeu conservent
leurs entrées explicites et ne lisent pas les surcharges d'une recette en cours.

## Organisation et validation

Contrat C : `include/nimby/detail/control.h`. État commun :
`control_lease.hpp` et `signalling_control.hpp`. Transport Windows :
`include/nimby/detail/platform/windows/control_pipe.hpp` et
`src/platform/windows/runtime/control_client.cpp`. Intégration du mouvement :
`engine/train_constraint.h`, `automatic_controller.h` et le pont Windows.
Linux dispose d'un point d'entrée explicite retournant `HOOKS_UNAVAILABLE` ;
aucun contrôle natif Linux n'est annoncé comme fonctionnel.

Tests : contrat de bail et de mouvement, surcharges de réglages/décisions,
aller-retour réel du canal Windows, appels successifs, arrêt avec client muet,
intégration native avec callbacks contrôlés, transport JVM et politique SFR.
Les tests d'adaptateur n'attestent pas l'exactitude des adresses dans un jeu en
cours. Cette nouvelle couche doit encore faire l'objet d'une recette dans le
jeu redémarré avec le SDK et le mod reconstruits, sur une copie de sauvegarde.
