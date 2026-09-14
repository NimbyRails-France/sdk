# Analyse native de NIMBY Rails

Projet Ghidra pour examiner la transmission des limites de vitesse NimbyScript
au moteur de simulation. Aucune DLL ni aucun hook n'est installé dans le jeu.

## Ouvrir le projet

1. Lancer `C:/Users/samsv/Desktop/ghidra_12.1.3_PUBLIC/ghidraRun.bat`.
2. Choisir **File > Open Project**.
3. Ouvrir `projects/NimbyRailsResearch.gpr` dans ce dossier.
4. Double-cliquer sur `NimbyRails.exe` pour ouvrir le CodeBrowser.

Ne pas ouvrir le projet pendant son analyse headless : Ghidra verrouille le
projet pour éviter des écritures concurrentes.

L'import initial et l'analyse automatique se sont terminés avec succès le
13 septembre 2026 (331 secondes d'analyse). Le projet est maintenant disponible.
Ghidra a identifié 18 914 fonctions et l'inventaire contient 41 chaînes
candidates. Le SHA-256 de l'original et de la copie a été vérifié après analyse.

Des avertissements d'analyse subsistent : PDB absent, certaines références
impossibles à lire, données PNG/RTTI non reconnues. Le succès de l'import ne
signifie pas que toutes les fonctions ont été correctement reconstituées.

## Provenance

- Ghidra : 12.1.3 PUBLIC, avec le JDK local 25.0.4.1.
- Exécutable original :
  `C:/Program Files (x86)/Steam/steamapps/common/NIMBY Rails/NimbyRails.exe`.
- Copie analysée : `inputs/NimbyRails.exe`.
- Taille : 12 583 424 octets ; architecture PE x64.
- SHA-256 : `FFF49AC21720ABFC824C2B4F68B862727630EB0DB71CFE1F9EA8F685D0DB10AE`.
- Les adresses de l'analyse sont propres à ce binaire et à la base d'image
  choisie par Ghidra. Les adresses chargées dans le jeu peuvent différer.

## Fichiers

- `projects/` : base d'analyse Ghidra.
- `reports/analysis.log` : journal de l'import et de l'analyse automatique.
- `reports/scripts.log` : journal des scripts Ghidra.
- `reports/initial-inventory.json` : chaînes candidates et références repérées
  par `scripts/InspectNimby.java`, avec architecture et nombre de fonctions.
- `reports/executable-before.json` : empreinte de l'exécutable avant analyse.
- `reports/executable-verification.json` : vérification après analyse.

L'inventaire est une liste de pistes : une chaîne `max_speed` peut appartenir
à l'enregistrement de l'API, aux données des trains ou à une autre fonction.
La présence d'une référence ne prouve pas qu'elle commande le freinage.
Les noms `FUN_...` sont des noms automatiques, pas des noms du code source.

Premiers repères, utilisables avec **G** dans le CodeBrowser :

| Chaîne | Adresse de la chaîne | Fonction avec une référence candidate |
| --- | --- | --- |
| `event_signal_lookahead` | `140a71c90` | `FUN_140424c00` |
| `SignalLookaheadResult` | `140a72390` | `FUN_140428790` |
| `max_speed` | `140a703e0` | plusieurs, voir le JSON |

Ces références doivent être examinées : elles peuvent décrire l'API sans
constituer le point où le moteur applique effectivement la limite.

## Investigation à poursuivre

Le test NimbyScript demande directement 15 km/h. L'utilisateur constate un
respect de la limite avec un train seul, puis des franchissements rapides avec
cinq trains. La cause reste inconnue : les tests JavaScript du mod ne simulent
pas le moteur natif ni son exécution multithread.

Identifier le traitement de `SignalLookaheadResult.max_speed`, puis suivre
son utilisation dans le moteur avec un débogueur, en comparant les deux cas.
L'analyse statique seule ne permet pas de conclure à un écrasement de mémoire.

Le dépôt `../SignalisationSNCF` reste la source du mod actuel. Ce dossier
de recherche n'est pas copié dans les mods locaux ni dans le Workshop.
