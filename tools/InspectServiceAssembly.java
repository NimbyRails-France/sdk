//@category NimbyRails
import ghidra.app.script.GhidraScript;
import java.nio.file.*;
public class InspectServiceAssembly extends GhidraScript {
 public void run() throws Exception {
  String[] args=getScriptArgs();StringBuilder out=new StringBuilder();
  for(int i=1;i<args.length;i++) {
   var address=toAddr(args[i]);out.append("\n"+address+"\n");
   var ins=currentProgram.getListing().getInstructionAt(address);
   if(ins==null)ins=currentProgram.getListing().getInstructionAfter(address);
   for(int n=0;n<65&&ins!=null;n++,ins=ins.getNext())out.append(ins.getAddress()+" "+ins+"\n");
  }
  Files.writeString(Path.of(args[0]),out);
 }
}
