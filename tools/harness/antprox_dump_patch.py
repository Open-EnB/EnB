"""T1 (2026-11-27): a diagnostic antprox-rs build for UI-08 / B-32. The verbose 'Sending [0x....] via UDP' line
gets ', N bytes: HEX' appended for the opcodes named in ANTPROX_DUMP_OPCODES (comma-separated hex, e.g. 00a3).
e2e.py's _LIVE_OP still matches the line's start. Local build only (never committed to AntaresProxy-Rust)."""
import sys

path = sys.argv[1]
src = open(path, encoding='utf-8').read()
old = '        self.verbose(|| format!("Sending [0x{:04x}] via UDP", opcode));\n'
new = ('        self.verbose(|| {\n'
       '            let dump = std::env::var("ANTPROX_DUMP_OPCODES").unwrap_or_default();\n'
       '            let want = dump.split(\',\').any(|o| u16::from_str_radix(o.trim(), 16).ok() == Some(opcode));\n'
       '            if want {\n'
       '                let hex: String = payload.iter().map(|b| format!("{:02x}", b)).collect();\n'
       '                format!("Sending [0x{:04x}] via UDP, {} bytes: {}", opcode, payload.len(), hex)\n'
       '            } else {\n'
       '                format!("Sending [0x{:04x}] via UDP", opcode)\n'
       '            }\n'
       '        });\n')
assert src.count(old) == 1, 'anchor'
open(path, 'w', encoding='utf-8', newline='\n').write(src.replace(old, new))
print('patched')
