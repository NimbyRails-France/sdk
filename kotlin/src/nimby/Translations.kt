package nimby

import nimby.internal.TranslationEnvironment

/** Référence à un texte de `assets/translations.json`, traduite par le SDK à l'affichage.
 *
 * Exemple : `ToolButton("repeat", tr("repeat"))` ou
 * `tr("preview.count", "count" to positions.size)`.
 * Les paramètres remplacent les marqueurs `{count}` du JSON. Les doubles accolades
 * `{{` et `}}` affichent une accolade. Le catalogue choisit sa langue `fallback`
 * (anglais si omise). Une traduction absente utilise cette langue, puis la clé.
 *
 * Réserver cette valeur aux titres, descriptions, boutons et messages d'interface.
 * Ne pas la concaténer, la stocker comme identifiant ou l'utiliser dans les journaux :
 * c'est une référence différée, pas une traduction calculée au moment de l'appel.
 * Les identifiants des réglages et actions restent indépendants de la langue.
 */
fun tr(key: String, vararg arguments: Pair<String, Any>): String {
    check(TranslationEnvironment.catalogAvailable) { "Missing translations.json beside the mod DLL; add assets/translations.json and rebuild the package" }
    fun identifier(value: String) = value.isNotEmpty() && value.length <= 96 &&
        value.all { it in 'a'..'z' || it in 'A'..'Z' || it in '0'..'9' || it in "._-" }
    require(identifier(key)) { "Invalid translation key: $key" }
    require(arguments.size <= 8 && arguments.map { it.first }.distinct().size == arguments.size) {
        "At most 8 unique translation arguments are allowed"
    }
    fun quote(value: String): String = buildString {
        append('"')
        for (c in value) when (c) {
            '"' -> append("\\\"")
            '\\' -> append("\\\\")
            '\n' -> append("\\n")
            '\r' -> append("\\r")
            '\t' -> append("\\t")
            else -> { require(c >= ' ') { "Control character in translation argument" }; append(c) }
        }
        append('"')
    }
    val values = arguments.joinToString(",") { (name, value) ->
        require(identifier(name)) { "Invalid translation argument: $name" }
        quote(name) + ":" + quote(value.toString())
    }
    // Private transport: existing String controls remain source-compatible.
    return "\u001eNRF:[${quote(key)},{$values}]".also {
        require(it.encodeToByteArray().size <= 256) { "Translation reference exceeds 256 UTF-8 bytes" }
    }
}
