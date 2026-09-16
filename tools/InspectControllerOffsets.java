//@category NimbyRails
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.scalar.Scalar;
import java.nio.file.*;
import java.util.*;
public class InspectControllerOffsets extends GhidraScript {
 public void run() throws Exception {
  Path dir=Path.of(getScriptArgs()[0]);Files.createDirectories(dir);
  Set<Function> funcs=new LinkedHashSet<>();StringBuilder hits=new StringBuilder();
  var iter=currentProgram.getListing().getInstructions(true);
  while(iter.hasNext()){
   var ins=iter.next();long va=ins.getAddress().getOffset();
   if(!ins.getMnemonicString().equals("LEA") || ins.toString().contains("RSP") || ins.toString().contains("RBP"))continue;
   for(int i=0;i<ins.getNumOperands();i++)for(Object o:ins.getOpObjects(i))
    if(o instanceof Scalar){long v=((Scalar)o).getUnsignedValue();if(v==0x58){
     Function f=getFunctionContaining(ins.getAddress());hits.append(ins.getAddress()+" "+ins+" caller "+(f==null?"none":f.getEntryPoint())+"\n");
    }}
  }
  Files.writeString(dir.resolve("offsets.txt"),hits.toString());
  var decompiler=new DecompInterface();decompiler.openProgram(currentProgram);
  try {for(Function f:funcs){var result=decompiler.decompileFunction(f,60,monitor);if(result.decompileCompleted())Files.writeString(dir.resolve(f.getEntryPoint()+".c"),result.getDecompiledFunction().getC());}}finally{decompiler.dispose();}
 }
}
