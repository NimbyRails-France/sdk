//@category NimbyRails
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;
import java.nio.file.*;
import java.util.*;
public class InspectAnchor extends GhidraScript {
 public void run() throws Exception {
  String[] args=getScriptArgs(); Path dir=Path.of(args[0]); Files.createDirectories(dir);
  Set<Function> functions=new LinkedHashSet<>(); StringBuilder refs=new StringBuilder();
  for(int i=1;i<args.length;i++) {
   var address=toAddr(args[i]); refs.append("Anchor "+address+" RVA "+Long.toHexString(address.subtract(currentProgram.getImageBase()))+"\n");
   Function direct=getFunctionAt(address); if(direct!=null) functions.add(direct);
   var iterator=currentProgram.getReferenceManager().getReferencesTo(address);
   while(iterator.hasNext()) { var ref=iterator.next(); Function f=getFunctionContaining(ref.getFromAddress()); refs.append(ref+" function="+(f==null?"none":f.getEntryPoint())+"\n"); if(f!=null)functions.add(f); }
   for(int n=-1;n<8;n++) refs.append("qword "+address.add(n*8)+" = "+Long.toHexString(getLong(address.add(n*8)))+"\n");
  }
  Files.writeString(dir.resolve("anchors.txt"),refs);
  DecompInterface d=new DecompInterface(); d.openProgram(currentProgram);
  try { for(Function f:functions) { var result=d.decompileFunction(f,90,monitor); Files.writeString(dir.resolve(f.getEntryPoint()+".c"), result.decompileCompleted()?result.getDecompiledFunction().getC():result.getErrorMessage()); } } finally {d.dispose();}
 }
}
