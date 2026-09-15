//@category NimbyRails
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.scalar.Scalar;
import java.nio.file.*;
import java.util.*;
public class InspectSignalState extends GhidraScript {
 public void run() throws Exception {
  Path dir=Path.of(getScriptArgs()[0]);Files.createDirectories(dir);
  boolean kind=getScriptArgs().length>1 && getScriptArgs()[1].equals("kind");
  Set<Function> found=new LinkedHashSet<>();StringBuilder matches=new StringBuilder();
  for(Instruction ins:currentProgram.getListing().getInstructions(true)) {
   for(int op=0;op<ins.getNumOperands();op++)for(Object obj:ins.getOpObjects(op)) {
    if(obj instanceof Scalar && (kind
       ? ins.getMnemonicString().equals("CMP") && ins.toString().contains("+ 0x30]") && ((Scalar)obj).getUnsignedValue()==4
       : ((Scalar)obj).getUnsignedValue()==0x1180)) {
     Function f=getFunctionContaining(ins.getAddress());
     if(f!=null){found.add(f);matches.append(ins.getAddress()+" "+ins+" function="+f.getEntryPoint()+"\n");}
    }
   }
  }
  Files.writeString(dir.resolve("event-offset-references.txt"),matches);
  DecompInterface d=new DecompInterface();d.openProgram(currentProgram);
  try{for(Function f:found){var r=d.decompileFunction(f,45,monitor);if(r.decompileCompleted())Files.writeString(dir.resolve(f.getEntryPoint()+".c"),r.getDecompiledFunction().getC());}}finally{d.dispose();}
 }
}
