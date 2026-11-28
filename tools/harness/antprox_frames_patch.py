"""T2 (2026-11-28, SP-06 P2 / B-31): a diagnostic antprox-rs build for the warp-start jump. Local build only, never
committed to AntaresProxy-Rust: patch a clone of it at the store's antprox-rs commit (baselines/tools/antprox/
manifest.json, 9601958), then `cargo build --release` (~40 s).

    python tools/harness/antprox_frames_patch.py CLONE [--map build/programs/client/client.map]

What the build does differently, each by an environment variable of the run:
- ANTPROX_DUMP_OPCODES=0012,0013,0014,009b,003e: the verbose 'Sending [0x....] via UDP' (client to server) and
  '<SERVER TO CLIENT UDP> ----> OPCODE [LEN]' lines get ', N bytes: HEX' for those opcodes (T1's udp_link.rs patch,
  2026-11-27, and the same for downstream.rs). e2e.py's _LIVE_OP still matches the lines' start.
- ANTPROX_NO_CLIENT_FRAMES=1: RS21's input placement at the client's own frames is off for the retail build too
  (session.rs position_source): the authoritative integrator places retail's inputs by the wire's estimate, as it
  does for any build it does not know (ours).
- ANTPROX_OURS_FRAMES=1: RS21's input placement for our one client build: the sampler's retail addresses
  (sampler.rs: the kernel instance, the ClientTask / SClient / type-4 state / extrapolator vtables, 0xBE71D4) are
  swapped for those --map gives (our link map; the classes' layouts are the client's, so +0x10BC, +0x112C, +0x98,
  +0x1C are the same). The build is then good for that client.exe only: re-patch after a relink.
  The frame reader prints 'T2 frame reader: N reads, M misses' to stderr (antprox.out) at 1, 100, 1000, 10000.
"""
import argparse, re, sys

SAMPLER = 'crates/antprox/src/net7/position_integrated/sampler.rs'
SESSION = 'crates/antprox/src/net7/session.rs'
UDP_LINK = 'crates/antprox/src/net7/udp_link.rs'
DOWNSTREAM = 'crates/antprox/src/net7/downstream.rs'
# sampler.rs constant -> the symbol in our link map
SYMBOLS = {
    'KERNEL_INSTANCE': '?Instance@KernelClass@@2PAV1@A',
    'CLIENT_TASK_VTABLE': '??_7ClientTaskClass@@6B@',
    'SCLIENT_VTABLE': '??_7SpaceClientClass@@6BClientClass@@@',
    'BLEND_WINDOW': '?Unknown_BE71D4@@3IA',
    'TYPE4_VTABLE': '??_7Unknown717010Class@@6B@',
    'CROSSFADE_VTABLE': '??_7Unknown7125F0Class@@6B@',
    'SNAPSHOT_VTABLE': '??_7Unknown712DB0Class@@6B@',
    'ADVANCED_NODE_VTABLE': '??_7Unknown710110Class@@6B@',
    'MOTION_VTABLE': '??_7Unknown710530Class@@6B@',
}


def edit(path, old, new):
    src = open(path, encoding='utf-8').read()
    if new in src:
        return
    assert src.count(old) == 1, f'anchor not unique in {path}: {old[:60]!r}'
    open(path, 'w', encoding='utf-8', newline='\n').write(src.replace(old, new))


def dump(name, fmt_plain, fmt_dump, indent):
    i = ' ' * indent
    return (f'{i}state.link.verbose(|| {{\n' if name == 'down' else f'{i}self.verbose(|| {{\n') + (
        f'{i}    let dump = std::env::var("ANTPROX_DUMP_OPCODES").unwrap_or_default();\n'
        f'{i}    let want = dump.split(\',\').any(|o| u16::from_str_radix(o.trim(), 16).ok() == Some(opcode));\n'
        f'{i}    if want {{\n'
        f'{i}        let hex: String = payload.iter().map(|b| format!("{{:02x}}", b)).collect();\n'
        f'{i}        {fmt_dump}\n'
        f'{i}    }} else {{\n'
        f'{i}        {fmt_plain}\n'
        f'{i}    }}\n'
        f'{i}}});\n')


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('clone')
    ap.add_argument('--map', default='build/programs/client/client.map')
    a = ap.parse_args()
    root = a.clone.rstrip('/\\') + '/'
    # the dumps
    edit(root + UDP_LINK, '        self.verbose(|| format!("Sending [0x{:04x}] via UDP", opcode));\n',
         dump('up', 'format!("Sending [0x{:04x}] via UDP", opcode)',
              'format!("Sending [0x{:04x}] via UDP, {} bytes: {}", opcode, payload.len(), hex)', 8))
    edit(root + DOWNSTREAM,
         '            state.link.verbose(|| format!("<SERVER TO CLIENT UDP> ----> {:04x} [{:04x}]", opcode, length));\n',
         dump('down', 'format!("<SERVER TO CLIENT UDP> ----> {:04x} [{:04x}]", opcode, length)',
              'format!("<SERVER TO CLIENT UDP> ----> {:04x} [{:04x}], {} bytes: {}", opcode, length, payload.len(), hex)',
              12))
    # the two switches
    edit(root + SESSION, '    let client_frames = integrates && layout.is_some() && !options.net7.no_patches;\n',
         '    let client_frames = integrates && (layout.is_some() || std::env::var("ANTPROX_OURS_FRAMES").is_ok())\n'
         '        && !options.net7.no_patches && std::env::var("ANTPROX_NO_CLIENT_FRAMES").is_err();\n')
    # our addresses
    addr = {}
    for line in open(a.map, errors='replace'):
        p = line.split()
        if len(p) >= 3 and p[1] in SYMBOLS.values():
            addr[p[1]] = int(p[2], 16)
    missing = [s for s in SYMBOLS.values() if s not in addr]
    if missing:
        sys.exit(f'not in {a.map}: {missing}')
    src = open(root + SAMPLER, encoding='utf-8').read()
    for name, sym in SYMBOLS.items():
        m = re.search(r'^const ' + name + r': u32 = (0x[0-9A-Fa-f_]+);\n', src, re.M)
        if not m:
            continue  # patched before
        retail = m.group(1)
        src = src.replace(m.group(0), '')
        src = re.sub(r'\b' + name + r'\b', name + '()', src)
        src += (f'\n#[allow(non_snake_case)]\nfn {name}() -> u32 {{\n'
                f'    if std::env::var("ANTPROX_OURS_FRAMES").is_ok() {{ 0x{addr[sym]:08X} }} else {{ {retail} }}\n}}\n')
    old = ('            Some(_) => {\n'
           '                self.reads += 1;\n')
    if old in src:
        src = src.replace(old, (
            '            Some(r) => {\n'
            '                self.reads += 1;\n'
            '                if matches!(self.reads, 1 | 100 | 1000 | 10000) {\n'
            '                    eprintln!("T2 frame reader: {} reads, {} misses (tick {}, MovementID {})", self.reads, self.misses, r.tick, r.movement_id);\n'
            '                }\n'))
        src = src.replace(
            '            None => {\n'
            '                self.misses += 1;\n',
            '            None => {\n'
            '                self.misses += 1;\n'
            '                if matches!(self.misses, 1 | 100 | 1000) {\n'
            '                    eprintln!("T2 frame reader: {} misses, {} reads", self.misses, self.reads);\n'
            '                }\n', 1)
    open(root + SAMPLER, 'w', encoding='utf-8', newline='\n').write(src)
    print('patched:', ', '.join(f'{n}=0x{addr[s]:08X}' for n, s in SYMBOLS.items()))


if __name__ == '__main__':
    main()
