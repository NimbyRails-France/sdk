// Static UI ABI evidence only. Run on a read-only analysis project.
//@category NimbyRails
import ghidra.app.script.GhidraScript;
import ghidra.program.model.symbol.*;
import ghidra.program.model.address.Address;
import java.nio.file.*;
import java.nio.charset.StandardCharsets;
import java.util.Locale;

public class InspectNativeUi extends GhidraScript {
    public void run() throws Exception {
        StringBuilder out = new StringBuilder();
        out.append("Binary SHA256: ").append(currentProgram.getExecutableSHA256()).append('\n');
        SymbolIterator symbols = currentProgram.getSymbolTable().getAllSymbols(true);
        while (symbols.hasNext()) {
            monitor.checkCancelled();
            Symbol symbol = symbols.next();
            String name = symbol.getName(true);
            String lower = name.toLowerCase(Locale.ROOT);
            if (!(lower.contains("nimby::shell::ui::") && !lower.contains("std::"))) continue;
            out.append(symbol.getAddress()).append(' ').append(name).append('\n');
            if (!lower.endsWith("::vftable")) continue;
            for (int offset = 0; offset <= 0x108; offset += 8) {
                try {
                    Address slot = symbol.getAddress().add(offset);
                    long value = getLong(slot);
                    Address target = toAddr(value);
                    out.append(String.format("  +%03x -> %s", offset, target));
                    var function = getFunctionAt(target);
                    if (function != null) out.append(' ').append(function.getName(true));
                    out.append('\n');
                } catch (Exception error) { out.append("  unreadable slot\n"); break; }
            }
        }
        Files.writeString(Path.of(getScriptArgs()[0]), out, StandardCharsets.UTF_8);
        println("Native UI symbols exported");
    }
}
