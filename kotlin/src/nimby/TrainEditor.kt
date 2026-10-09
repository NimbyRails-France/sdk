package nimby

/** Règles de composition appliquées par le SDK avant une modification du train.
 *
 * Déclarer avec [ToolModBuilder.trainEditor]. La déclaration est immuable :
 * elle ne crée ni fenêtre, ni service, ni callback périodique. Les messages
 * appartiennent au mod et suivent la langue du jeu via [tr]. Le SDK ajoute
 * séparément les longueurs mesurées et la limite aux refus de dépassement. */
class TrainEditor internal constructor(val maximumLength: TrainLengthLimit)

/** Configuration exécutée une seule fois lors de la création du mod. */
class TrainEditorBuilder internal constructor() {
    private var declaredMaximumLength: TrainLengthLimit? = null

    /** Limite la longueur totale, locomotives et autres véhicules compris.
     *
     * [meters] doit être la même [IntegerOption] que celle passée à
     * [ToolModBuilder.options]. Le mod choisit la valeur par défaut.
     * Les trois messages sont obligatoires : texte fixe ou [tr] sans paramètres.
     * Les traductions sont résolues par le SDK à l'affichage, sans appeler le mod.
     * Une baisse de la limite ne raccourcit pas les trains existants. */
    fun maximumLength(
        meters: IntegerOption,
        exceeded: String,
        lengthUnavailable: String,
        verificationUnavailable: String,
    ) {
        require(declaredMaximumLength == null) { "Limite de longueur des trains déjà déclarée" }
        declaredMaximumLength = TrainLengthLimit(meters, exceeded, lengthUnavailable, verificationUnavailable)
    }

    internal fun build(): TrainEditor = TrainEditor(requireNotNull(declaredMaximumLength) {
        "Déclarer maximumLength(...) dans trainEditor { ... }"
    })
}

internal fun checkedTrainEditorMessage(message: String) {
    require(message.isNotBlank() && message.encodeToByteArray().size <= 1024 && '\u0000' !in message) {
        "Le message de composition doit contenir entre 1 et 1024 octets UTF-8, sans caractère nul"
    }
    if (message.startsWith("\u001eNRF:")) {
        require(message.matches(Regex("\u001eNRF:\\[\"[a-zA-Z0-9_.-]{1,96}\",\\{\\}\\]"))) {
            "Utiliser tr(\"clé\") sans paramètres pour les messages de composition"
        }
    } else require(message.none { it < ' ' && it != '\n' && it != '\t' }) {
        "Le message de composition contient un caractère de contrôle"
    }
}

internal fun checkedTrainEditor(editor: TrainEditor?, options: List<ModOption<*>>): TrainEditor? {
    require(editor == null || options.any { it === editor.maximumLength.option }) {
        "Déclarer la même IntegerOption dans options(...) et trainEditor { maximumLength(...) }"
    }
    return editor
}
