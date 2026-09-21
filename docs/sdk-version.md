# Versions et compatibilité

Cette branche prépare **SDK 0.7.3**. La version du dépôt ou d'un kit local ne
prouve pas qu'une release a été publiée.

## Chaîne Kotlin prise en charge

| Composant | Version ou cible |
| --- | --- |
| Kit SDK Kotlin | 0.7.3, format de manifeste 1 |
| Plugin Gradle `fr.nimbyrails.mod` | 0.7.3, distribué dans le kit |
| Kotlin/Native | 2.2.20 |
| Gradle Wrapper | 8.14.3 |
| JVM de développement vérifiée | JDK 21 |
| Cible native | Windows x64, `mingw_x64` |
| Contrat du loader | NRF Loader API 1 |
| Pont interne C++ | ABI 2 |

Utiliser ensemble les fichiers d'un même kit. Le plugin vérifie `sdk.json`
et refuse un kit incomplet, une cible différente ou un SDK hors de l'intervalle
déclaré par le mod. Mettre à jour Kotlin indépendamment du kit peut rendre
l'API `.klib` incompatible.

Le kit déclare les SHA-256 des exécutables du jeu pris en charge.
Le profil actuellement déclaré est
`fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae`.
Une autre version du jeu nécessite une adaptation et une validation du SDK.

## Versions des mods

`mod.json` déclare une borne SDK minimale incluse et une borne maximale
exclue. Les versions suivent `MAJEUR.MINEUR.CORRECTIF`, éventuellement
`-alpha.N` ou `-beta.N`. Le Hub utilise le même ordre pour ces versions.

Les identifiants Hub, du dossier de mod, du panneau et des réglages sont
persistants. Une modification peut casser les associations ou réglages des
utilisateurs même lorsque le code compile.

## Mise à niveau

1. Extraire le nouveau kit dans un dossier distinct.
2. Vérifier ses versions et contraintes.
3. Mettre à jour la version du plugin dans le projet et le chemin local du kit.
4. Recompiler et exécuter les tests.
5. Choisir le SDK d'exécution correspondant dans le profil développeur du Hub.
6. Redémarrer le jeu et vérifier les scénarios concernés avant publication.

## Clients C++

`nimby::getVersion()` expose la version du SDK et du pont interne sans ouvrir
le jeu. Les headers, bibliothèques d'import et DLL doivent provenir du même kit.
Les clients 0.6 doivent suivre la [migration 0.7](migration-0.7.md).
Le pont privé n'est pas une API consommateur et n'a pas de garantie indépendante
de compatibilité.