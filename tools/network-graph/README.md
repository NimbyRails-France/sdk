# Étoile lyonnaise — graphe technique

Ouvrir `index.html` dans un navigateur. Tous les fichiers du dossier doivent rester ensemble ; aucun serveur ni accès Internet n’est nécessaire.

Périmètre : Vaise–Perrache, Saint-Fons, Vénissieux (gare et triage), Saint-Priest, Collonges-au-Mont-d’Or, Sathonay–Rillieux, avec le centre lyonnais. Capture du 17 septembre 2026 : 1 028 signaux, 694 raccordements d’aiguille, 6 527 tronçons natifs. Les 2 056 recherches brutes ont été comparées au SDK sans différence. Ces contrôles portent sur la topologie, pas sur les permissions propres à un train.

La liaison Vaise–Perrache est vérifiée dans les deux sens sur les connexions natives, en respectant les OneWay et les NoWay sans exception. Le périmètre ouest a été élargi pour inclure les voies auparavant coupées. Les tronçons et signaux des chemins de contrôle sont consignés dans `branch-audit.json`, rubrique `vaisePerracheConnections`.

- Cliquer sur un signal de la carte colore en vert les chemins exportés vers les prochains signaux et les aiguilles traversées.
- Le chaînage recherche le prochain Path actif et respecte les sens uniques : un OneWay pris à revers coupe la branche ; dans son sens autorisé il est traversé sans arrêter la recherche. Les balises et les Path opposés sont traversés sans retourner le sens du déplacement. Un NoWay avec au moins une exception native est traversé ; sans exception, ou si le nombre est inconnu, il termine le parcours. Le graphe n’évalue pas la correspondance des exceptions avec un train particulier.
- Molette pour zoomer, glisser pour déplacer la carte. « Parcours technique » affiche les connexions sous forme de graphe.
- La vue commence à quatre signaux de profondeur. Les nœuds sont ordonnés selon leurs connexions pour réduire les croisements, les colonnes sont espacées et les retours ou liaisons longues passent au-dessus du graphe. Survoler un nœud met ses liaisons directes en évidence. « Tous les suivants » reste disponible ; les réseaux complexes peuvent conserver des croisements.
- « Chaînage → Tous les suivants » poursuit automatiquement de Path en Path sur toutes les branches retenues. Chaque signal orienté est exploré une seule fois pour terminer aussi en présence de boucles. Les choix de 1 à 8 signaux limitent la profondeur. Les sens uniques sont respectés à chaque étape ; les limites de capture sont signalées pour l’ensemble de la chaîne.
- Les IDs hexadécimaux et décimaux sont des chaînes pour préserver leur précision.

Les positions de dessin sont approximatives. Les chemins sont des possibilités topologiques, pas des itinéraires réservés. Un chemin représentatif est conservé par destination ; les branches sans prochain signal peuvent être incomplètes aux limites du périmètre. Les états sont figés à la capture.

`network.json` contient les données réutilisables ; `network-data.js` permet l’ouverture hors ligne.

Chaque chemin conserve les identifiants exacts des liaisons physiques (`edgeIds`) : deux voies reliant les mêmes nœuds ne sont pas surlignées ensemble par défaut. `branch-audit.json` détaille les branches retenues et exclues pour chaque signal. Les limites de capture restent signalées comme incomplètes.

Pour régénérer depuis une capture SDK :

```powershell
py tools/build_network_graph.py build/lyon-capture.json exports/lyon
node tests/network_graph_viewer.cjs
py tools/audit_network_branches.py
```

Extension ouest : Tassin, Charbonnières, La Tour-de-Salvagny et Lentilly-Charpenay. Depuis `0x8000061ea0001`, le prochain Path `0x8000039e80001` est maintenant atteint sans coupure de capture. Le rapport `westBranchConnection` conserve ce parcours ; les OneWay restent respectés.

## Corrections communes au réseau

La géométrie utilise `tools/network_geometry.py`, sans ID de gare, de ville ou de signal codé en dur. Les connexions restent issues du SDK. Les exemples lyonnais sont isolés dans `tests/network_graph_lyon_regressions.py` et activés uniquement avec `--lyon-regressions`.

Audit d’une autre capture : `py tools/audit_network_branches.py capture.json exports/autre-reseau`. Tests de géométrie indépendants de la carte : `py tests/network_geometry.py`. Les coordonnées restent une approximation graphique, pas les courbes natives du jeu.
