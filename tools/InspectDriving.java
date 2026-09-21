// Static evidence only. Run against the research project with -readOnly -noanalysis.
//@category NimbyRails
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.*;
import java.nio.file.*;
import java.util.*;

public class InspectDriving extends GhidraScript {
    public void run() throws Exception {
        Set<String> names = Set.of("max_speed", "max_acceleration", "max_regular_braking",
            "max_emergency_braking", "max_tractive_effort", "total_power", "empty_mass",
            "length", "max_pax", "dynamics", "as_purchased", "result", "train_distance");
        StringBuilder report = new StringBuilder("SHA256=" + currentProgram.getExecutableSHA256() + "\n");
        var listing = currentProgram.getListing();
        var data = listing.getDefinedData(true);
        while (data.hasNext()) {
            monitor.checkCancelled();
            Data value = data.next();
            if (!(value.getValue() instanceof String) || !names.contains(value.getValue())) continue;
            var refs = currentProgram.getReferenceManager().getReferencesTo(value.getAddress());
            while (refs.hasNext()) {
                var ref = refs.next();
                var function = getFunctionContaining(ref.getFromAddress());
                if (function == null || !function.getEntryPoint().equals(toAddr("140428790"))) continue;
                report.append("\nFIELD " + value.getValue() + " at " + ref.getFromAddress() + "\n");
                var instruction = listing.getInstructionAt(ref.getFromAddress());
                for (int i = 0; instruction != null && i < 8; ++i) instruction = instruction.getPrevious();
                for (int i = 0; instruction != null && i < 24; ++i, instruction = instruction.getNext())
                    report.append(instruction.getAddress() + " " + instruction + "\n");
            }
        }
        Files.writeString(Path.of(getScriptArgs()[0]), report);
    }
}
