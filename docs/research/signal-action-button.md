# Bouton d'action natif : observation statique du 27 septembre 2026

Analyse locale Windows, Ghidra 12.1.3, JDK 21, projet existant ouvert avec
`-readOnly -noanalysis`. Aucun accès au VPS, appel natif ni lancement du jeu.
SHA-256 analysé : `fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae`.

Le slot `+0x80` précédemment envisagé reçoit une structure d'image, pas un texte.
Il ne convient donc pas à un bouton portant seulement « Répéter ce signal ».

Le slot `+0x90` de la table interactive `RVA 0xa83470` pointe vers `0x55ef00`.
Le pseudo-code lit une chaîne terminée par NUL, appelle `0x50c3b0`, consomme
un élément de layout et retourne 0 ou 1. Son troisième argument contient des
flags ; le bit 0 empêche le retour d'un clic. La table de layout `0xa83818`
pointe au même slot vers `0x55c9c0`, qui reste à examiner avec ses appelants.

Le slot `+0x98` ajoute une structure d'image et d'autres paramètres ; il ne doit
pas être appelé avec la signature du bouton texte.

Ces constats ne qualifient pas encore une API publique. Avant intégration :

- confirmer la convention des deux passes avec les appelants et le layout ;
- conserver la même liste d'actions entre layout et interaction ;
- mettre le clic en file avec identité de partie et de signal, sans rappeler
  directement une DLL de mod depuis le thread UI ;
- retirer les actions lorsque leur fournisseur optionnel disparaît et rejeter
  les clics déjà en attente après changement de partie ;
- valider le service de placement et l'annulation de construction indépendamment
  de l'affichage du bouton ;
- qualifier le comportement dans le jeu avant de le distribuer.

Les exports locaux se trouvent dans `build/button-analysis`, produits par
`InspectTrains.java` pour les VA `14055f0b0`, `14055ef00`, `14055f340`.
Ils ne sont pas inclus dans le kit utilisateur. Le wiki présente toujours cette
intégration comme indisponible, et SFR fonctionne sans ce fournisseur.
