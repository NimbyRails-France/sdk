// Read-only PE inventory. Run: node inspect-pe.mjs <exe> <report.json>
import fs from 'node:fs';
import crypto from 'node:crypto';
import { pathToFileURL } from 'node:url';
export function inspect(path) {
  const b = fs.readFileSync(path);
  const u16 = o => b.readUInt16LE(o), u32 = o => b.readUInt32LE(o);
  if (u16(0) !== 0x5a4d) throw Error('Not MZ');
  const pe = u32(0x3c), opt = pe + 24;
  if (u32(pe) !== 0x4550 || u16(opt) !== 0x20b) throw Error('Not PE32+');
  const hex = n => '0x' + n.toString(16);
  const sections = [];
  for (let i = 0; i < u16(pe + 6); i++) {
    const o = opt + u16(pe + 20) + 40 * i;
    sections.push({name: b.toString('ascii', o, o+8).replace(/\0.*/, ''), rva:u32(o+12), virtual_size:u32(o+8), raw_size:u32(o+16), raw_offset:u32(o+20), characteristics:hex(u32(o+36))});
  }
  function offset(rva) {
    if (rva < u32(opt+60) && rva < b.length) return rva;
    const s = sections.find(s => rva >= s.rva && rva-s.rva < s.raw_size);
    if (!s || s.raw_offset+rva-s.rva >= b.length) throw Error('Unmapped RVA '+hex(rva));
    return s.raw_offset + rva-s.rva;
  }
  function str(o) { const end=b.indexOf(0,o); if(end<0) throw Error('Unterminated string'); return b.toString('utf8',o,end); }
  const dir = i => ({rva:u32(opt+112+i*8),size:u32(opt+116+i*8)});
  const imports=[];
  const imp=dir(1);
  if (imp.rva) for(let i=0;i+20<=imp.size;i+=20) {
    const o=offset(imp.rva+i); if(!u32(o+12)) break;
    const symbols=[]; const thunk=u32(o)||u32(o+16);
    for(let j=0;j<b.length/8;j++) {
      const v=b.readBigUInt64LE(offset(thunk+j*8)); if(!v) break;
      symbols.push({name:(v>>63n) ? '#'+Number(v&65535n) : str(offset(Number(v))+2), iat_rva:hex(u32(o+16)+j*8)});
    }
    imports.push({dll:str(offset(u32(o+12))),symbols});
  }
  const exports=[]; const e=dir(0);
  if(e.rva) {
    const o=offset(e.rva), count=u32(o+24);
    for(let i=0;i<count;i++) {
      const ordinal=u16(offset(u32(o+36)+i*2));
      const rva=u32(offset(u32(o+28)+ordinal*4));
      exports.push({name:str(offset(u32(offset(u32(o+32)+i*4)))),ordinal:ordinal+u32(o+16),rva:hex(rva),forwarder:rva>=e.rva&&rva<e.rva+e.size?str(offset(rva)):null});
    }
  }
  const debug=[]; const d=dir(6);
  if(d.rva) for(let i=0;i+28<=d.size;i+=28) {
    const o=offset(d.rva+i), raw=u32(o+24), size=u32(o+16);
    debug.push({type:u32(o+12),size,codeview: size>=24 && b.toString('ascii',raw,raw+4)==='RSDS' ? {guid_bytes:b.subarray(raw+4,raw+20).toString('hex'),age:u32(raw+20),pdb_path:str(raw+24)} : null});
  }
  return {path,sha256:crypto.createHash('sha256').update(b).digest('hex').toUpperCase(),size:b.length,machine:hex(u16(pe+4)),coff_timestamp:u32(pe+8),coff_symbol_count:u32(pe+16),image_base:hex(b.readBigUInt64LE(opt+24)),entry_rva:hex(u32(opt+16)),image_size:u32(opt+56),subsystem:u16(opt+68),dll_characteristics:hex(u16(opt+70)),export_directory:dir(0),tls_directory:dir(9),delay_import_directory:dir(13),sections,imports,exports,debug};
}
if (typeof process !== 'undefined' && process.argv[1] && import.meta.url === pathToFileURL(process.argv[1]).href) {
  const result=inspect(process.argv[2]);
  fs.writeFileSync(process.argv[3],JSON.stringify(result,null,2)+'\n');
}
