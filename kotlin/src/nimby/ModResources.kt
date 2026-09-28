package nimby

/** Identité du paquet, générée depuis mod.json sous le nom nimby.mod.modInfo.
 * Réutiliser cette valeur dans signalMod ou toolMod évite de recopier le nom
 * et l'identifiant. Le Hub peut lire mod.json avant toute compilation. */
data class ModInfo(val id: String, val title: String)

/** Informations du mod et textes des listes et fiches de mods dans le jeu.
 * L'auteur et la description ne sont pas des règles de signalisation.
 * name et description acceptent tr(...). Le nom technique et la version
 * restent ceux de mod.json ; name ne change que le texte affiché.
 * Le paquet conserve une version lisible dans la langue de repli même sans
 * l'adaptation Windows de localisation du SDK. Le sélecteur de nouvelle partie
 * et le gestionnaire de mods en partie sont couverts. Les pages du Hub et de
 * Steam restent des métadonnées de distribution, indépendantes du jeu. */
data class ModMetadata(val author: String, val description: String, val name: String? = null)

/** Déclaration du signal dans les menus de construction du jeu.
 * states contient les chemins relatifs au paquet, dans leur ordre définitif.
 * Ne pas réordonner ni retirer les anciennes entrées d'un catalogue publié.
 * name est le nom constructible ; catalogueName est le nom du jeu de textures.
 * Tous deux acceptent tr(...) : le SDK génère les clés de localisation natives
 * et suit la langue du jeu dans les catalogues. Sans son adaptation Windows,
 * le nom anglais (ou la langue de repli disponible) reste lisible.
 * Les clés nameKey/catalogueNameKey désignent la localisation native du jeu,
 * uniquement pour conserver un catalogue existant. Elles ne sont pas des clés tr
 * et ne doivent pas être combinées avec tr(...) pour le même nom.
 * kind correspond au type natif : path est le type utilisé par nos signaux.
 * Le plugin vérifie les chemins et la présence des fichiers à la compilation. */
class SignalConstruction(
    states: List<String>, val name: String, val kind: String = "path",
    val catalogueName: String = name,
    val nameKey: String? = null, val catalogueNameKey: String? = null,
) {
    val states: List<String> = states.toList()
    init {
        require(states.isNotEmpty() && states.size <= 256) { "Déclarer de 1 à 256 images" }
        require(states.distinct().size == states.size) { "Image déclarée plusieurs fois" }
        require(kind.matches(Regex("[a-z_]{1,32}"))) { "Type de signal natif invalide" }
    }
}

/** Assemble le mod avec l'identité générée à partir de mod.json. */
fun signalMod(info: ModInfo, block: SignalModelsBuilder.() -> Unit): SignallingMod =
    signalMod(info.id, info.title, block)

/** Assemble l'outil avec l'identité générée à partir de mod.json. */
fun toolMod(info: ModInfo, block: ToolModBuilder.() -> Unit): ToolMod =
    toolMod(info.id, info.title, block)
