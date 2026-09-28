// Read-only analysis of metadata parsing and display; never a hook approval.
//@category NimbyRails
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.listing.*;
import java.nio.file.*;
import java.util.*;
public class InspectModMetadata extends GhidraScript {
 public void run() throws Exception {
  Path dir=Path.of(getScriptArgs()[0]); Files.createDirectories(dir);
  StringBuilder out=new StringBuilder("SHA256 "+currentProgram.getExecutableSHA256()+"\n");
  Set<Function> functions=new LinkedHashSet<>();
  var data=currentProgram.getListing().getDefinedData(true);
  while(data.hasNext()) {
   monitor.checkCancelled(); var d=data.next(); if(!(d.getValue() instanceof String))continue;
   String s=(String)d.getValue(); String lower=s.toLowerCase(Locale.ROOT);
   if(!(s.equals("ModMeta") || lower.contains("mod.txt") || lower.contains("modmeta") || lower.equals("name_loc") || lower.equals("desc_loc") || lower.contains("mods::") || lower.contains("mod_manager")))continue;
   out.append(d.getAddress()+" "+s+"\n");
   var refs=currentProgram.getReferenceManager().getReferencesTo(d.getAddress());
   while(refs.hasNext()) {var ref=refs.next();var f=getFunctionContaining(ref.getFromAddress());out.append("  "+ref+" function="+(f==null?"none":f.getEntryPoint())+"\n");if(f!=null&&s.equals("ModMeta"))functions.add(f);}
  }
  var symbols=currentProgram.getSymbolTable().getAllSymbols(true);
  while(symbols.hasNext()){var symbol=symbols.next();String name=symbol.getName(true);if(name.contains("nimby::")&&name.toLowerCase(Locale.ROOT).contains("mod"))out.append(symbol.getAddress()+" "+name+"\n");}
  Files.writeString(dir.resolve("references.txt"),out);
  DecompInterface decompiler=new DecompInterface();decompiler.openProgram(currentProgram);
  try{for(Function f:functions){var result=decompiler.decompileFunction(f,90,monitor);Files.writeString(dir.resolve(f.getEntryPoint()+".c"),result.decompileCompleted()?result.getDecompiledFunction().getC():result.getErrorMessage());}}finally{decompiler.dispose();}
 }
}
