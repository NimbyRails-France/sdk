//@category NimbyRails
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.listing.*;
import java.nio.file.*;
import java.util.*;
public class InspectSignalFilters extends GhidraScript {
 public void run() throws Exception {
  Path dir=Path.of(getScriptArgs()[0]);Files.createDirectories(dir);
  Set<Function> funcs=new LinkedHashSet<>();StringBuilder refs=new StringBuilder();
  for(Data item:currentProgram.getListing().getDefinedData(true)) {
   Object value=item.getValue();if(!(value instanceof String))continue;
   String text=(String)value;if(!text.startsWith("signal_filter_"))continue;
   for(var ref:getReferencesTo(item.getAddress())) {
    Function f=getFunctionContaining(ref.getFromAddress());
    refs.append(text+" "+ref.getFromAddress()+" "+(f==null?"none":f.getEntryPoint())+"\n");
    if(f!=null)funcs.add(f);
   }
  }
  Files.writeString(dir.resolve("references.txt"),refs);
  var d=new DecompInterface();d.openProgram(currentProgram);
  try{for(Function f:funcs){var r=d.decompileFunction(f,45,monitor);if(r.decompileCompleted())Files.writeString(dir.resolve(f.getEntryPoint()+".c"),r.getDecompiledFunction().getC());}}finally{d.dispose();}
 }
}
