// Read-only native intervention evidence, not an activation approval.
//@category NimbyRails
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;
import java.nio.file.*;
import java.util.*;
public class InspectIntervention extends GhidraScript {
    public void run() throws Exception {
        Path dir = Path.of(getScriptArgs()[0]); Files.createDirectories(dir);
        StringBuilder symbols = new StringBuilder();
        Set<Function> funcs = new LinkedHashSet<>();
        for (String arg : Arrays.copyOfRange(getScriptArgs(),1,getScriptArgs().length)) {
            Function f = getFunctionContaining(toAddr(arg)); if(f != null) funcs.add(f);
        }
        for (Symbol s : currentProgram.getSymbolTable().getAllSymbols(true)) {
            String name = s.getName(true);
            if (!(name.contains("TrainIntervention") || name.contains("train::Intervention"))) continue;
            symbols.append(name).append(" @ ").append(s.getAddress()).append("\n");
            if(name.endsWith("vftable")) for(int i=0;i<12;i++) {
                long ptr = getLong(s.getAddress().add(i*8));
                Function f = getFunctionAt(toAddr(ptr));
                symbols.append(i*8).append(" -> ").append(Long.toHexString(ptr)).append("\n");
                if(f != null) funcs.add(f); else break;
            }
        }
        Files.writeString(dir.resolve("symbols.txt"),symbols);
        DecompInterface d = new DecompInterface();
        try {
            d.openProgram(currentProgram);
            for(Function f:funcs) {
                DecompileResults r=d.decompileFunction(f,90,monitor);
                StringBuilder out=new StringBuilder();
                if(r.decompileCompleted())out.append(r.getDecompiledFunction().getC());
                else out.append(r.getErrorMessage());
                for(var it=currentProgram.getReferenceManager().getReferencesTo(f.getEntryPoint());it.hasNext();) {
                    var ref=it.next(); var caller=getFunctionContaining(ref.getFromAddress());
                    out.append("\n// ref ").append(ref.getFromAddress()).append(" caller ").append(caller==null?"none":caller.getEntryPoint());
                }
                Files.writeString(dir.resolve(f.getEntryPoint()+".c"),out);
            }
        } finally {d.dispose();}
    }
}
