# Quais et occupation par gare (SDK 0.7)

```cpp
const auto snapshot = client.capture();
if (auto platforms = snapshot->getPlatformOccupationsForStation(stationId)) {
    for (const auto& row : *platforms) {
        const auto name = row.platform.getName();
        const auto occupied = row.isOccupied(); // nullopt = inconnu
        if (occupied && *occupied) {
            for (const auto& train : *row.occupying_trains) {
                // train.getId(), train.getName()
            }
        }
        // row.reserving_trains : reservations, independantes de l'occupation.
    }
}
```

Chaque resultat correspond a une **section de voie de gare** avec son nom de
quai natif, son ID de voie et son ID de gare. Un quai peut comprendre plusieurs
sections, parfois avec des noms identiques ou des suffixes N/S/E/W. Ne pas
considerer une section libre comme la preuve que tout le quai est libre ;
les identifiants de voie restent les cles uniques. Les trains sont dedoublonnes
par ID complet au sein de chaque section, meme si plusieurs voitures l'occupent.
Plusieurs trains sur une section sont tous retournes.

- `isOccupied() == true` : au moins une occupation native de voiture.
- `isOccupied() == false` : table d'occupation lue, aucune occupation sur cette section.
- `isOccupied() == nullopt` : occupation indisponible ; ne pas afficher libre.
- `occupying_trains` et `reserving_trains` sont independamment optionnels.
- Gare inconnue : resultat `nullopt`. Gare existante sans section : tableau vide.

Un train reserve une voie avant d'y entrer. Une reservation ne signifie donc
pas que le quai est physiquement occupe. Les trains masques au depot ne sont pas
ajoutes artificiellement aux occupations physiques. Libre ne signifie pas
qu'une nouvelle circulation est autorisee. Le snapshot est une observation
non atomique, pas une commande de signalisation.

## Evidence native et validation

Profil SHA-256 :
`fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae`.
`research/reports/train-timetable/14038af60.c` construit le nom du quai :
Track+0xc8 selectionne manuel (+0xa8, chaine MSVC) ou automatique (int +0xa0 +1).
`14036d0a0.c` fournit les voisins +8 et +16 dans cet ordre.
Pour une extremite, le suffixe depend des gares voisines et des premiers/derniers
points doubles du vecteur +0x1b0/+0x1b8. Les constantes du binaire aux RVA
0xa4e3f0, 0xa6efa0, 0xa6efa8, 0xa6efa4 sont S, N, W, E.
Les chaines, points et champs determinants sont relus avant validation du nom.
Une lecture de nom incorrecte laisse la section disponible avec nom inconnu.

Les occupations proviennent de la table native des voitures Sim+0x2c8, deja
validee par le lecteur `track_usage`. Aucun calcul de presence a partir de la
vitesse ou du seul point avant du train n'est utilise.

Tests : noms manuels, numerotation automatique et quatre orientations,
lecture impossible, reservation seule, occupation par plusieurs trains,
dedoublonnage des voitures, occupation indisponible et gare inconnue.
Sur la partie de verification : 3 342 gares et 23 190 sections, toutes avec
un nom resolu. L'outil automatique et la verification de l'API terminent sans erreur.
