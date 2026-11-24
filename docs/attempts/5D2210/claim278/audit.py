from pathlib import Path
import os,sys,struct,re,json,hashlib
root=Path('/home/dev/Projects/EnB');w=root/'.work/claim-278/5D2210';sys.path.insert(0,str(root/'tools/re'))
os.environ['ENB_RETAIL_INSTALL']='/home/dev/Games/earth-and-beyond/prefix/drive_c/Program Files (x86)/EA GAMES/Earth & Beyond'
from objmatch import CoffObject,symbol_addresses,object_unwind,client_funcinfos,_state_pattern,_state_match,_whole_text
from clientimage import ClientImage
img=ClientImage(); thunks=img.incremental_thunks();known=symbol_addresses(); folder=Path(sys.argv[1]) if len(sys.argv)>1 else w/'runs/batch1-fgmw35ja/bottomreverse';obj=CoffObject(str(folder/'ui_pda_equip.obj')); rows=json.loads((folder/'matches.json').read_text())
for r in rows:
 if r['status']=='exact':known.setdefault(r['symbol'],set()).update(int(x,16) for x in r['addresses'])
def rels(sec,start,length,va,extra={}):
 out=[]
 for off,width,kind,sym,add in obj.reloc_targets(sec,start,length):
  name=sym['name'];assert width==4 and add==bytes(4),(name,add.hex());value=img.u32(va+off);target=(va+off+4+struct.unpack('<i',struct.pack('<I',value))[0])&0xffffffff if kind==20 else value;target=thunks.get(target,target)
  if name=='__except_list':assert value==0
  elif name in extra:assert value==extra[name]
  elif name.startswith('$L'):
   assert sym['section']-1==sec and value==va+sym['value']-start,(name,hex(value),sym)
  elif name.startswith('??_C@'):
   lit=obj.literal(sym);assert lit is not None,name
   if b'.cpp\0' in lit or b'.h\0' in lit:
    assert img.data[img.offset(value):].split(b'\0',1)[0].replace(b'\\',b'/').split(b'/')[-1]==lit.rstrip(b'\0').replace(b'\\',b'/').split(b'/')[-1],name
   else:assert img.data[img.offset(value):img.offset(value)+len(lit)]==lit,name
  elif name=='?TheInstance@WW3DAssetManager@@1PAV1@A':assert value==0xbfaee0
  else:assert target in known.get(name,set()),(name,hex(target),known.get(name))
  out.append([hex(off),name,kind,add.hex(),hex(target)])
 return out
out={}
for prefix,va in [('?Unknown_B0@UIPdaEquipClass@@',0x5d2210),('?Create_Get@@YA?AV?$RefCountPtr@VTextureClass@@@@PAVTextureClass@@@Z',0x47d010)]:
 name,sec,start,end=next(x for x in obj.functions() if x[0].startswith(prefix));pat,_,length=obj.pattern(sec,start,end);raw=img.data[img.offset(va):img.offset(va)+length];assert re.fullmatch(pat,raw,re.S),(name,length)
 un=object_unwind(obj,sec,start,length);extra={};states=[]
 if un:
  handler=img.u32(va+un[0]);hsym=next(r[3] for r in obj.reloc_targets(sec,start,length) if r[0]==un[0]);extra[hsym['name']]=handler
  hsec=hsym['section']-1;hoff=hsym['value'];rb=img.data[img.offset(handler):img.offset(handler)+10];hb=bytearray(obj.data[obj.sections[hsec]['raw']+hoff:obj.sections[hsec]['raw']+hoff+10]);hr=obj.reloc_targets(hsec,hoff,10)
  for off,width,kind,sym,add in hr:
   assert width==4 and add==bytes(4)
   if off==6:
    target=handler+off+4+struct.unpack_from('<i',rb,off)[0];target=thunks.get(target,target);assert target in known.get(sym['name'],set()),(sym['name'],hex(target))
   else:assert off==1
   hb[off:off+width]=rb[off:off+width]
  assert hb==rb
  maps,_=client_funcinfos(img);cm=maps[handler];assert len(cm)==len(un[2]);text,base=_whole_text(img)
  for state,cf in zip(un[2],cm):
   if state is None:assert not cf;states.append(None);continue
   assert _state_match(_state_pattern(obj,un[1],state),cf,text,base);states.append({'va':hex(cf),'relocations':rels(un[1],state[0],state[1]-state[0],cf)})
  fr=next(r for r in hr if r[0]==1);fs=fr[3];fi=obj.sections[fs['section']-1];fo=fs['value']+struct.unpack('<I',fr[4])[0];ov=struct.unpack_from('<7I',obj.data,fi['raw']+fo);fva=img.u32(handler+1);cv=struct.unpack_from('<7I',img.data,img.offset(fva));assert (ov[0],ov[1],ov[3:])==(cv[0],cv[1],cv[3:]);assert ov[3:]==(0,0,0,0)
  mr=next(r for r in obj.reloc_targets(fs['section']-1,fo,28) if r[0]==8);ms=mr[3];mi=obj.sections[ms['section']-1];mo=ms['value']+struct.unpack('<I',mr[4])[0];op=[struct.unpack_from('<i',obj.data,mi['raw']+mo+8*i)[0] for i in range(len(states))];cp=[struct.unpack_from('<i',img.data,img.offset(cv[2])+8*i)[0] for i in range(len(states))];assert op==cp
 else:
  assert b'\x64\xa1\x00\x00\x00\x00' not in raw and b'\x64\x89\x25\x00\x00\x00\x00' not in raw
 out[hex(va)]={'symbol':name,'size':length,'relocations':rels(sec,start,length,va,extra),'handler':hex(handler) if un else None,'states':states,'predecessors':op if un else []}
(w/'audit.json').write_text(json.dumps(out,indent=2)+'\n');print('PASS',[(k,v['size'],len(v['relocations']),len(v['states'])) for k,v in out.items()])
