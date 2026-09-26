package nimby

/**
 * Construit les consignes executees par le moteur natif, sans regle nationale.
 * Le mod choisit les vitesses (m/s), la cible et chaque permission. Ces fonctions
 * ne lisent ni couleur ni occupation et ne publient rien par elles-memes : le
 * resultat est renvoye par SignallingMod.drivingRule puis publie par son worker.
 * Zero represente un arret ; aucune vitesse de circulation n'est implicite.
 */
object AutomaticDriving {
    private fun speed(value: Double): Double {
        require(value.isFinite() && value >= 0.0) { "A speed must be finite and nonnegative" }
        return value
    }

    /** Libere les consignes dont le mod a choisi une sortie sur Clear. */
    fun clear() = DrivingRule(flags = setOf(DrivingFlag.Clear))

    /** Exige l'arret au panneau courant, sans autorisation de franchissement. */
    fun stop() = DrivingRule(speedMps = 0.0, flags = setOf(DrivingFlag.Stop))

    /**
     * Memorise, au franchissement, un objectif d'arret au panneau designe.
     * Une permission explicite de ce panneau remplace l'arret par passageSpeedMps
     * jusqu'au passage de la tete. La contrainte ne disparait pas a la reouverture.
     * passableHere concerne CE panneau, pas la cible annoncee. followTargetSpeed
     * autorise la lecture d'une vitesse numerique visible de la cible. La
     * cancellation au prochain Clear est une politique optionnelle du mod.
     */
    fun announceStop(signalsAhead: Int, passageSpeedMps: Double,
                     passableHere: Boolean, followTargetSpeed: Boolean = false,
                     cancelAtNextClear: Boolean = false): DrivingRule {
        require(signalsAhead in 1..2)
        require(!cancelAtNextClear || (followTargetSpeed && signalsAhead == 2))
        return DrivingRule(0.0, speed(passageSpeedMps), signalsAhead, buildSet {
            if (passableHere) add(DrivingFlag.ApproachPassable)
            if (followTargetSpeed) add(DrivingFlag.FollowTarget)
            if (cancelAtNextClear) add(DrivingFlag.CancelAtNextClear)
        })
    }

    /** Plafond ponctuel a ce panneau, sans restriction retenue apres passage. */
    fun limitAtSignal(speedMps: Double, passableHere: Boolean): DrivingRule {
        require(!passableHere || speedMps > 0.0)
        return DrivingRule(speedMps = speed(speedMps),
            flags = if (passableHere) setOf(DrivingFlag.ApproachPassable) else emptySet())
    }

    /**
     * Plafond a la cible, retenu jusqu'au degagement par la queue d'un Clear
     * effectivement franchi. Choisir cette fonction est une decision du mod.
     */
    fun limitUntilClearThenRear(speedMps: Double, signalsAhead: Int, passableHere: Boolean): DrivingRule {
        require(signalsAhead in 0..2 && speed(speedMps) > 0.0)
        return DrivingRule(speedMps = speedMps, signalsAhead = signalsAhead, flags = buildSet {
            add(DrivingFlag.HoldToClear)
            if (passableHere) add(DrivingFlag.ApproachPassable)
        })
    }

    /**
     * Apres passage, plafond conserve jusqu'au panneau suivant (passage tete),
     * avec freinage sur l'espace libre physiquement verifie. stopFirst impose
     * une preuve d'arret avant entree ; les permissions natives restent actives.
     */
    fun restrictedUntilNextSignal(entrySpeedMps: Double, maximumSpeedMps: Double, stopFirst: Boolean): DrivingRule {
        require(speed(maximumSpeedMps) > 0.0)
        require(speed(entrySpeedMps) <= maximumSpeedMps && (!stopFirst || entrySpeedMps == 0.0))
        return DrivingRule(entrySpeedMps, maximumSpeedMps, flags = buildSet {
            add(DrivingFlag.OnSight)
            if (stopFirst) { add(DrivingFlag.Stop); add(DrivingFlag.StopThenProceed) }
        })
    }
}
