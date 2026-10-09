package nimby

/** Plus petite longueur configurable, en mètres. */
const val minimumTrainLengthMeters: Int = 1

/** Borne de configuration du SDK, en mètres. Ce n'est pas un nombre de
 * véhicules ni une garantie de compatibilité du jeu : le SDK vérifie aussi
 * la version native et ses limites techniques avant d'activer la fonction. */
const val maximumTrainLengthMeters: Int = 10000

/** Limite de longueur totale d'une composition, liée à une préférence du mod.
 * Déclarer avec [TrainEditorBuilder.maximumLength] et enregistrer la même
 * [IntegerOption] avec [ToolModBuilder.options]. Locomotives et autres
 * véhicules font partie de la longueur totale ; aucun nombre de voitures
 * n'est choisi par le mod.
 *
 * Le SDK applique la préférence au démarrage et à chaque changement validé.
 * Cette déclaration ne crée ni fenêtre, ni raccourci, ni callback périodique.
 * Elle ne raccourcit pas les trains existants lorsque la limite diminue.
 * Si plusieurs mods déclarent une limite, le SDK applique la plus petite des
 * limites actives ; chaque mod conserve sa propre préférence.
 * [maximumLengthMeters] lit uniquement le cache des options du SDK : conserver
 * cette déclaration ne fournit aucun accès direct au jeu. */
class TrainLengthLimit internal constructor(
    internal val option: IntegerOption,
    /** Refus d'un ajout qui dépasserait la longueur maximale. */
    val exceeded: String,
    /** Refus lorsque la longueur d'un véhicule est indisponible. */
    val lengthUnavailable: String,
    /** Refus lorsque le SDK ne peut pas vérifier la composition. */
    val verificationUnavailable: String,
) {
    init {
        require(option.minimum >= minimumTrainLengthMeters && option.maximum <= maximumTrainLengthMeters) {
            "La longueur des trains doit être comprise entre $minimumTrainLengthMeters et $maximumTrainLengthMeters mètres"
        }
        listOf(exceeded, lengthUnavailable, verificationUnavailable).forEach(::checkedTrainEditorMessage)
    }

    /** Longueur totale maximale demandée par le joueur, en mètres. */
    val maximumLengthMeters: Int get() = option.value

    /** Identifiant stable de la préférence liée à cette déclaration. */
    val optionId: String get() = option.id
}
