// Read-only evidence export; all addresses are candidates, never hook approvals.
//@category NimbyRails
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;
import com.google.gson.*;
import java.nio.file.*;
import java.nio.charset.StandardCharsets;
import java.util.*;

public class InspectTrains extends GhidraScript {
    private String rva(ghidra.program.model.address.Address a) {
        return "0x" + Long.toHexString(a.subtract(currentProgram.getImageBase()));
    }
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 1) throw new IllegalArgumentException("Output directory required; optional function VAs");
        Path dir = Path.of(args[0]);
        Files.createDirectories(dir);
        JsonObject root = new JsonObject();
        root.addProperty("sha256", currentProgram.getExecutableSHA256());
        root.addProperty("image_base", currentProgram.getImageBase().toString());
        root.addProperty("status", "Static evidence only; no validated hooks or object layouts");
        JsonArray matches = new JsonArray();
        Set<Function> functions = new LinkedHashSet<>();
        DataIterator data = currentProgram.getListing().getDefinedData(true);
        while (data.hasNext()) {
            monitor.checkCancelled();
            Data d = data.next();
            if (!(d.getValue() instanceof String)) continue;
            String s = (String)d.getValue();
            String lower = s.toLowerCase(Locale.ROOT);
            if (!(lower.contains("speed") || lower.contains("train") || lower.contains("dynamics") || lower.contains("motion") ||
                lower.contains("track") || lower.contains("signal") || lower.contains("station") || lower.contains("platform") ||
                lower.contains("balise") || lower.contains("beacon") || lower.contains("position"))) continue;
            JsonObject entry = new JsonObject();
            entry.addProperty("text", s);
            entry.addProperty("rva", rva(d.getAddress()));
            JsonArray refs = new JsonArray();
            ReferenceIterator it = currentProgram.getReferenceManager().getReferencesTo(d.getAddress());
            while (it.hasNext()) {
                Reference ref = it.next();
                JsonObject j = new JsonObject();
                j.addProperty("from_rva", rva(ref.getFromAddress()));
                j.addProperty("type", ref.getReferenceType().toString());
                Function f = getFunctionContaining(ref.getFromAddress());
                if (f != null) {
                    j.addProperty("function_va", f.getEntryPoint().toString());
                    j.addProperty("function_rva", rva(f.getEntryPoint()));
                    if (args.length == 1 && (lower.equals("speed") || lower.equals("train") || lower.equals("trains") || lower.equals("dynamics") || lower.equals("current_speed"))) functions.add(f);
                }
                refs.add(j);
            }
            entry.add("references", refs);
            matches.add(entry);
        }
        root.add("strings", matches);
        Files.writeString(dir.resolve("strings.json"), new GsonBuilder().setPrettyPrinting().create().toJson(root), StandardCharsets.UTF_8);
        for (int i = 1; i < args.length; ++i) {
            Function f = getFunctionContaining(toAddr(args[i]));
            if (f == null) {
                // Explicit addresses only; discoveries remain temporary with -readOnly.
                disassemble(toAddr(args[i]));
                f = createFunction(toAddr(args[i]), null);
            }
            if (f == null) throw new IllegalArgumentException("No function: " + args[i]);
            functions.add(f);
        }
        DecompInterface decompiler = new DecompInterface();
        try {
            decompiler.openProgram(currentProgram);
            for (Function f : functions) {
                monitor.checkCancelled();
                StringBuilder report = new StringBuilder("// Candidate VA " + f.getEntryPoint() + "; RVA " + rva(f.getEntryPoint()) + "\n");
                report.append("// Ghidra inferred prototype: " + f.getPrototypeString(false, false) + "\n");
                DecompileResults result = decompiler.decompileFunction(f, 180, monitor);
                if (result.decompileCompleted()) report.append(result.getDecompiledFunction().getC());
                else report.append("// Decompilation failed: " + result.getErrorMessage());
                report.append("\n// Incoming references\n");
                ReferenceIterator refs = currentProgram.getReferenceManager().getReferencesTo(f.getEntryPoint());
                while (refs.hasNext()) {
                    Reference ref = refs.next();
                    Function caller = getFunctionContaining(ref.getFromAddress());
                    report.append("// " + rva(ref.getFromAddress()) + " " + ref.getReferenceType() + " caller " + (caller == null ? "none" : caller.getEntryPoint()) + "\n");
                }
                Files.writeString(dir.resolve(f.getEntryPoint() + ".c"), report, StandardCharsets.UTF_8);
            }
        } finally { decompiler.dispose(); }
        println("Train evidence: " + matches.size() + " strings, " + functions.size() + " functions");
    }
}
