package nimby

/** Fenêtre de l'outil, indépendante d'un signal. Sous Windows, le raccourci
 * ouvre un panneau appartenant à la fenêtre du jeu, uniquement au premier plan.
 * Ce n'est pas un bouton injecté dans la barre native du jeu.
 * shortcut : Ctrl+, Alt+, Shift+ facultatifs dans cet ordre, puis une touche
 * (A–Z, 0–9, F1–F24, Enter, Space, Tab, Escape, Backspace, Delete, Insert,
 * Home, End, PageUp, PageDown, Left, Right, Up ou Down). Vide le désactive.
 * Le joueur peut le modifier dans les options des mods, gérées par le SDK. */
data class ToolWindow(val id: String, val title: String, val shortcut: String)

/** Action d'une fenêtre : open à l'ouverture, sinon l'id du bouton cliqué.
 * values contient tous les entiers validés du formulaire à cet instant.
 * Le SDK fournit cette requête : ne pas en fabriquer ni la réutiliser après
 * un changement de partie. sequence associe la réponse au formulaire courant. */
data class ToolWindowEvent(
    val window: String, val action: String, val values: Map<String, Int>,
    val sequence: Long, val worldId: String, val generation: Long,
)

/** Horloge UTC observée. elapsedMillis est le temps simulé écoulé ; modifier
 * la date ne simule pas les journées intermédiaires. */
data class ToolClock(val utcSeconds: Long, val elapsedMillis: Long)

/** Résultat d'un changement explicite. interventions vaut zéro en mode
 * translation ; le recalcul optionnel peut déplacer les trains et coûter de
 * l'argent comme les interventions natives. Ne pas rejouer après une erreur. */
data class ToolTimeChange(val clock: ToolClock, val interventions: Long)
