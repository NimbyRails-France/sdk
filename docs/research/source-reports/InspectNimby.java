// Export a small, read-only inventory of signaling-related strings and references.
//@category NimbyRails

import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;
import com.google.gson.*;
import java.nio.file.*;
import java.nio.charset.StandardCharsets;
import java.util.Locale;

public class InspectNimby extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length != 1) throw new IllegalArgumentException("Expected output JSON path");
        JsonObject report = new JsonObject();
        report.addProperty("program", currentProgram.getName());
        report.addProperty("language", currentProgram.getLanguageID().toString());
        report.addProperty("compiler", currentProgram.getCompilerSpec().getCompilerSpecID().toString());
        report.addProperty("image_base", currentProgram.getImageBase().toString());
        report.addProperty("function_count", currentProgram.getFunctionManager().getFunctionCount());
        JsonArray matches = new JsonArray();
        DataIterator data = currentProgram.getListing().getDefinedData(true);
        while (data.hasNext()) {
            monitor.checkCancelled();
            Data item = data.next();
            Object value = item.getValue();
            if (!(value instanceof String)) continue;
            String text = (String)value;
            String lower = text.toLowerCase(Locale.ROOT);
            if (!(lower.contains("signal_lookahead") || lower.contains("signallookaheadresult") ||
                lower.contains("max_speed") || lower.contains("max_regular_braking") ||
                lower.contains("waiting_signal_id") || lower.contains("extrapolator"))) continue;
            JsonObject match = new JsonObject();
            match.addProperty("address", item.getAddress().toString());
            match.addProperty("text", text);
            JsonArray refs = new JsonArray();
            ReferenceIterator it = currentProgram.getReferenceManager().getReferencesTo(item.getAddress());
            while (it.hasNext() && refs.size() < 20) {
                Reference ref = it.next();
                JsonObject entry = new JsonObject();
                entry.addProperty("from", ref.getFromAddress().toString());
                entry.addProperty("type", ref.getReferenceType().toString());
                Function fn = currentProgram.getFunctionManager().getFunctionContaining(ref.getFromAddress());
                if (fn != null) {
                    entry.addProperty("function", fn.getName());
                    entry.addProperty("function_address", fn.getEntryPoint().toString());
                }
                refs.add(entry);
            }
            match.add("references", refs);
            matches.add(match);
        }
        report.add("candidate_strings", matches);
        Files.writeString(Path.of(args[0]), new GsonBuilder().setPrettyPrinting().create().toJson(report),
            StandardCharsets.UTF_8, StandardOpenOption.CREATE_NEW);
        println("NIMBY inventory: " + matches.size() + " candidate strings; " +
            currentProgram.getFunctionManager().getFunctionCount() + " functions; " + args[0]);
    }
}
