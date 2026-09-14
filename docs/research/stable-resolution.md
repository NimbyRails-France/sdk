# Résolution après redémarrage — 14 septembre 2026

## Résultat

La base Train et l'état de simulation testé sont retrouvés depuis une variable
globale de l'exécutable, sans scan du tas ni choix arbitraire A/B. Une fermeture
et un nouveau lancement réels ont été observés ; les cinq trains et une écriture
ponctuelle sur B81500 ont été vérifiés dans la nouvelle instance.

Le résolveur réutilisable est dans `include/engine/live_state.h` et
`src/engine/live_state.cpp`. Il reçoit une fonction de lecture, une base de module
et la reconnaissance de version obtenue par identification SHA-256. Le moniteur
l'utilise par défaut. L'ancienne découverte exhaustive reste uniquement dans
le mode de recherche `--roots` ; elle n'autorise pas d'écriture interactive.

## Profil de version et preuves

SHA-256 exigé : `FFF49AC21720ABFC824C2B4F68B862727630EB0DB71CFE1F9EA8F685D0DB10AE`.

| Étape | Emplacement vérifié | Preuve |
| --- | --- | --- |
| Racine | pointeur à `module + RVA 0xB81998` | Découverte mémoire puis références Ghidra ; setter RVA `0x73AC80`, destruction RVA `0x9A9E00`, lectures dans `0x7272D0` |
| Base de données | pointeur à `racine + 0x540` | Collection Train connue en mémoire ; `0x7401F0` transmet `racine+0x540` à `0x347E20` |
| Copie | pointeur à `racine + 0x5C0` | Correspond à l'ancien A ; `0x347E20` lit le membre relatif `+0x80` et actualise cet état |
| Simulation | pointeur à `racine + 0x680` | Correspond à l'ancien B et reproduit son comportement après redémarrage ; destruction du membre relatif `+0x140` dans `0x73B1F0` |
| Collection Train | `base de données + 0x200` | Recherche Train RVA `0x33F780` et parcours déjà documentés |
| Collection Motion | `simulation + 0xA0` | Recherche Motion RVA `0x45F300` et accès UI déjà documentés |

Le setter global remplace la racine et détruit l'ancien objet : cela confirme
qu'il ne faut pas conserver cette adresse sans la revérifier. Les pointeurs de
collections sont relus à partir de la racine. Aucune fonction de ce tableau n'est
appelée par le SDK ou installée comme hook ; les signatures décompilées restent
des hypothèses, sauf les lectures d'instructions citées.

**Correction d'une piste antérieure : RVA `0xA6D430` n'est pas une table virtuelle.**
Ses premiers octets sont `0x80808080808080FF`, puis `0x8080808080808080` ; les
références montrent une sentinelle de conteneurs vides. Le contexte trouvé dans
le rapport précédent était une coïncidence de données et n'est pas utilisé.
Preuves négatives conservées dans `reports/anchor/`, preuves du vrai global dans
`reports/root/`. `tools/InspectAnchor.java` et `InspectTrains.java` ont été exécutés
avec Ghidra `-readOnly -noanalysis`.

## Vérification réelle avant/après

| Valeur | Avant : PID 73252 | Après : PID 64116 |
| --- | --- | --- |
| Base du module | `0x7FF79AA50000` | `0x7FF79AA50000` |
| Racine | `0x1CD805A7040` | `0x2827FD6A080` |
| Base de données | `0x1CDCA904300` | `0x2829DBF35B0` |
| Copie | `0x1CD80A40140` | `0x282A6789610` |
| Simulation | `0x1CDCA7A1BA0` | `0x282A64A2FC0` |

La base du module n'a pas changé sur ce redémarrage ; le test réel ne démontre
donc pas à lui seul un déplacement ASLR du module. Des tests synthétiques couvrent
explicitement le déplacement simultané de la base du module et des objets.
En revanche, **l'ordre des adresses Copie/Simulation s'est inversé réellement** :
choisir la deuxième collection par adresse aurait ciblé la copie après relance.

Les cinq IDs/noms ont été énumérés à nouveau : B81500, B81501, B81502, TGV-M 900,
B81503. Le rapport indique `scanned=0 MiB`. Les IDs identiques résultent du
rechargement de cette partie ; leur stabilité entre parties n'est pas garantie.

Une écriture unique à 5 km/h dans la simulation retrouvée, sur l'ID
`0x5000000000003`, a donné : avant **92,807203 km/h**, relecture **5 km/h** ;
à 62 ms, simulation **5,142560** et copie **5,095040 km/h**. La suite augmente
progressivement. Traces brutes : `reports/root/before-restart.txt`,
`after-restart.txt`, `write-after-restart.txt`.

## Contrôles et limites

- Refus du profil si le fichier exécutable n'a pas l'empreinte reconnue.
- Base chargée obtenue par les modules du processus ; aucune VA de tas persistée.
- Deux lectures de la racine et de ses membres doivent concorder ; rejet des
  pointeurs nuls, non alignés, hors plage ou de deux états confondus.
- Identité de la racine et des membres revérifiée à la lecture des collections
  et juste avant l'écriture. Les contrôles existants sur ID complet, descripteur,
  bloc, présence et page accessible restent actifs.
- L'écriture par défaut n'est autorisée que dans la collection `Simulation`.
  « Copie (lecture) » ne peut plus être modifiée via le moniteur.
- Nouveau PID : ouvrir le moniteur, qui redétecte automatiquement l'unique jeu.
  Un moniteur attaché à un ancien processus ne transfère jamais ses adresses au
  nouveau. Après invalidation ou changement de partie, relancer le moniteur.
- Les relectures ne constituent pas un verrou moteur : une course reste possible
  après le dernier contrôle. Le thread propriétaire et un accès synchronisé
  doivent encore être identifiés avant de qualifier l'écriture de sûre pour les mods.

L'intégration actuelle est un adaptateur interne réutilisable et un moniteur
expérimental. Aucune nouvelle API publique d'écriture ni aucun hook de simulation
n'est activé dans la DLL installée.

## Validation du code

**7/7 tests CTest passent en Debug et Release.** Le nouveau test couvre la
relocalisation, le refus d'une version inconnue sans lecture mémoire, une racine
remplacée entre captures, une racine nulle et des états confondus. Le test du
moniteur vérifie aussi le refus d'écriture dans la copie, l'écriture dans l'état
de simulation d'une fixture locale, puis le refus après disparition de sa racine.
Ces fixtures sont synthétiques et distinctes du redémarrage réel documenté ci-dessus.
