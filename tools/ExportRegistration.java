//@category NimbyRails
import ghidra.app.script.GhidraScript;
import java.nio.file.*;
public class ExportRegistration extends GhidraScript {
 public void run() throws Exception {
  StringBuilder text=new StringBuilder();
  var function=getFunctionAt(toAddr("140428790"));
  var instructions=currentProgram.getListing().getInstructions(function.getBody(),true);
  while(instructions.hasNext()) {
   var instruction=instructions.next(); text.append(instruction.getAddress()+" "+instruction+" ");
   for(var ref: instruction.getReferencesFrom()) {
    var data=getDataAt(ref.getToAddress());
    if(data!=null && data.getValue() instanceof String) text.append(" STRING="+data.getValue());
   }
   text.append("\n");
  }
  Files.writeString(Path.of(getScriptArgs()[0]),text);
 }
}
