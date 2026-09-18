//@category NimbyRails
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.listing.*;
import java.nio.file.*;
import java.util.*;
public class InspectTextureCommands extends GhidraScript {
 public void run() throws Exception {
  Path dir=Path.of(getScriptArgs()[0]);Files.createDirectories(dir);
  Set<Function> funcs=new LinkedHashSet<>();StringBuilder refs=new StringBuilder();
  for(Data item:currentProgram.getListing().getDefinedData(true)) {
   Object value=item.getValue();if(!(value instanceof String))continue;
   String s=(String)value;if(!s.contains("event_signal_texture_state"))continue;
   for(var ref:getReferencesTo(item.getAddress())) {
    Function f=getFunctionContaining(ref.getFromAddress());
    refs.append(s+" "+ref.getFromAddress()+" "+(f==null?"none":f.getEntryPoint())+"\n");
    if(f!=null)funcs.add(f);
   }
  }
  // Existing validated texture selection, and direct callers of event functions.
  for(String address:Arrays.copyOfRange(getScriptArgs(),1,getScriptArgs().length)) {
   Function f=getFunctionAt(toAddr(address));if(f!=null)funcs.add(f);
  }
  Files.writeString(dir.resolve("references.txt"),refs);
  var decompiler=new DecompInterface();decompiler.openProgram(currentProgram);
  try {for(Function f:funcs) {
   var result=decompiler.decompileFunction(f,60,monitor);
   if(result.decompileCompleted())Files.writeString(dir.resolve(f.getEntryPoint()+".c"),result.getDecompiledFunction().getC());
   StringBuilder callers=new StringBuilder();
   for(var ref:getReferencesTo(f.getEntryPoint())) {Function caller=getFunctionContaining(ref.getFromAddress());callers.append(ref.getFromAddress()+" "+(caller==null?"none":caller.getEntryPoint())+"\n");}
   Files.writeString(dir.resolve(f.getEntryPoint()+"-callers.txt"),callers);
   StringBuilder assembly=new StringBuilder();
   for(var instruction:currentProgram.getListing().getInstructions(f.getBody(),true))
    assembly.append(instruction.getAddress()+" "+instruction+"\n");
   Files.writeString(dir.resolve(f.getEntryPoint()+".asm"),assembly);
  }} finally {decompiler.dispose();}
 }
}
