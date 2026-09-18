# Raccordements de branches — 16 septembre 2026

Profil expérimental du binaire reconnu par le SDK. Lecture seule, sans appel
aux fonctions du jeu ni écriture dans la simulation.

## Preuves

Au débogueur CLion/GDB, `NimbyTrainMonitor 13248 --sample`, arrêt conditionnel
dans `src/engine/network.cpp` après lecture des liens de la voie
`0x1000000d50001`. Pile : lambda de `read_network`, `collect`, `read_network`,
`run` (`train_monitor.cpp:687`), `WinMain`.

- Liens primaires : A = 0, B = `0x1000000d40001`.
- Track + `0x3f0` : ID complet `0x1000000820001` (voie principale).
- Track + `0x3f8` : double `0.7324387267046919` (fraction sur la principale).
- Track + `0x400` : int32 `-1` (sens sur la principale pour entrer dans la branche).

La session de diagnostic a été arrêtée et son point d'arrêt supprimé ; le jeu
n'était pas le processus débogué et a continué à tourner.

Analyse statique Ghidra, VA relatives à la base `0x140000000` :

- RVA `0x39d9a0` / `0x39d2e0` écrivent l'ID principal et la fraction,
  puis ajoutent l'ID de branche à la liste de la principale située à `+0x408`.
- RVA `0x37cf30` parcourt cette liste, compare le sens à `branch+0x400`,
  et ne prend que les fractions encore devant la position courante.
- RVA `0x379d10` rejoint la principale quand le lien primaire dans le sens
  de parcours est nul ; sa position de sortie utilise la fraction `+0x3f8`
  et le sens opposé à `+0x400`. La variante autorisant les retournements
  est volontairement exclue du SDK.
- RVA `0x310850` sérialise ces champs et la liste inverse.

`tools/InspectJunctions.java` exporte les fonctions candidates utilisant cet
offset. Les rapports locaux sont dans `build/junction-native/`.

## Gardes et validation

Le lecteur exige exactement une extrémité primaire ouverte, une fraction finie
dans [0,1], un sens ±1, un parent existant et distinct, et exactement une référence
inverse avec l'ID complet de branche. Il relit la liste, les identités, les liens
et les descripteurs du parent et de la branche pour rejeter les données instables.
Les captures restent non atomiques, comme le reste du SDK.

Validation réelle sur Professional Freight : 29 signaux dont les trois Path
posés pour l'essai, 20 raccordements validés. Le signal `0x8000000140003`, parcouru
en sens -1, donne deux prochains signaux : `0x8000000190002` (voie directe)
et `0x80000001c0001` (branche). Le parcours inverse de la branche retrouve
le signal d'entrée. Contrôles exécutables dans `build/junction-verify.cpp`,
résultat `build/junction-verify.log`, code de sortie 0.

Tests de régression : sens opposés, raccordement au milieu d'une voie, aiguille
derrière la position, plusieurs branches au même point, génération d'ID périmée,
fraction NaN, mauvais sens, mauvaise liste inverse, lecture impossible et limites
de parcours. Ne pas interpréter ces alternatives comme un état d'aiguille ou une
autorisation de passage.
