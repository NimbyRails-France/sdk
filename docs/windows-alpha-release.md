# Publication Windows alpha

Cette série est publiée sur la branche `alpha`, avec un tag `vX.Y.Z-alpha.N`
et une release GitHub marquée **prerelease**, jamais « latest stable ».
Les binaires sont construits et testés sur Windows. Aucun paquet Linux.
Le contrôle distant alpha du SDK/mod vérifie les métadonnées ; il ne remplace
pas les tests et le packaging Windows et ne publie pas de binaires.

Avant publication : vérifier les tests natifs Release/Debug, le plugin/kit Kotlin,
le mod, le client, le Hub, le TCO et le banc ; conserver le journal de build.
Les minima SDK des consommateurs doivent accepter la version alpha publiée.
Les manifestes contiennent le canal, la plateforme, l'URL du tag, la taille et
le SHA-256 du fichier final. Fournir aussi les noms de manifestes historiques
`project.json` / `hub-latest.json` pour les anciens Hub Windows.

Préparer d'abord les fichiers localement. Pousser les sources sur `alpha`,
créer le tag et une release brouillon, uploader et vérifier les assets ; rendre
la prerelease visible seulement quand elle est complète. Publier le SDK avant
ses consommateurs, puis le Hub. Ne pas déplacer un tag publié ou écraser ses assets.

Dans le Hub : canal Alpha pour le Hub ET pour chaque projet. Les versions locales
du profil Développer restent protégées ; choisir l'origine publiée pour la recette.
La validation automatique ne remplace pas la recette en jeu.
