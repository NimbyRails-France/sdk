# Observation des trains — 14 septembre 2026

Mise à jour : [recherche réseau](network.md). Les offsets de réflexion de Motion
ne doivent pas être transposés directement au bloc vivant : la déduction du début
de Presence depuis `speed +0x18` est invalidée. Les lectures UI de la vitesse à
Motion +0x3C8 restent corroborées ; le Pos observé commence à +0x3A8.

Suite : [essais automatiques A/B](speed-experiments.md). Les quatre essais réels
autorisés ensuite confirment une différence causale entre les deux collections ;
les remarques ci-dessous sur l'absence de clic réel décrivent l'étape antérieure.

## Ajout : test ponctuel de vitesse

À la demande de l'utilisateur, le moniteur propose maintenant **Appliquer une fois** :
sélectionner un train, choisir explicitement la source A/B, saisir 0 à 600 km/h,
puis cliquer. Aucune source ni vitesse n'est présélectionnée. La borne de 600 est
une limite du test, pas une vitesse maximale déduite du jeu.

Ce clic ouvre temporairement un handle d'écriture, vérifie l'empreinte et l'identité
du processus (date de création comprise), retrouve l'ID complet dans les collections,
recontrôle le bloc, la présence et la page mémoire, puis écrit **un seul double**
à `Motion+0x3C8` en convertissant les km/h en m/s. Aucun changement de protection
mémoire, gel du jeu, boucle d'écriture ou hook n'est ajouté.

Le résultat affiche avant/demande/relecture ; l'historique des tentatives arrivées
au stade d'écriture est dans `%LOCALAPPDATA%/NimbyRailsSDK/speed-tests.log` (UTC,
PID, ID, source, collection et valeurs). Une relecture égale à la demande prouve
seulement le transfert mémoire. Le moteur peut immédiatement recalculer cette
valeur ; cela ne constitue pas une commande validée de régulation de vitesse.

Les rôles A/B et la synchronisation restent inconnus. Les vérifications précédant
l'écriture ne suppriment pas la course avec le moteur : un objet pourrait changer
entre le contrôle et l'écriture. Utiliser ce test sur une partie jetable ; le
moniteur ne touche pas directement aux fichiers de sauvegarde, mais le jeu peut
enregistrer les conséquences d'un changement d'état. Aucun retour automatique
à l'ancienne vitesse n'est tenté, car celle-ci peut déjà être périmée.

Tests synthétiques dans le processus du moniteur : écriture 72 km/h → 20 m/s,
relecture, comparaison de tous les octets hors du double, rejet ID périmé,
descripteur modifié, présence absente, valeur invalide et hôte inconnu via le bouton.
La sélection du train est conservée par ID entre rafraîchissements. **6/6 tests
Debug et Release passent.** Aucun clic d'écriture sur un train réel n'a été
effectué par l'agent ; l'effet réel reste à observer lors du test utilisateur.

Les sections suivantes décrivent la phase initiale d'observation en lecture seule.

## Résultat et statut

Un moniteur externe expérimental (`src/train_monitor.cpp`) retrouve les cinq
trains de la partie ouverte et affiche leurs vitesses à 4 Hz. Il ouvre le
processus uniquement avec `PROCESS_QUERY_INFORMATION | PROCESS_VM_READ`.
Aucune écriture mémoire distante, aucun appel interne, aucune suspension de
thread et aucun hook de simulation. La DLL installée n'a pas été remplacée.

**Deux collections Motion ont été observées. Leur rôle respectif reste inconnu.**
Le moniteur les affiche en colonnes A/B. Une copie de rendu ou un état de
simulation intermédiaire sont des hypothèses, pas des conclusions. Les lectures
des deux colonnes ne sont pas simultanées. A/B désigne l'ordre de découverte,
pas une identité stable entre lancements.

La correspondance avec la fiche du jeu, le thread propriétaire, la cohérence
d'un instantané et la durée de vie des objets restent à valider. Les outils du
débogueur CLion exposent ici le lancement de configurations, sans rattachement
au jeu déjà lancé ; aucune preuve de pile ou de thread du jeu n'est revendiquée.
Ce moniteur n'est pas encore une API de production ni un affichage intégré au jeu.

## Binaire et méthode reproductible

