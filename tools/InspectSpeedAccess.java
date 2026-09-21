//@category NimbyRails
// Static candidate search: an offset match does not establish an object's type.
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.listing.*;
import java.nio.file.*;
import java.util.*;
public class InspectSpeedAccess extends GhidraScript {
 public void run() throws Exception {
  Path dir=Path.of(getScriptArgs()[0]); Files.createDirectories(dir);
  StringBuilder report=new StringBuilder("SHA256="+currentProgram.getExecutableSHA256()+"\n");
  Set<Function> candidates=new LinkedHashSet<>();
  for(Instruction i:currentProgram.getListing().getInstructions(true)) {
   monitor.checkCancelled(); String s=i.toString().toLowerCase(Locale.ROOT);
   if(!s.contains("0x3c8") || s.contains("rsp") || s.contains("rbp")) continue;
   Function f=getFunctionContaining(i.getAddress());
   report.append(i.getAddress()+" "+s+" function="+(f==null?"none":f.getEntryPoint())+"\n");
   if(f!=null && s.startsWith("movsd") && s.indexOf("[")<s.indexOf(",")) candidates.add(f);
  }
  Files.writeString(dir.resolve("speed-access.txt"),report);
  DecompInterface d=new DecompInterface();d.openProgram(currentProgram);
  try { for(Function f:candidates) {
   var r=d.decompileFunction(f,30,monitor);
   if(r.decompileCompleted())Files.writeString(dir.resolve(f.getEntryPoint()+".c"),r.getDecompiledFunction().getC());
   StringBuilder callers=new StringBuilder();
   for(var ref:getReferencesTo(f.getEntryPoint())){Function c=getFunctionContaining(ref.getFromAddress());callers.append(ref.getFromAddress()+" "+(c==null?"none":c.getEntryPoint())+"\n");}
   Files.writeString(dir.resolve(f.getEntryPoint()+"-callers.txt"),callers);
  }} finally {d.dispose();}
 }
}
