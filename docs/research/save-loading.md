# Grandes sauvegardes et résolution des objets — 2026-09-14

Binaire reconnu : SHA-256 FFF49AC21720ABFC824C2B4F68B862727630EB0DB71CFE1F9EA8F685D0DB10AE.

## Échec reproduit

Le test public `nimby_observation_tests.exe 69952` échouait à CaptureSnapshot.
Le débogueur CLion a arrêté uniquement le programme de test, pas le jeu.
Pile : main (tests/observation.cpp) → CaptureSnapshot → read_network → collect.
La lecture des trains et Motion avait déjà réussi.

Dans le pool Track : shift=9, size=512, begin=2205032861872,
end=2205032865760, cap=2205032865968. Soit 486 blocs et 248832 emplacements.
Le contrôle `blocks.size()*size > 100000` refusait cette capacité valide.
Ce n'était donc pas une adresse de sauvegarde manquante : la racine avait été
retrouvée, mais une limite arbitraire empêchait la lecture du réseau plus grand.

## Correction

La capacité maximale est désormais bornée à 1048576 emplacements. Les lectures
restent effectuées bloc par bloc. Les contrôles de pointeurs, tailles, tags,
indices, générations et stabilité des en-têtes restent actifs. Les références
entre voies, gares, signaux et trains sont vérifiées par ensembles d'IDs complets
pour éviter les parcours quadratiques sur de grands réseaux.

Après correction, le test public a lu : **924 trains, 248418 voies, 3342 gares,
26003 signaux**, avec validation des buffers et de la durée de vie des snapshots.
Le test synthétique couvre une capacité de 486×512 et le rejet au-delà du plafond.
Il simule des emplacements vides ; ce n'est pas une sauvegarde réelle.

Validation Release : 10/10 tests réussis, paquet installé et consommateur externe
recompilé. Ce consommateur a lu cinq snapshots de la grande sauvegarde. Le TCO
recompilé a également lu 924/248418/26003 objets (trains/voies/signaux) sous GDB,
puis terminé normalement avec `--smoke`, sans avertissement QML observé.

## Lien avec le cycle de vie d'une partie

Le module EXE identifie la version ; son RVA racine est indépendant du fichier
de sauvegarde. Les pointeurs DB/copie/simulation et les collections appartiennent
à la partie chargée. Le SDK les résout de nouveau à chaque capture et compare
les racines avant/après. Il ne doit jamais conserver des pointeurs d'objet.
Un snapshot public reste une copie historique valide après un changement de partie.
Un ID seul ne prouve pas l'identité d'un objet entre deux sauvegardes : ne pas
réutiliser des sélections ou commandes d'une partie sur une autre.

Le nouveau PID 69952 indique également un redémarrage de processus par rapport au
PID 53392 précédent. Le TCO en mode automatique recherche le nouveau processus ;
un PID explicitement saisi doit être mis à jour. Les captures restent non atomiques :
pendant un chargement, un refus temporaire nécessite une nouvelle tentative.
Cette correction ne prouve pas la détection de tous les changements de sauvegarde
dans un même processus, notamment si des racines et IDs sont réutilisés.

Le TCO applique maintenant les filtres dans le worker et transmet au plus 500
segments par page à QML, avec compteurs globaux et navigation. Un seul résultat
peut attendre l'interface, afin de borner la file de mises à jour. La fréquence
réelle dépend du temps de capture et de construction des données, pas d'un tick garanti.
