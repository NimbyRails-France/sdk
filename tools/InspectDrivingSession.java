//@category NimbyRails
// Read-only research session. Each line in requests.txt is a function VA.
// Adding STOP ends the session; automatic timeout bounds unattended execution.
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.listing.*;
import java.nio.file.*;
import java.util.*;
public class InspectDrivingSession extends GhidraScript {
 public void run() throws Exception {
  Path dir=Path.of(getScriptArgs()[0]);Files.createDirectories(dir);
  Path queue=dir.resolve("requests.txt");
  Set<String> done=new HashSet<>();
  DecompInterface d=new DecompInterface();d.openProgram(currentProgram);
  Files.writeString(dir.resolve("ready.txt"),currentProgram.getExecutableSHA256());
  long until=System.currentTimeMillis()+1800000;
  try {while(System.currentTimeMillis()<until) {
   monitor.checkCancelled();
   List<String> pending=List.of();
   try {if(Files.exists(queue))pending=Files.readAllLines(queue);} catch(java.io.IOException busy) {Thread.sleep(250);continue;}
   for(String line:pending) {
    String address=line.trim();if(address.equals("STOP"))return;
    if(address.isEmpty()||!done.add(address)||Files.exists(dir.resolve(address+".c")))continue;
    Function f=getFunctionContaining(toAddr(address));
    if(f==null){Files.writeString(dir.resolve(address+".error"),"No function");continue;}
    StringBuilder asm=new StringBuilder(),refs=new StringBuilder(),bytes=new StringBuilder();
    for(var i:currentProgram.getListing().getInstructions(f.getBody(),true))asm.append(i.getAddress()+" "+i+"\n");
    for(var r:getReferencesTo(f.getEntryPoint())) {Function c=getFunctionContaining(r.getFromAddress());refs.append(r.getFromAddress()+" "+(c==null?"none":c.getEntryPoint())+"\n");}
    for(int i=0;i<32;i++)bytes.append(String.format("%02x ",getByte(f.getEntryPoint().add(i))&255));
    Files.writeString(dir.resolve(address+".asm"),asm);
    Files.writeString(dir.resolve(address+"-callers.txt"),refs);
    Files.writeString(dir.resolve(address+"-bytes.txt"),bytes);
    var result=d.decompileFunction(f,45,monitor);
    Files.writeString(dir.resolve(address+".c"),result.decompileCompleted()?result.getDecompiledFunction().getC():"// "+result.getErrorMessage());
   }
   Thread.sleep(250);
  }} finally{d.dispose();Files.writeString(dir.resolve("finished.txt"),"finished");}
 }
}