- SHA-256 : `FFF49AC21720ABFC824C2B4F68B862727630EB0DB71CFE1F9EA8F685D0DB10AE`.
- PE AMD64 ; base préférée Ghidra `0x140000000` ; 18 914 fonctions automatiques.
- `tools/InspectTrains.java` exporte chaînes, références, RVA et décompilations
  dans `docs/research/reports/trains/`, depuis le projet de recherche existant.
- Ghidra exécuté avec `-process NimbyRails.exe -readOnly -noanalysis` ; aucune
  modification de sa base ou de l'exécutable. 934 chaînes candidates recensées.
- La décompilation de RVA `0x428790` a expiré ; les observations locales dans
  cette fonction proviennent du désassemblage, pas d'un pseudo-code réussi.
- Les fichiers `.c` du rapport sont du pseudo-code Ghidra, avec types supposés.
  Ils ne sont pas compilés dans le SDK.

Exemple depuis la racine du SDK (JAVA_HOME configuré pour Ghidra) :

```powershell
& 'C:/Users/samsv/Desktop/ghidra_12.1.3_PUBLIC/support/analyzeHeadless.bat' `
  'C:/Users/samsv/IdeaProjects/NimbyRailsResearch/projects' NimbyRailsResearch `
  -process NimbyRails.exe -readOnly -noanalysis -scriptPath "$PWD/tools" `
  -postScript InspectTrains.java "$PWD/docs/research/reports/trains" 14045f300 140804000
