//@category NimbyRails
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.scalar.Scalar;
import java.nio.file.*;
import java.util.*;
public class InspectJunctions extends GhidraScript {
 public void run() throws Exception {
 Path dir=Path.of(getScriptArgs()[0]);Files.createDirectories(dir);
 Set<Function> funcs=new LinkedHashSet<>();StringBuilder hits=new StringBuilder();
 var iter=currentProgram.getListing().getInstructions(true);
 while(iter.hasNext()){var ins=iter.next();long va=ins.getAddress().getOffset();
 if(va<0x140350000L||va>=0x140428000L)continue;
 for(int i=0;i<ins.getNumOperands();i++)for(Object o:ins.getOpObjects(i))if(o instanceof Scalar){long v=((Scalar)o).getUnsignedValue();if(v==0x3f0){Function f=getFunctionContaining(ins.getAddress());hits.append(ins.getAddress()+" "+ins+" caller "+(f==null?"none":f.getEntryPoint())+"\n");if(f!=null)funcs.add(f);}}
 }
 Files.writeString(dir.resolve("offsets.txt"),hits.toString());
 var d=new DecompInterface();d.openProgram(currentProgram);try{for(Function f:funcs){var r=d.decompileFunction(f,30,monitor);if(r.decompileCompleted())Files.writeString(dir.resolve(f.getEntryPoint()+".c"),r.getDecompiledFunction().getC());}}finally{d.dispose();}
 }
}
