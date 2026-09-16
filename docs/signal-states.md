> Archive technique antérieure à la 0.7 : noms et API C historiques.
> Pour la version actuelle, utiliser la [référence C++](cpp-api-reference.md) et la [migration](migration-0.7.md).

# États des signaux

`NimbySdk_CopySignalStates` complète `NimbySdk_CopySignals` sans modifier
`NimbySignal` ni l'ABI v1 existante. Utiliser le header et la DLL reconstruits
ensemble : les anciennes DLL ne contiennent pas ce nouvel export.

Une capture contient une ligne `NimbySignalState` par signal. Relier les lignes
par `signal_id` dans le même snapshot. La fonction suit le protocole habituel
buffer/count, sans copie partielle ; les données restent immuables jusqu'à
`NimbySdk_ReleaseSnapshot`.

| Champ | Utilisation |
|---|---|
| `aspect` | État général : inconnu, arrêt, prudence, passage, éteint |
| `system_utf8` / `specific_state_utf8` | Identifiant spécifique à un système, par exemple `fr` / `carre` |
| `texture_state` | Sélecteur natif avant limitation au nombre de textures et repli du rendu |
| `flags` | Disponibilité indépendante des trois informations ci-dessus |

Tester respectivement `NIMBY_SIGNAL_ASPECT_VALID`,
`NIMBY_SIGNAL_SPECIFIC_STATE_VALID` et `NIMBY_SIGNAL_TEXTURE_STATE_VALID`.
Un index de texture zéro peut être valide ; le bit de validité fait foi.
Les chaînes sont terminées par zéro, avec au maximum 31 et 63 octets UTF-8
utiles. `reserved` reste nul. Un état spécifique peut exister sans équivalent
général ; les consommateurs doivent accepter les identifiants inconnus.

## Disponibilité actuelle

L'adaptateur lit la table de sélecteurs consultée par le rendu natif :
`Sim + 0x2200 -> Query + 0x378`. Chaque entrée associe un ID complet à un
entier signé. Il expose `TEXTURE_STATE_VALID | SPECIFIC_STATE_VALID` lorsque
la table est stable ; un ID absent utilise le zero par defaut du rendu.
L'identifiant specifique a la forme
`nimby:<hash textures sur 16 chiffres hexadécimaux>:kind.<type>.state.<valeur>`.
Un hash nul désigne les textures par défaut du type ; ce n'est pas un pointeur.

Le sélecteur est observé, mais sa signification ferroviaire dépend des textures
et des scripts. **L'aspect général reste UNKNOWN**. Le rendu peut limiter le
sélecteur au nombre de textures ou utiliser des textures de repli ; le SDK
ne prétend pas exposer l'image finale affichée.

Depuis 0.7.1, une entree absente d'une table lue et valide utilise le selecteur
zero, comme le rendu natif, avec `NIMBY_SIGNAL_TEXTURE_STATE_DEFAULT`.
En C++, `usesDefaultTextureSelector()` distingue ce cas d'un zero explicite.
Une table illisible ou instable laisse `flags=0` et les
chaînes vides. `NIMBY_OK` signifie que les lignes ont été copiées, pas que tous
les états sont connus. Aucun état n'est déduit des réservations/occupations.
Voir les [preuves de lecture native](research/signal-states.md).

## Préparer la signalisation française

Les identifiants spécifiques permettent de conserver des distinctions telles
que `fr:carre`, `fr:semaphore` et `fr:avertissement` dans une future extension.
Ce sont des exemples de nommage, pas une implémentation des règles françaises.
Cette étape observe les sélecteurs natifs ; elle ne calcule ni ne commande les
aspects français. Une correspondance vérifiée entre textures, scripts et noms
français reste nécessaire avant de renvoyer ces noms ou un aspect général.

L'exemple `observer` montre les trois informations et leur indisponibilité.
Le test `signal_state_contract` vérifie des snapshots synthétiques contenant
des états inconnus et spécifiques, les buffers et l'immuabilité.
Le TCO affiche les textures natives dans la liste et sur la carte au zoom
rapproché, avec le sélecteur comme repli si le fichier ne peut pas être chargé.
Il les invalide après 1,5 seconde sans capture fraîche.

## Texture sélectionnée

`NimbySdk_CopySignalTextures` renvoie une ligne `NimbySignalTexture` par signal,
à joindre par `signal_id` dans le même snapshot. Le contrat buffer/count et
l'immuabilité sont identiques à ceux des états. Reconstruire le consommateur
avec le nouveau header et la nouvelle DLL.

La table chargée par le jeu fournit `textures_hash`, `textures_id_utf8`,
`state_count`, `selected_index`, `file_hash`, `mod_id_utf8` et
`relative_path_utf8`. Le sélecteur est limité à `[0, state_count - 1]` ;
`texture_state` conserve sa valeur native originale.

| Bit de `flags` | Signification |
|---|---|
| `NIMBY_SIGNAL_TEXTURE_REFERENCE_VALID` | Entrée du catalogue résolue |
| `NIMBY_SIGNAL_TEXTURE_FILE_VALID` | `file_path_utf8` désigne un fichier local existant |
| `NIMBY_SIGNAL_TEXTURE_DEFAULT_SET` | Jeu de textures par défaut du type utilisé |
| `NIMBY_SIGNAL_TEXTURE_CLAMPED` | Sélecteur limité à la taille du jeu de textures |

`source` vaut 0 pour les ressources intégrées, 1 pour un mod local, 2 pour
Steam Workshop. Les chemins sont UTF-8, terminés par zéro ; un fichier absent
laisse la référence disponible, sans `FILE_VALID`. Une table illisible ou un
selecteur indisponible donne `flags=0`. L'absence d'ID dans une table valide
utilise la texture zero du jeu de textures selectionne. Le SDK ne reproduit pas un eventuel repli
du moteur après une erreur de chargement GPU et n'infère aucun nom d'aspect
ferroviaire depuis la couleur du fichier.

Voir la [table native et sa validation](research/signal-texture-catalog.md).
