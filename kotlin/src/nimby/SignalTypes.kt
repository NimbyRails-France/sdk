package nimby

/** Contrat commun au chargement natif et aux tests hors jeu. Les catalogues
 * doivent être distincts : le SDK ne devine jamais les règles d'un signal. */
fun validateSignalTypes(types: List<SignalType>) {
    require(types.size in 1..16) { "Un mod déclare entre 1 et 16 types de signaux." }
    require(types.map { it.id }.toSet().size == types.size) { "Identifiant de type dupliqué." }
    require(types.map { it.textureSet }.toSet().size == types.size) { "Catalogue de textures partagé entre deux types." }
    fun text(value: String, limit: Int, empty: Boolean = false) {
        require((empty || value.isNotBlank()) && '\u0000' !in value && value.encodeToByteArray().size <= limit)
    }
    types.forEach { type ->
        require(type.approachBlocks in 1..16) { "La portée d'approche doit être comprise entre 1 et 16 cantons." }
        validateSignalActions(type.actions)
        text(type.id, 128); text(type.title, 256); text(type.textureSet, 256)
        require(type.checkboxes.size <= 64)
        require(type.checkboxes.map { it.name }.toSet().size == type.checkboxes.size)
        type.checkboxes.forEach { box ->
            text(box.name, 128); text(box.label, 256); text(box.description, 1024, empty = true)
        }
    }
}
