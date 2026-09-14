# Textures des signaux — 2026-09-14

## Faits vérifiés

Exécutable NIMBY Rails 1.19.10.5bfaea3, AMD64, 12 583 424 octets.
SHA-256 : `FFF49AC21720ABFC824C2B4F68B862727630EB0DB71CFE1F9EA8F685D0DB10AE`.

Le fichier original `resources/signals/mod.txt`, sous le dossier Steam du jeu,
déclare un mod « Built-in signals », auteur Weird and Wry, schéma 1, version 1.0.0.
SHA-256 : `478FAD23BE99A40FB24C43DFCAD5576ACADDDF0E8D4B7CFEC2052A61AF2D9219`.

Les sections `SignalTextures` associent des identifiants à des fichiers SVG.
Les sections `SignalTemplate` référencent ces identifiants et un `kind`.

| Identifiant de textures | Ressources relatives | Type de template |
|---|---|---|
| waw_signal_idx_1 | textures/oneway.svg | oneway |
| waw_signal_idx_2 | textures/platstop.svg | platform_stop |
| waw_signal_idx_3 | textures/block_balise.svg | balise |
| waw_signal_idx_4 | textures/path_pass.svg, textures/path_stop.svg | path |
| waw_signal_idx_5 | textures/noway.svg | noway |
| waw_signal_idx_6 | textures/marker.svg | marker |

Le template path dispose de deux entrées `state`. Cela prouve l'existence de
ressources distinctes ; la règle native qui choisit l'état n'a pas été validée.
La numérotation des identifiants de textures n'est pas la valeur numérique du kind
dans les objets mémoire.

La recherche existante contient également ces références de chaînes :

| Chaîne | RVA chaîne | RVA référence | Début de fonction supposé |
|---|---:|---:|---:|
| icon_signal_balise.svg | 0xA9B360 | 0x734E2B | 0x7337C0 |
| icon_signal_path_flip.svg | 0xA9B340 | 0x734EDA | 0x7337C0 |

Source : `reports/network/strings.json`. Ces références sont des pistes d'icônes
d'interface, pas des cibles de hooks de rendu validées. Signature, convention
d'appel, thread de rendu et durée de vie des ressources restent inconnus.

## Ce que l'on peut conclure

Le format d'assets fournit une piste concrète pour personnaliser l'apparence via
un mod de textures. Un remplacement chargé en jeu n'a pas encore été testé.
Changer dynamiquement la texture d'un signal individuel, connaître son aspect
effectif et commander son comportement sont trois capacités encore non validées.
L'API publique reste en lecture seule ; aucun setter de texture n'est ajouté.

Le TCO Qt dessine ses propres symboles et peut changer leur style indépendamment
du jeu. Il affiche toujours l'aspect comme inconnu. Tous ses objets viennent de
l'API publique, sans nouvelles adresses internes.

## Prochaines validations

1. Valider le chargement d'un mod de textures séparé avec un identifiant propre,
   sans remplacer les ressources originales ; noter portée et rechargement.
2. Observer le choix pass/stop sur un même signal dans des situations reproductibles.
3. Identifier le lien template → ressource → instance et les threads concernés.
4. Valider les contrôles de version, durées de vie et retrait avant tout hook.
5. Exposer d'abord un aspect observé avec validité explicite ; ajouter une commande
   uniquement après preuve de sa sémantique et de son exécution sur le bon thread.

Aucun exécutable, asset original ou fichier de sauvegarde modifié pour cette étape.
