// Recherche statique uniquement : aucun accès au processus du jeu.
// Exécuter dans Ghidra avec -readOnly -noanalysis et un dossier de sortie.
//@category NimbyRails
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.listing.*;
import java.nio.file.*;
import java.util.*;

public class InspectDrivingDispatch extends GhidraScript {
    public void run() throws Exception {
        Path output = Path.of(getScriptArgs()[0]);
        Files.createDirectories(output);
        StringBuilder evidence = new StringBuilder("SHA256=" + currentProgram.getExecutableSHA256() + "\n");
        Set<Function> candidates = new LinkedHashSet<>();
        // Offsets observés dans l'enregistrement des événements du binaire étudié.
        // Une occurrence est une piste, pas une preuve de l'identité d'un objet.
        for (Instruction instruction : currentProgram.getListing().getInstructions(true)) {
            monitor.checkCancelled();
            String assembly = instruction.toString().toLowerCase(Locale.ROOT);
            if (!assembly.contains("0xe20") && !assembly.contains("0x1258")) continue;
            Function function = getFunctionContaining(instruction.getAddress());
            evidence.append(instruction.getAddress()).append(" ").append(assembly)
                .append(" function=").append(function == null ? "none" : function.getEntryPoint()).append("\n");
            // Écarter les faux positifs fréquents : variables locales sur la pile.
            if (function != null && !assembly.contains("rsp") && !assembly.contains("rbp")) candidates.add(function);
        }
        Files.writeString(output.resolve("dispatch-offsets.txt"), evidence);
        DecompInterface decompiler = new DecompInterface();
        decompiler.openProgram(currentProgram);
        try {
            for (Function function : candidates) {
                monitor.checkCancelled();
                var result = decompiler.decompileFunction(function, 20, monitor);
                if (result.decompileCompleted()) Files.writeString(
                    output.resolve(function.getEntryPoint() + ".c"), result.getDecompiledFunction().getC());
            }
        } finally { decompiler.dispose(); }
    }
}
