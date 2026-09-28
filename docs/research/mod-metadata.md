# Métadonnées et catalogues traduits — Windows 1.19

Profil : `fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae`.
L'analyse statique est reproductible avec `tools/InspectModMetadata.java`, puis
`InspectConstruction.java`. Elle ne constitue pas une qualification en jeu.

Le lecteur `0x2df0c0` reçoit un emplacement de retour optional, le type de
source et un `filesystem::path` MSVC (wstring). Il renvoie un ModMeta de
`0x110` octets ; l'octet suivant indique la présence. Le parseur `0x2dd350`
remplit les chaînes `name +0x28`, `author +0x48`, `desc +0x68`, `version +0x88`.
Il convertit `<br>` en saut de ligne dans la description. Le ModId est composé
du type à +0 et de la chaîne à +8. Les noms ne sont jamais une identité.

`0x2e0b70` fournit une copie optional du cache ; `0x2e0c80` construit une
collection de copies. Le sélecteur de mods de nouvelle partie `0x5dfe40`
les appelle aux sites de retour `0x5e00f5` et `0x5dfe84`. La passerelle limite
la traduction de ces copies à ces deux sites. Les fonctions sont aussi utilisées par la
publication Steam, les ressources et les sauvegardes : les détourner sans
filtrer les appelants changerait le contrat au-delà du texte affiché.

La liste appelle `0x2e0c80` avec le filtre de ressources actif. Le prédicat
`0x2dd820` vérifie les types présents dans ModMeta à +0xe8. Un paquet ne
contenant que ModMeta, notamment un outil pur, n'est pas rendu éligible par
la traduction. Son chargement NRF Loader reste indépendant du sélecteur.

La découverte conserve le cache natif en langue de repli et charge seulement
`nrf-metadata.json` dans le dossier déjà découvert par le jeu. Taille bornée,
JSON strict, langues normalisées, référence partagée immuable et identité
source/id assurent l'isolation. Une redécouverte supprime d'abord l'ancienne
entrée, même si le nouveau fichier est absent ou invalide. Le repli doit
correspondre exactement aux valeurs lues depuis mod.txt.

La traduction du sélecteur ne touche que les chaînes de ses copies.
Les fonctions natives `0x2c00` (construction depuis bytes/length), `0x25630`
(affectation par déplacement) et `0x2d30` (destruction) gardent toutes les
allocations dans le runtime du jeu. Deux temporaires sont construits avant
de remplacer les champs. Une std::string MinGW n'est jamais transmise comme
objet MSVC. L'empreinte du binaire et les neuf prologues sont vérifiés avant les six hooks.

## Gestionnaire pendant une partie

Le gestionnaire `0x669130` peut copier les métadonnées depuis la sauvegarde.
Cette collection sert aussi aux changements de mods : elle ne doit donc
jamais être traduite en place.

Le hook du rendu de fiche `0x668ca0` construit une vue temporaire : mêmes
identité, auteur, version et pointeurs de ressources ; seulement le nom et la
description possèdent deux nouvelles chaînes natives. Le rendu est synchrone,
en lecture seule. La vue est détruite après cet appel, pas la collection ni
les pointeurs empruntés. Une erreur de préparation utilise la fiche originale.
L'appel natif n'est jamais rejoué après une exception du rendu.

Le wrapper de libellé `0x55bc50` couvre les parcours de mesure et d'affichage.
Il ne traduit que deux appelants reconnus :

- Retour `0x66a56a` : nom du mod dans la liste, chaîne à ModMeta+0x28.
- Retour `0x66ad59` : ressource du mod sélectionné. Le RuleInfo du nœud possède
  le type à +0x20, l'ID à +0x28, `name_en` à +0x48. Le type 10 est SignalTextures.
  La traduction exige la correspondance de l'ID et du nom natif original avec
  le catalogue de la fiche sélectionnée. Les autres types restent inchangés.

## Noms des catalogues de construction

Le format 1 du catalogue décrit le nom et la description. Le format 2 ajoute
au maximum 32 ressources, chacune avec `kind`, `id`, `key`, `default` et
`languages`. `kind` vaut `template` ou `textures`. Le plugin génère les clés
`nrf.sdk.` suivies du SHA-256 de l'identité mod/modèle/rôle. Aucun nom traduit
ne devient un ID. `default` correspond au `name_en` écrit dans mod.txt, anglais
ou repli du JSON. Les couples type/id et les clés doivent être uniques ; une
langue non déclarée au niveau du catalogue est refusée. Taille totale : 1 Mio.

SignalTemplate et SignalTextures lisent `name_en` et `name_loc` (parseurs
`0x4113e0` et `0x410d60`). Le plugin écrit automatiquement les deux champs
pour les noms déclarés avec `tr`. Les clés natives explicites restent possibles
pour un nom direct ; les mélanger avec `tr` pour ce nom est refusé.

Le hook de localisation native `0x2d82e0` accepte seulement une clé générée
et son texte original exact. Une ambiguïté entre catalogues est rejetée.
La langue est celle du jeu, puis langue de base, puis repli. Le moteur peut
emprunter le pointeur retourné : les traductions sont internées jusqu'à l'arrêt
du processus, même après invalidation du catalogue. Ce pool est limité à
32 Mio ; au-delà, le texte natif reste utilisé. La recherche des clés du jeu
et des autres mods n'est pas modifiée.

## Chargement et validation

Le proxy charge la passerelle après SDL_Init, hors DllMain, avant le chargement
d'une partie. La DLL est résidente et épinglée ; aucun déchargement à chaud
de ses trampolines n'est annoncé. L'installateur suit les empreintes des DLL
supplémentaires. L'absence de la passerelle laisse le mod.txt de repli lisible.

Validation autonome : génération Kotlin/Gradle, fallbacks et alias de langues,
descriptions multilignes, isolation des IDs, retrait d'un catalogue périmé,
compilation Windows et gestion des chaînes natives avec un allocateur fixture.
Les tests couvrent aussi les clés générées distinctes et stables, les conflits,
la durée de vie des pointeurs, les vues de rendu FR/EN et l'intégrité octet par
octet des données natives originales. Les fenêtres d'outils ont un test de
changement de langue conservant la saisie et la demande en cours.

Contrôle en jeu du 28 septembre 2026 (1.19.10.5bfaea3, empreinte ci-dessus) :
passerelle compilée chargée par le loader de diagnostic, menu uniquement.
Un catalogue temporaire correspondant au mod.txt SFR installé a fourni une
traduction anglaise distincte et une description sur deux lignes. Après
passage de Français à English dans les options puis réouverture de Nouvelle
partie > Mods, le nom de liste, le nom sélectionné et la description étaient
traduits. Auteur, version et source restaient identiques. Aucune sauvegarde
n'a été chargée ou créée et aucun mod.txt n'a été modifié. Le catalogue de
test a ensuite été retiré.

Le démarrage de la passerelle via le proxy de production a depuis été observé
dans les journaux ; cela ne prouve pas tous les parcours d'affichage.

Restent à qualifier en jeu : gestionnaire pendant une partie, contenu des
ressources, menus de construction, changement de langue sans réouverture,
repli et invalidation visibles, interface Steam inchangée. Les tests de rendu
isolés ne remplacent pas cette vérification visuelle.
