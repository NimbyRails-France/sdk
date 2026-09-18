# Exceptions des signaux — 17 septembre 2026

Profil SHA-256 : `FFF49AC21720ABFC824C2B4F68B862727630EB0DB71CFE1F9EA8F685D0DB10AE`.

Le panneau natif de filtre `RVA 0x79f4d0` copie la liste d’exceptions depuis
`Signal +0x78` (début), `+0x80` (fin), `+0x88` (capacité), par éléments de
8 octets. Le callback `RVA 0x7a0a40` affiche cette liste sous « Except: »
et « No exceptions ». Il lit le mode par défaut à `+0x70` : 0 applique le
signal, 1 l’ignore. L’ordre des deux options est établi par leur construction
dans `0x79f4d0`. Script reproductible : `tools/InspectSignalFilters.java`.

Les tags sont des clés scalaires de 64 bits, pas des IDs d’objets portant une
génération : les NoWay `0x8000004c60002` et `0x800005e2a0001` contiennent
la clé 10115, mode 0. `0x8000045290003` possède une liste vide, mode 0.
Lecture réelle en mémoire et recapture SDK : 25 971 filtres disponibles.
Parmi 545 NoWay : 213 sans exception, 298 avec une exception, 34 avec deux.

La lecture est bornée à 4 096 éléments. Elle vérifie les bornes du vecteur,
relit son contenu, son en-tête et l’ID du signal. Une lecture invalide ou
instable rend seulement le filtre indisponible, sans inventer une liste vide.

`SignalState::getExceptionCount()` et `isIgnoredByDefault()` renvoient des
optionnels. Le pont interne utilise deux nouveaux bits de validité et l’ancien
champ réservé de `NimbySignalState`, sans changer sa taille ni celle de
`NimbySignal`. Les anciens producteurs donnent une valeur inconnue.

La règle du graphe demandée par l’utilisateur autorise le prolongement d’un
NoWay si sa liste d’exceptions n’est pas vide. Elle ne teste pas si un train
particulier correspond à ces tags. Sans exception, ou si l’information est
inconnue, la recherche s’arrête au NoWay. Le mode par défaut est exporté
séparément et ne remplace pas cette règle de visualisation.
