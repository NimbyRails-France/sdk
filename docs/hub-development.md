# Tester un mod avec le Hub

Le Hub orchestre la compilation et les installations de développement.
Le SDK et son plugin Gradle définissent comment construire le mod.

## Préparer l'environnement

Dans les paramètres développeur du Hub, choisir :

- le dossier du kit Kotlin utilisé pour compiler, contenant `sdk.json` ;
- le SDK d'exécution destiné aux essais ;
- le dossier réservé aux installations de développement.

Le kit Kotlin n'est pas le paquet d'installation du loader. Le Hub transmet
son chemin à Gradle avec `-PnrfSdkDir=...`. Pour tester un mod compilé par
le Hub, la version du kit doit correspondre au SDK d'exécution sélectionné.

Le Hub empaqueté a besoin d'un JDK accessible via `JAVA_HOME` pour lancer
Gradle. Utiliser JDK 21 avec ce SDK.

## Ajouter un projet neuf

Sélectionner **Ajouter un projet local**, puis son dossier source.
Un projet Kotlin créé depuis l'exemple du SDK fournit un `mod.json` complet
et le Wrapper Gradle. Le Hub peut donc l'ajouter avant la première compilation,
sans paquet préexistant ni manifeste `project.json` rédigé à la main.

**Compiler** appelle `packageMod`. Cette tâche vérifie le mod, crée le ZIP
et génère ses métadonnées. Le Hub recalcule l'intégrité du paquet puis le
prépare dans son dossier de développement.

Un projet personnalisé peut toujours utiliser `hub-local.json` pour déclarer
sa tâche, son manifeste de distribution et le chemin de son archive.

## Choisir la version active

Le profil **Jouer** utilise les versions publiées installées.
Le profil **Développer** permet de choisir la version publiée ou le projet
local de chaque mod, avec un seul exemplaire actif pour un même identifiant.

Le paquet local ne remplace pas les fichiers de l'installation publiée.
Le Hub applique les changements avec le jeu arrêté et restaure les liaisons
requises lors du retour au profil habituel. Retirer un projet du profil ne
supprime pas ses sources.

Après un échec de compilation, le résultat n'est pas prêt à tester : le Hub
ne réutilise pas silencieusement un ancien binaire. Une modification des sources
ou du kit SDK invalide également la préparation. Les dossiers locaux
`tools/`, `local-notes/` et `captures/` sont exclus de l'empreinte des sources ;
le projet ne doit pas les utiliser comme entrées du build.

## Sauvegardes et réglages

La séparation des paquets ne signifie pas que toutes les données du jeu sont
isolées. Le Hub actuel demande un acquittement pour les données partagées.
Les profils de réglages du SDK peuvent dépendre de l'identifiant du monde :
une copie de sauvegarde peut conserver cet identifiant.

Utiliser une partie d'essai et conserver ses sauvegardes avant une expérimentation.
Ne pas présenter les profils du Hub comme une isolation complète des parties
tant que le jeu et le stockage des réglages ne la fournissent pas.