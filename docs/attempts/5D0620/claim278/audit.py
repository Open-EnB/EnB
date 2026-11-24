from pathlib import Path
import hashlib, json, os, re, struct, sys
root=Path('/home/dev/Projects/EnB')
os.environ['ENB_RETAIL_INSTALL']='/home/dev/Games/earth-and-beyond/prefix/drive_c/Program Files (x86)/EA GAMES/Earth & Beyond'
sys.path.insert(0,str(root/'tools/re'))
from objmatch import CoffObject, symbol_addresses, object_unwind
from clientimage import ClientImage
w=root/'.work/claim-278/5D0620'
folder=w/'runs/final-oo33s38g/final'
obj=CoffObject(str(folder/'ui_pda_cargo.obj')); img=ClientImage(); thunks=img.incremental_thunks()
known=symbol_addresses()
known.setdefault('?Unknown_695A00@PDAInterfaceClass@@QAE_NH_N@Z',set()).add(0x695A00)
known.setdefault('?Instance@OptionClass@@2PAV1@A',set()).add(0xBE8B38) # option.h
known.setdefault('?_Xlen@std@@YAXXZ',set()).add(0xA17E50) # docs/data/bytematch-libcpmt.json
known.setdefault('?Unknown_62F620@SlotDataInterface@@QAEXPBD@Z',set()).add(0x62F620) # app/client/ui_inv.h
known.setdefault('?Unknown_68@SlotDataInterface@@UAE_NH@Z',set()).add(0x62E850) # app/client/ui_inv.h
for row in json.loads((folder/'matches.json').read_text()):
    if row['status']=='exact': known.setdefault(row['symbol'],set()).update(int(x,16) for x in row['addresses'])
_,sec,start,end=next(f for f in obj.functions() if f[0]=='?Unknown_68@UIPdaCargoClass@@UAE_NH@Z')
va=0x5D0620
pat,_,length=obj.pattern(sec,start,end)
retail=img.data[img.offset(va):img.offset(va)+length]
assert length==0x2c4 and re.fullmatch(pat,retail,re.S)
assert object_unwind(obj,sec,start,length) is None
assert b'\x64\xa1\x00\x00\x00\x00' not in retail and b'\x64\x89\x25\x00\x00\x00\x00' not in retail
relocations=[]
for off,width,kind,sym,add in obj.reloc_targets(sec,start,length):
    assert width==4 and add==bytes(4)
    name=sym['name']; value=img.u32(va+off)
    if kind==20:
        target=(va+off+4+struct.unpack('<i',struct.pack('<I',value))[0])&0xffffffff
        target=thunks.get(target,target)
        assert target in known.get(name,set()),(name,hex(target),known.get(name))
    else:
        assert kind==6
        target=value
        if name.startswith('$L'):
            assert sym['section']-1==sec and target==va+sym['value']-start,(name,hex(target),sym)
        elif name.startswith('??_C@'):
            literal=obj.literal(sym)
            if literal is None:
                assert name=='??_C@_00A@?$AA@'
                literal=b'\0'
            assert img.data[img.offset(target):img.offset(target)+len(literal)]==literal
        elif name.startswith('?_C@?1??_Nullstr@'):
            assert img.data[img.offset(target)]==0
        else:
            assert target in known.get(name,set()),(name,hex(target),known.get(name))
    relocations.append([hex(off),name,kind,add.hex(),hex(target)])
baseline=w/'runs/batch1-pqx0cdml/baseline'
old=CoffObject(str(baseline/'ui_pda_cargo.obj'))
oldrows=json.loads((baseline/'matches.json').read_text())
newrows=json.loads((folder/'matches.json').read_text())
oldpairs={(r['symbol'],a) for r in oldrows if r['status']=='exact' for a in r['addresses']}
newpairs={(r['symbol'],a) for r in newrows if r['status']=='exact' for a in r['addresses']}
assert newpairs-oldpairs=={('?Unknown_68@UIPdaCargoClass@@UAE_NH@Z','0x5D0620')} and not oldpairs-newpairs
def signature(o,f):
    _,s,a,b=f; info=o.sections[s]
    raw=o.data[info['raw']+a:info['raw']+b]
    rel=[]
    for off,width,kind,sym,add in o.reloc_targets(s,a,b-a):
        name=sym['name']
        if name.startswith('??_C@'):
            lit=o.literal(sym)
            if lit and lit.replace(b'\\',b'/').endswith(b'/ui_pda_cargo.cpp\0'):
                name='__FILE__:ui_pda_cargo.cpp'
        rel.append((off,width,kind,name,add.hex()))
    return raw,rel
oldfunc={f[0]:f for f in old.functions()};newfunc={f[0]:f for f in obj.functions()}
for name,address in oldpairs:
    assert name in oldfunc and name in newfunc
    assert signature(old,oldfunc[name])==signature(obj,newfunc[name]),(name,address)
out={'body_length':length,'masked_bytes_and_all_branches_match':True,'relocations':relocations,'object_unwind':None,'retail_unwind':'no FS exception registration in body; no handler, FuncInfo, unwind states or funclets','prior_exact_symbol_pairs_unchanged':len(oldpairs),'object_sha256':hashlib.sha256(Path(obj.path).read_bytes()).hexdigest()}
(w/'audit.json').write_text(json.dumps(out,indent=2)+'\n')
print('AUDIT PASS:',len(relocations),'relocations,',length,'bytes; no EH registration')
