# Lecture ciblée de conduite — Kotlin, SDK 0.8.0

Pour un outil Kotlin/JVM, `NimbyClient.readTrain(id)` lit un seul train sans
capturer le réseau entier. Réutiliser la connexion pour conserver le handle
du processus. Cette lecture n'écrit pas dans le jeu et n'installe aucun hook.

```kotlin
import fr.nimby.sdk.NimbyClient
import java.nio.file.Path

NimbyClient.open(Path.of(library), pid).use { client ->
    val sample = client.readTrain(trainId)
    if (sample != null) {
        val speed = sample.speedMps
        val position = sample.position
        val length = sample.currentDynamics?.lengthM
        val braking = sample.currentDynamics?.serviceBrakingMps2
        // Caractéristiques du matériel : aucune commande de freinage ici.
    }
}
```

`library`, `pid` et `trainId` viennent du choix de DLL, du processus et du train
fait par l'application. Les identifiants sont opaques ; utiliser ceux du SDK.
Le parcours de connexion est sur le [wiki](https://wiki.nimbyrails-france.fr/lire/connexion).

## Contrat de lecture

- `null` signifie train absent, partie indisponible ou lecture instable. Une
  ancienne observation ne remplace jamais une nouvelle lecture manquante.
- Un identifiant d'un autre type est refusé. Les erreurs natives remontent sous
  forme de `SdkException`, notamment le statut `11` lorsque le jeu est fermé.
- `speedMps` et `position` sont optionnels. Un zéro mesuré est une vitesse valide ;
  le zéro de présentation signalé par `speedDefaulted` reste sans vitesse mesurée.
- `purchasedDynamics` et `currentDynamics` sont indépendants. Aucun modèle acheté
  ne remplace implicitement une dynamique courante absente. Les valeurs non finies
  sont refusées comme données de conduite.
- `TrainDynamics` expose vitesse et accélération maximales, freinages de service
  et d'urgence, effort de traction, puissance, masse à vide et longueur, en SI.
  Ce sont les paramètres déclarés du matériel, pas des performances mesurées.
- `elapsedBeginMillis` et `elapsedEndMillis` encadrent la lecture en temps simulé.
  `capturedAtMillis` est l'heure système de fin de lecture.
- `sessionGeneration` appartient à la connexion. Réinitialiser les mémoires
  dérivées lors d'une reconnexion ou d'un changement de génération ; ce compteur
  n'est pas un identifiant universel de sauvegarde.

Le résultat contient uniquement des valeurs JVM copiées. Il reste utilisable
après une autre lecture ou après `close()`. Les appels sur une connexion sont
sérialisés. Le client n'installe ni boucle de rafraîchissement ni reconnexion
automatique : l'application choisit sa cadence et le traitement des erreurs.

Le moteur natif contrôle les records et leur identité plusieurs fois, avec au
maximum trois essais immédiats. Cela reste une observation optimiste, pas un
tick atomique. Cette lecture ne prouve ni une autorisation de mouvement, ni un
chemin libre, ni une distance à un signal. La pente n'est pas exposée.

## Mods Kotlin/Native et commandes

Un mod déclare ses règles en Kotlin avec `signalMod`, décrit dans le
[guide officiel](https://wiki.nimbyrails-france.fr/commencer/premier-mod). Le SDK lui transmet les
observations dans `decide(signal, next)` ; le mod fournit ses règles via
`drivingRule(decision)`. Le client JVM ci-dessus appartient aux outils externes,
pas aux dépendances d'un mod Kotlin/Native.

Le [protocole de recette](recipe-commands.md) permet aux outils Kotlin de demander
les opérations proposées par le mod. `ModControlSession.readTrain()` lit l'état
d'une contrainte de recette ; `NimbyClient.readTrain()` lit la mesure physique
ciblée. Ces deux résultats ont des rôles différents.

## Implémentation et validation

Le client public C++ a été retiré en 0.8.0. Le pont précompilé utilise une
`detail::ObservationSession` privée, sans cache de capture ni worker propre.
Ses types et le transport C interne ne sont pas des API pour les consommateurs.
Le noyau natif reste responsable de la lecture du binaire du jeu ; les offsets
du jeu ne figurent jamais dans le code d'un mod ou d'un outil Kotlin.

Les tests JVM vérifient le décodage contre une DLL de fixture compilée depuis les
headers C : taille et alignement, données optionnelles, vrai arrêt, zéro par
défaut, valeurs non finies, erreurs et durée de vie des copies. Les tests natifs
vérifient les gardes de lecture et la libération des handles. Ces tests autonomes
ne remplacent pas la recette en jeu du SDK 0.8.0.
