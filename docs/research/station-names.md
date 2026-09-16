# Noms automatiques des gares — SDK 0.6.2

## Source native

Binaire reconnu : SHA-256
`fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae`.
Analyse Ghidra en lecture seule, sans modifier le jeu ni appeler ses fonctions.

- RVA `0x61ACC0` (étiquettes des gares) sélectionne la chaîne manuelle à
  `Station+0x20` si `Station+0x40 == 0`, sinon la chaîne du cache à `record+8`.
- RVA `0x790C40` et `0x755EA0` font la même sélection dans l'éditeur de gare.
- RVA `0x6236D0` cherche le modèle avec `0x32C3B0`, puis le cache avec
  `0x3CA140`. La table de cache est à `DB+0x430` dans la partie observée.
- RVA `0x3CA140` valide le tag 2 et l'ID complet, calcule bloc et slot, puis
  utilise un pas de `0xF8`. Le cache utilise le même format de pool que les
  autres collections. Sa chaîne MSVC commence à `+8`, longueur `+0x18`,
  capacité `+0x20`, seuil de stockage interne 15 octets.

Exports reproductibles via `tools/InspectTrains.java` : fonctions ci-dessus,
rapports locaux dans `reports/station-names/`. Aucun pointeur de tas observé
n'est codé en dur. Le SDK repart du DB validé par `resolve_live_state`.

## Preuve sur la partie ouverte

Lecture externe, PID 31988, le 16 septembre 2026 : les **3 342 gares** rejoignent
un enregistrement de cache avec le même ID complet, et les 3 342 chaînes sont
non vides. La table observée comporte deux blocs de 4 096 slots.

La voie signalée par l'utilisateur, `281490654363649`, référence la gare
`562950016598017`. Son nom manuel exposé était vide ; son nom automatique est
**Tours**. D'autres entrées automatiques incluent Vandières, Narbonne, Roanne,
Saint-Chamond, Beauvais et Compiègne.

Les noms proviennent du cache du jeu : ils peuvent donc être des noms de lieux
ou de points d'intérêt, pas nécessairement les noms ferroviaires officiels.

## Contrat et protections

Le lecteur conserve les noms manuels sélectionnés. Il résout uniquement les
gares en mode automatique, avec jointure par l'ID complet et sa génération.
La table et ses pointeurs de blocs sont relus ; l'ID et les métadonnées de la
chaîne sont revérifiés, et les chaînes externes sont lues deux fois.
Les tailles sont bornées (256 octets de texte), le terminateur est obligatoire,
et une chaîne contenant un NUL interne est refusée.

Un cache absent, malformé ou modifié reste facultatif : les noms concernés
restent indisponibles, mais le réseau reste lisible. Aucun nom approchant ni
nom d'une autre génération n'est substitué. ABI 1 et `NimbyStation` inchangés.

Les tests synthétiques couvrent les noms internes et externes UTF-8, la priorité
manuelle, les générations différentes, les lectures échouées, les remplacements
concurrents, les tailles excessives et les chaînes non terminées.

Validation finale : 13/13 tests CTest réussis ; l'API publique a copié
3 342/3 342 noms non vides sur la partie réelle. Les trois consommateurs
Debug/Release ont été liés au paquet `install/Release` 0.6.2 ; l'exemple C++
a également été exécuté depuis CLion avec succès.

## Audit exhaustif des correspondances

Contrôle supplémentaire sur le PID 31988 : un lecteur indépendant parcourt
les modèles et le cache natifs, choisit le nom selon `Station+0x40`, puis
compare les octets UTF-8 aux résultats de `NimbySdk_CopyStations` du paquet
`install/Release`. Les noms natifs et leur mode sont identiques avant et
après la capture publique ; les identifiants complets sont uniques et les
ensembles de gares ont la même taille.

- 3 342 gares comparées : 2 051 automatiques et 1 291 manuelles.
- 3 342 correspondances exactes, aucun écart et aucun nom vide.
- 248 418 voies comparées au modèle natif (`Track+0xD0`), dont 23 190
  références de gare : aucune différence ni référence non résolue.
- Exemples : Tours (automatique), Roquesérière - Buzet, Toulouse Matabiau
  SNCF et Bordeaux Saint-Jean (manuels).

Outil local : `build/station-name-audit.cpp`, résultat dans
`build/station-name-audit.txt`. Il utilise l'API publique installée pour les
données SDK, sans utiliser `read_network` pour calculer les noms attendus.
Ce contrôle valide les correspondances dans la partie observée, pas les
appellations ferroviaires officielles ni une inspection visuelle de chaque gare.
