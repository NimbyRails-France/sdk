// Static evidence only. Run on a private copy with -readOnly -noanalysis.
//@category NimbyRails
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.scalar.Scalar;
import java.nio.file.*;
import java.util.*;

/**
 * Export static construction evidence without changing inferred types.
 * Arguments: output directory, then VA, data:VA, vtable:name,
 * symbols:fragment or offset:hex:startVA:endVA. All addresses are Ghidra VAs.
 * Offset searches exclude RSP/RBP heuristically; absence is not evidence that
 * a field is unused. Decompiled prototypes are not callable ABI declarations.
 */
public class InspectConstruction extends GhidraScript {
    public void run() throws Exception {
        String expected="fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae";
        if(!expected.equalsIgnoreCase(currentProgram.getExecutableSHA256()))
            throw new IllegalArgumentException("Unqualified binary: no address-based export");
        String[] args=getScriptArgs();
        if(args.length<2)throw new IllegalArgumentException("Expected output directory and at least one selector");
        Path output=Path.of(args[0]);Files.createDirectories(output);
        Set<Function> functions=new LinkedHashSet<>();
        StringBuilder references=new StringBuilder();
        for(int i=1;i<args.length;i++) {
            if(args[i].startsWith("symbols:")) {
                String match=args[i].substring(8).toLowerCase(Locale.ROOT);
                for(var symbol:currentProgram.getSymbolTable().getAllSymbols(true)) {
                    String name=symbol.getName(true);
                    if(name.toLowerCase(Locale.ROOT).contains(match))
                        references.append("symbol "+symbol.getAddress()+" "+name+"\n");
                }
                continue;
            }
            // offset:hex:startVA:endVA lists candidates only. Do not decompile
            // thousands of unrelated accesses or infer an object from an offset.
            if(args[i].startsWith("offset:")) {
                String[] parts=args[i].split(":");
                if(parts.length!=4)throw new IllegalArgumentException("Expected offset:hex:startVA:endVA");
                long offset=Long.parseUnsignedLong(parts[1],16);
                var start=toAddr(parts[2]);var end=toAddr(parts[3]);
                if(start.compareTo(end)>0)throw new IllegalArgumentException("Reversed scan range");
                for(var instruction:currentProgram.getListing().getInstructions(start,true)) {
                    if(instruction.getAddress().compareTo(end)>0)break;
                    monitor.checkCancelled();
                    if(instruction.toString().contains("RSP")||instruction.toString().contains("RBP"))continue;
                    boolean match=false;
                    for(int operand=0;operand<instruction.getNumOperands();operand++)
                        for(Object object:instruction.getOpObjects(operand))
                            if(object instanceof Scalar && ((Scalar)object).getUnsignedValue()==offset)match=true;
                    if(match) {
                        Function function=getFunctionContaining(instruction.getAddress());
                        references.append("offset "+parts[1]+" "+instruction.getAddress()+" "+instruction+
                            " function "+(function==null?"none":function.getEntryPoint())+"\n");
                    }
                }
                continue;
            }
            if(args[i].startsWith("vtable:")) {
                String match=args[i].substring(7);
                for(var symbol:currentProgram.getSymbolTable().getAllSymbols(true)) {
                    String symbolName=symbol.getName(true);
                    if(!symbolName.contains(match)||!symbolName.endsWith("::vftable"))continue;
                    references.append(symbolName+" "+symbol.getAddress()+"\n");
                    for(int slot=0;slot<16;slot++) {
                        long target=getLong(symbol.getAddress().add(slot*8));
                        Function function=getFunctionAt(toAddr(target));
                        if(function==null)break;
                        references.append("slot "+slot*8+" -> "+function.getEntryPoint()+"\n");
                        functions.add(function);
                    }
                }
                continue;
            }
            boolean data=args[i].startsWith("data:");
            var address=toAddr(data?args[i].substring(5):args[i]);
            if(!data) {
                Function function=getFunctionContaining(address);
                if(function==null)throw new IllegalArgumentException("No analyzed function at "+address);
                functions.add(function);
            } else for(var ref:getReferencesTo(address)) {
                Function caller=getFunctionContaining(ref.getFromAddress());
                references.append(address+" <- "+ref.getFromAddress()+" "+ref.getReferenceType()+" caller "+
                    (caller==null?"none":caller.getEntryPoint())+"\n");
                if(caller!=null)functions.add(caller);
            }
        }
        if(functions.size()>64)throw new IllegalArgumentException("Bounded export exceeded; narrow the requested references");
        Files.writeString(output.resolve("invocations.txt"),String.join(" ",Arrays.copyOfRange(args,1,args.length))+"\n",
            StandardOpenOption.CREATE,StandardOpenOption.APPEND);
        Files.writeString(output.resolve("references.txt"),references,StandardOpenOption.CREATE,StandardOpenOption.APPEND);
        Files.writeString(output.resolve("provenance.txt"),"SHA256="+expected+"\nimageBase="+currentProgram.getImageBase()+
            "\nGhidra="+getGhidraVersion()+"\nInferred signatures are not runtime-validated.\n");
        DecompInterface decompiler=new DecompInterface();
        try {
            decompiler.openProgram(currentProgram);
            for(Function function:functions) {
                monitor.checkCancelled();
                String name=function.getEntryPoint().toString();
                println("Export "+name+" "+function.getName());
                var result=decompiler.decompileFunction(function,60,monitor);
                Files.writeString(output.resolve(name+".c"),result.decompileCompleted()?
                    result.getDecompiledFunction().getC():"// Decompilation failed: "+result.getErrorMessage());
                StringBuilder edges=new StringBuilder(),assembly=new StringBuilder();
                for(var ref:getReferencesTo(function.getEntryPoint())) {
                    Function caller=getFunctionContaining(ref.getFromAddress());
                    edges.append("caller "+ref.getFromAddress()+" "+ref.getReferenceType()+" "+(caller==null?"none":caller.getEntryPoint())+"\n");
                }
                for(Function called:function.getCalledFunctions(monitor))edges.append("calls "+called.getEntryPoint()+" "+called.getName()+"\n");
                for(var instruction:currentProgram.getListing().getInstructions(function.getBody(),true))
                    assembly.append(instruction.getAddress()+" "+instruction+"\n");
                Files.writeString(output.resolve(name+"-edges.txt"),edges);
                Files.writeString(output.resolve(name+".asm"),assembly);
            }
        } finally { decompiler.dispose(); }
    }
}
