// Regenerate only after reviewing the SDL binary shipped with the game.
import fs from 'node:fs';
import { inspect } from './inspect-pe.mjs';
const [original, output] = process.argv.slice(2);
const pe = inspect(original);
if (pe.machine !== '0x8664') throw Error('AMD64 SDL required');
const entries = pe.exports.map(e => {
    if (!/^[A-Za-z_][A-Za-z_0-9]*$/.test(e.name)) throw Error('Unsupported export name');
    const destination = e.name === 'SDL_Init' ? 'Proxy_SDL_Init' :
        e.name === 'SDL_Quit' ? 'Proxy_SDL_Quit' : `NimbyRailsSDL3Original.${e.name}`;
    return `    ${e.name}=${destination} @${e.ordinal}`;
});
fs.writeFileSync(output, `; Generated from SDL3.dll SHA256 ${pe.sha256}\nLIBRARY SDL3\nEXPORTS\n${entries.join('\n')}\n`);
