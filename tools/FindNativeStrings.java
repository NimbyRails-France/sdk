// Read-only discovery: string references do not establish object layouts.
//@category NimbyRails
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.*;
import java.nio.file.*;
import java.nio.charset.StandardCharsets;
import java.util.Locale;

public class FindNativeStrings extends GhidraScript {
    public void run() throws Exception {
        String[] args=getScriptArgs();
        if(args.length<2)throw new IllegalArgumentException("Output path and search terms required");
        StringBuilder out=new StringBuilder("SHA256: "+currentProgram.getExecutableSHA256()+"\n");
        DataIterator data=currentProgram.getListing().getDefinedData(true);
        while(data.hasNext()){
            monitor.checkCancelled();
            Data item=data.next();
            if(!(item.getValue() instanceof String))continue;
            String value=(String)item.getValue(),lower=value.toLowerCase(Locale.ROOT);
            boolean matches=false;
            for(int i=1;i<args.length;i++)if(lower.contains(args[i].toLowerCase(Locale.ROOT)))matches=true;
            if(!matches)continue;
            out.append(item.getAddress()).append(' ').append(value.replace('\n',' ')).append('\n');
            var references=currentProgram.getReferenceManager().getReferencesTo(item.getAddress());
            while(references.hasNext()){
                var ref=references.next();
                Function caller=getFunctionContaining(ref.getFromAddress());
                out.append("  ").append(ref.getFromAddress()).append(" caller ")
                    .append(caller==null?"none":caller.getEntryPoint()).append('\n');
            }
        }
        var symbols=currentProgram.getSymbolTable().getAllSymbols(true);
        while(symbols.hasNext()){
            monitor.checkCancelled();
            var symbol=symbols.next();
            String name=symbol.getName(true),lower=name.toLowerCase(Locale.ROOT);
            boolean matches=false;
            for(int i=1;i<args.length;i++)if(lower.contains(args[i].toLowerCase(Locale.ROOT)))matches=true;
            if(!matches||name.startsWith("s_"))continue;
            out.append("SYMBOL ").append(symbol.getAddress()).append(' ').append(name).append('\n');
            var references=currentProgram.getReferenceManager().getReferencesTo(symbol.getAddress());
            while(references.hasNext()){
                var ref=references.next();
                var caller=getFunctionContaining(ref.getFromAddress());
                out.append("  ").append(ref.getFromAddress()).append(" caller ")
                    .append(caller==null?"none":caller.getEntryPoint()).append('\n');
            }
        }
        Files.writeString(Path.of(args[0]),out,StandardCharsets.UTF_8);
        println("Native strings exported");
    }
}
