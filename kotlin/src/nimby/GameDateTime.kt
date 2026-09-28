package nimby

/** Date et heure UTC, calendrier grégorien, de l'an 1 à 9999.
 * Aucun fuseau Windows ni décalage d'affichage du jeu n'est appliqué.
 * Une date impossible est refusée, jamais corrigée silencieusement. */
data class GameDateTime(val year: Int, val month: Int, val day: Int,
                        val hour: Int = 0, val minute: Int = 0, val second: Int = 0) {
    init {
        require(year in 1..9999 && month in 1..12 && day in 1..daysInMonth(year, month)) { "Date invalide" }
        require(hour in 0..23 && minute in 0..59 && second in 0..59) { "Heure invalide" }
    }
    fun toUtcSeconds(): Long {
        val days = daysBeforeYear(year) - daysBeforeYear(1970) + (1 until month).sumOf { daysInMonth(year, it) } + day - 1
        return days * 86400L + hour * 3600 + minute * 60 + second
    }
    override fun toString(): String = "${year.toString().padStart(4, '0')}-${month.toString().padStart(2, '0')}-${day.toString().padStart(2, '0')}T${hour.toString().padStart(2, '0')}:${minute.toString().padStart(2, '0')}:${second.toString().padStart(2, '0')}Z"
    companion object {
        private fun daysBeforeYear(year: Int): Int { val y = year - 1; return 365 * y + y / 4 - y / 100 + y / 400 }
        private fun daysInMonth(year: Int, month: Int) = when(month) {
            2 -> if(year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) 29 else 28
            4, 6, 9, 11 -> 30
            else -> 31
        }
        fun fromUtcSeconds(seconds: Long): GameDateTime {
            require(seconds in -62135596800L..253402300799L) { "Date hors limites" }
            val totalDays = seconds.floorDiv(86400L).toInt() + daysBeforeYear(1970)
            var low = 1; var high = 10000
            while(high - low > 1) { val mid = (low + high) / 2; if(daysBeforeYear(mid) <= totalDays) low = mid else high = mid }
            var days = totalDays - daysBeforeYear(low); var month = 1
            while(days >= daysInMonth(low, month)) { days -= daysInMonth(low, month); month++ }
            val time = seconds.mod(86400L).toInt()
            return GameDateTime(low, month, days + 1, time / 3600, time / 60 % 60, time % 60)
        }
    }
}

/** Convertit l'observation en date UTC sans modifier le jeu. */
fun ToolClock.dateTime(): GameDateTime = GameDateTime.fromUtcSeconds(utcSeconds)
