# Lecture des extensions de signal — recherche du 20 septembre 2026

**Changement de direction :** le mod BAL demande une interface directement
injectée par le SDK, sans NimbyScript. Cette recherche et ses sondes restent
expérimentales ; le lecteur réseau automatique n'appelle plus ce décodeur.
Le contrat C++ `Mod::signalSettings` décrit maintenant l'interface demandée,
dont le moteur de rendu et la persistance restent à implémenter. Les sections
ci-dessous décrivent la piste étudiée, pas un raccordement BAL actif.

But : rendre les champs des extensions natives accessibles au mod C++, en
conservant leur interface et leur persistance natives. Aucun contournement par
le numéro de texture, aucune logique BAL dans le lecteur SDK.

## Provenance

Analyse statique en lecture seule du binaire SHA-256
`fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae`.
Rapports dans `reports/extensions`, produits avec `tools/InspectExtensions.java`
depuis le projet Ghidra existant (`-readOnly -noanalysis`). Les fichiers `.c`
sont du pseudo-code décompilé, pas des sources compilées ou des traces en jeu.

## Chaîne identifiée

- L'éditeur de signal RVA `0x79f4d0` passe `Signal + 0x90` au panneau
  `ScriptStructInstancesTabs`, à l'appel `0x79fb8d`.
- Le panneau RVA `0x57c820` parcourt cette liste avec un pas de `0x50` octets.
  Il résout l'ID script à `instance + 0`, puis le type à `instance + 0x40`.
- La sérialisation RVA `0x2f29b0` confirme le pas `0x50` des instances.
- RVA `0x30f6a0` donne les vecteurs de valeurs à `+0x10/+0x18/+0x20`
  et de métadonnées à `+0x28/+0x30/+0x38`. Les deux ont un pas de 16 octets.
  Le sélecteur de variante des valeurs se trouve à `+8`. Le type sérialisé
  énumère `i64`, `f64`, `bool`, puis les autres alternatives.
- RVA `0x33f6a0` vérifie un ID script de tag 7, avec comparaison de l'ID entier,
  dans un pool de pas `0x150`. Le panneau utilise le pool `DB + 0x300`.
- Le panneau résout les définitions compilées via `DB + 0x1518`, lookup
  `0x2a2e80`, puis le type via `0x2a7900` sur le résultat `+0x70`.
- RVA `0x578690` associe aussi l'instance aux informations de champs persistées
  dans le script (`Script + 0x128`). Cette piste reste à décoder.

La passe suivante (`0x4232e0`, `0x438e10`) identifie l'entier d'instance `+8`
comme l'empreinte comparée à la définition compilée `+0x88`. Les champs compilés
forment un vecteur à `+0x70` de pas `0x60`, avec nom string à `+0`, identifiant à
`+0x20`. Le premier entier de chaque métadonnée reprend cet identifiant. Le
second vient de la description de type ; il reste opaque dans notre lecteur.

## Implémentation interne et limites

`engine/signal_extensions.h` lit uniquement les enregistrements natifs. Il
borne les listes, vérifie l'ID du signal, relit valeurs/métadonnées/descripteurs,
rejette les doublons script/type et efface le résultat en cas d'échec. Une liste
vide stable est distinguée d'une lecture impossible. Seule l'alternative bool
est décodée ; les autres restent opaques.

`engine/named_signal_extensions.h` résout les noms dans les définitions compilées,
rejette erreurs de compilation, script désactivé, ID recyclé, empreinte différente,
identifiants de champ incohérents et noms dupliqués. Le lecteur réseau l'appelle
avec l'adresse fraîche du signal, puis la capture vérifie les racines de session.
Le snapshot public expose `getSignalSettings(id,typeName)`. Les doubles lectures détectent certaines mutations, sans
garantir une transaction atomique du moteur ni exclure un changement aller-retour.

Le test `signal_extension_read_guards` couvre les lectures simulées : ID recyclé,
remplacement des données, des métadonnées, des instances et du propriétaire,
limites de taille, lecture impossible, bool mal typé et valeur bool invalide.
Il ne valide pas les offsets sur une extension réellement attachée en jeu.

Sonde en lecture seule `nimby_signal_extension_probe PID SIGNAL_ID`, construite
par CMake. Elle vérifie le SHA-256, les racines de session, le pool des signaux
et l'ID entier avant de lire les extensions. Sur le processus 10236, les signaux
`2251799905828866` et `2251799960813571` renvoient tous deux `extensions=0`, avec
code de sortie 0. Cela concorde avec le panneau vide, mais ne valide pas encore
une liste non vide ni le décodage des valeurs dans la partie.

## Étapes nécessaires pour les cases BAL

1. Vérifier la résolution des noms sur une extension réelle, et compléter la
   sélection par référence d'asset du script (actuellement type unique, doublons rejetés).
2. Attacher une extension de test dans le jeu, changer ses booléens et comparer
   les valeurs natives observées. Vérifier suppression et rechargement.
3. Valider l'API nommée et les états absent/disponible/indisponible de bout en bout.
4. Vérifier dans le jeu le manifeste et le raccordement des huit champs au profil BAL,
   maintenant implémentés. L'audit haut niveau en partie confirme seulement l'absence.

La déclaration d'interface seule ne constitue pas une fonctionnalité opérationnelle.