```

Les adresses en paramètres sont des VA d'analyse pour ce binaire. Les rapports
incluent leurs RVA. Ne pas ouvrir simultanément le projet dans Ghidra graphique.

## Preuves statiques

| Repère (RVA) | Observation | Limite |
| --- | --- | --- |
| `0x33F780` | Recherche Train : RCX descripteur, RDX identifiant, résultat RAX pointeur ou zéro ; tag supérieur de l'ID égal à 5 ; stride `0x178` ; comparaison de l'ID complet avant retour | Signature déduite des instructions Windows x64, jamais appelée par le SDK |
| `0x45F300` | Recherche Motion analogue, avec stride `0x638` | Même réserve ; pas de cible MinHook validée |
| `0x4A1710` / `0x4A44D0` | Parcours de la collection Train, étiquette `Train`, sérialisation ID à `+0`, nom à `+0x10`, code à `+0x30` | Noms reconnus également en mémoire ; ne pas confondre sérialisation avec boucle de simulation |
| `0x45ECE0` | Parcours et copie des Motion ayant le tag 5, stride `0x638` | Cette fonction alloue et copie : elle n'est pas utilisée comme observateur |
| `0x42DF99` / `0x42DFB4` | Enregistrement de `speed` avec quatrième argument `0x18`, après la description `Motion::Presence` | Indice de disposition croisé avec les lectures UI |
| `0x7F3600` | Étiquette `train_info_speed` : présence testée à `Motion+0x4B0`, vitesse lue comme double à `Motion+0x3C8`, puis multipliée par la constante à RVA `0xAABDF0` | Le maximum est lu ailleurs comme float ; il ne faut pas l'afficher comme vitesse instantanée |
| `0x804000` | Étiquette `info_train_speed` / `Speed: {} km/h`, mêmes lectures Motion et conversion | La fonction utilise un contexte dont `+0x258` et `+0x260` désignent les propriétaires monde/simulation |
| `0xAABDF0` | Huit octets du fichier décodés en double : **3,6** | Prouve la conversion employée par cette interface |

Le descripteur de collection utilise : décalage d'index à `+4`, nombre de slots
par bloc à `+8`, masque à `+0x10`, début/fin de la table de blocs à `+0x18/+0x20`.
L'index est extrait des bits 16 à 47 de l'ID ; les 16 bits supérieurs valent 5.
Le test d'égalité de l'ID complet est conservé. Le sens exact des 16 bits bas
n'est pas encore établi ; ils sont traités comme partie opaque de l'identifiant.

La sonde recherche des descripteurs compatibles dans les pages privées lisibles,
avec limites de taille, d'index et de durée. Elle exige le binaire reconnu,
contrôle les chaînes MSVC, les IDs de chaque slot et les vitesses finies, puis
relit le descripteur et la liste des blocs. Ces contrôles ne remplacent pas une
synchronisation avec le moteur : des changements concurrents restent possibles.
Une analyse complète des pages visitées ne prouve pas l'absence d'autres copies
dans d'autres catégories de mémoire. Aucun emplacement de tas n'est codé en dur.
La vérification négative contre le processus PowerShell local renvoie
`REFUSED: unknown executable`, code de sortie 4.

## Observations réelles

PID `73252`, session du 14 septembre ; adresses de tas valables uniquement pour
cette exécution, conservées comme preuves dans `reports/trains/live-samples.txt` :

| Identifiant complet | Nom |
| --- | --- |
| `0x5000000000003` | B81500 |
| `0x5000000010003` | B81501 |
| `0x5000000020002` | B81502 |
| `0x5000000030001` | TGV-M 900 |
| `0x5000000040002` | B81503 |

Une collection Train : 1 bloc, 2 048 slots, 5 entrées actives. Deux collections
Motion : chacune 1 bloc, 512 slots, les mêmes 5 identifiants. Environ 2 190 Mio de
pages visitées, sans atteindre la limite de 4 Gio lors de cette mesure.

Sur cinq échantillons espacés d'environ 500 ms, B81500 source A passe de
79,915459 à 82,450429 km/h ; B81501 reste à 0 ; B81503 source A passe de
90,925492 à 93,216523 km/h. Ce sont des lectures réelles, pas des simulations.
Elles prouvent l'évolution des données lues, pas encore leur synchronisation
exacte avec l'affichage du jeu. La recherche de paires de pointeurs du contexte
UI n'a pas permis d'identifier un propriétaire valide ; cette piste reste ouverte.

Fenêtre réelle ouverte sur cette partie : `MONITOR visible=1 rows=5 refresh_ms=250`.
Le processus du moniteur répond ; preuve de création dans `reports/trains/live-window.txt`.
Les empreintes de l'exécutable original et de la DLL SDK installée sont inchangées.

## Utilisation et tests

Après compilation : double-cliquer `build/Release/NimbyTrainMonitor.exe`, jeu
ouvert avec une partie chargée. Le programme détecte l'unique processus du jeu,
vérifie son empreinte, puis ouvre la fenêtre après la recherche des collections.
Il n'a pas besoin de la DLL du SDK pour ces lectures externes.

Modes de diagnostic :

```powershell
& build/Release/NimbyTrainMonitor.exe 73252 --sample | Tee-Object build/samples.txt
& build/Release/NimbyTrainMonitor.exe 73252 --window
```

Utiliser le PID courant, jamais recopier celui de ce rapport comme constante.
`--sample` produit cinq séries, puis quitte. Fermer la fenêtre arrête le moniteur.
Après changement de partie ou de structure de collection, relancer la détection.
Les lignes sont effacées si la lecture devient invalide ; `--` désigne une
présence absente ou un ID sans Motion, pas une vitesse nulle.

Debug et Release : **6/6 tests CTest réussis**. Le nouveau test utilise uniquement
de la mémoire synthétique dans son propre processus : rejet d'un index ID erroné,
d'un masque/taille incohérent, d'une vitesse non finie et d'un descripteur modifié ;
présence absente ; création d'une fenêtre cachée de test, affichage de 12,5 m/s
comme 45,0 km/h et suppression des anciennes lignes après invalidation.

## Vérifications avant intégration au SDK

1. Comparer A/B avec la fiche d'un train pendant accélération, arrêt et pause.
   Vérifier également les changements de vitesse de simulation.
2. Observer les appels et leurs threads pour identifier la source utilisée par
   l'UI et un instant où la collection est stable. Relever piles et registres.
3. Tester ajout/retrait de train, attelage, changement de partie et redémarrage ;
   vérifier le renouvellement des identifiants et l'absence de données périmées.
4. Valider un accès depuis le thread propriétaire et un point d'observation avec
   empreinte des instructions. Alors seulement autoriser un premier hook.
5. Exposer une API de copie d'instantané (ID opaque, nom, vitesse en m/s,
   disponibilité, temps de capture), sans pointeurs ni offsets dans l'API publique.

`src/runtime/runtime.cpp` reste en mode diagnostic : aucune promesse prématurée
d'énumération fiable n'est ajoutée à `include/nimby/sdk.h`.
