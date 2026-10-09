package nimby

/** Préférence du joueur, commune à toutes ses parties et propre à ce mod.
 * Déclarer l'option avec `options(...)`, puis lire [value] dans les callbacks
 * du mod. Cette lecture utilise le cache du SDK : aucun accès au jeu ni au disque.
 * Le SDK applique les changements entre deux callbacks sur le worker du mod.
 * [id] reste stable entre versions ; [label] et [description] acceptent [tr]. */
sealed class ModOption<T> protected constructor(
    val id: String,
    val label: String,
    val defaultValue: T,
    val description: String,
) {
    init {
        validateOptionId(id)
        require(!id.startsWith("window.")) { "Le préfixe window. est réservé aux raccourcis des fenêtres du SDK." }
        validateOptionText(label, 256)
        validateOptionText(description, 1024, allowEmpty = true)
    }

    private var currentValue: T = defaultValue
    val value: T get() = currentValue

    internal abstract val kind: Int
    internal abstract val encodedDefault: String
    internal abstract fun parse(value: String): T

    // Validation is completed for every option before any prepared write runs.
    // No mod callback or user-provided conversion participates in this step.
    internal fun prepareUpdate(text: String): () -> Unit {
        val parsed = parse(text)
        return { currentValue = parsed }
    }
}

class BooleanOption(
    id: String, label: String, defaultValue: Boolean = false, description: String = "",
) : ModOption<Boolean>(id, label, defaultValue, description) {
    internal override val kind: Int get() = 0
    internal override val encodedDefault: String get() = defaultValue.toString()
    internal override fun parse(value: String): Boolean = when (value) {
        "true" -> true
        "false" -> false
        else -> throw IllegalArgumentException("Valeur booléenne invalide pour $id")
    }
}

/** Nombre entier inclus entre [minimum] et [maximum]. */
class IntegerOption(
    id: String, label: String, defaultValue: Int, val minimum: Int, val maximum: Int,
    description: String = "",
) : ModOption<Int>(id, label, defaultValue, description) {
    init { require(minimum <= maximum && defaultValue in minimum..maximum) { "Bornes ou valeur par défaut invalides pour $id" } }
    internal override val kind: Int get() = 1
    internal override val encodedDefault: String get() = defaultValue.toString()
    internal override fun parse(value: String): Int {
        val parsed = requireNotNull(value.toIntOrNull()) { "Nombre entier invalide pour $id" }
        require(parsed.toString() == value && parsed in minimum..maximum) { "Nombre hors limites ou non canonique pour $id" }
        return parsed
    }
}

/** [id] est enregistré ; seul [label], éventuellement fourni par [tr], est affiché. */
data class OptionChoice(val id: String, val label: String) {
    init { validateOptionId(id); validateOptionText(label, 256) }
}

/** Choix parmi deux à seize valeurs. [value] fournit l'identifiant du choix. */
class ChoiceOption(
    id: String, label: String, choices: List<OptionChoice>, defaultValue: String,
    description: String = "",
) : ModOption<String>(id, label, defaultValue, description) {
    val choices: List<OptionChoice> = choices.toList()
    private val identifiers = this.choices.mapTo(HashSet()) { it.id }
    init {
        require(this.choices.size in 2..16 && identifiers.size == this.choices.size && defaultValue in identifiers) {
            "Choix dupliqués, absents ou valeur par défaut invalide pour $id"
        }
    }
    internal override val kind: Int get() = 2
    internal override val encodedDefault: String get() = defaultValue
    internal override fun parse(value: String): String {
        require(value in identifiers) { "Choix inconnu pour $id" }
        return value
    }
}

/** Raccourci configurable ; une chaîne vide signifie « désactivé ».
 * Format : Ctrl, Alt, Shift dans cet ordre, puis une touche (ex. Ctrl+Shift+T).
 * Touches : A–Z, 0–9, F1–F24, Backspace, Tab, Enter, Escape, Space, Left,
 * Right, Up, Down, Home, End, PageUp, PageDown, Insert, Delete.
 * Cette préférence ne crée pas de callback de touche. Les raccourcis d'ouverture
 * des fenêtres sont déclarés automatiquement par [ToolModBuilder.window]. */
internal class ShortcutOption(
    id: String, label: String, defaultValue: String = "", description: String = "",
) : ModOption<String>(id, label, defaultValue, description) {
    init { require(validOptionShortcut(defaultValue)) { "Raccourci par défaut invalide pour $id" } }
    internal override val kind: Int get() = 3
    internal override val encodedDefault: String get() = defaultValue
    internal override fun parse(value: String): String {
        require(validOptionShortcut(value)) { "Raccourci invalide pour $id" }
        return value
    }
}

internal fun validateOptionId(value: String) {
    require(value.length in 1..128 && (value[0] in 'A'..'Z' || value[0] in 'a'..'z')) {
        "Identifiant d'option invalide : $value"
    }
    require(value.all { it in 'A'..'Z' || it in 'a'..'z' || it in '0'..'9' || it in "_.-" }) {
        "Identifiant d'option invalide : $value"
    }
}

internal fun validateOptionText(value: String, maximumBytes: Int, allowEmpty: Boolean = false) {
    require((allowEmpty || value.isNotBlank()) && '\u0000' !in value && value.encodeToByteArray().size <= maximumBytes) {
        "Texte d'option vide, trop long ou contenant un caractère nul"
    }
}

internal fun checkedModOptions(options: List<ModOption<*>>, reservedCount: Int = 0): List<ModOption<*>> {
    require(reservedCount in 0..64 && options.size <= 64 - reservedCount) {
        "Au maximum 64 options par mod, raccourcis des fenêtres compris"
    }
    require(options.map { it.id }.distinct().size == options.size) { "Identifiant d'option dupliqué" }
    return options.toList()
}

internal fun validOptionShortcut(value: String): Boolean {
    if (value.isEmpty()) return true
    var key = value
    for (modifier in listOf("Ctrl+", "Alt+", "Shift+")) if (key.startsWith(modifier)) key = key.removePrefix(modifier)
    if (key.length == 1 && (key[0] in 'A'..'Z' || key[0] in '0'..'9')) return true
    if (key.startsWith('F')) {
        val number = key.drop(1).toIntOrNull()
        if (number != null && number in 1..24 && key == "F$number") return true
    }
    return key in setOf("Backspace", "Tab", "Enter", "Escape", "Space", "Left", "Right", "Up", "Down",
        "Home", "End", "PageUp", "PageDown", "Insert", "Delete")
}
