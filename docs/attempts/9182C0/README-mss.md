# Make_Screen_Shot 0x9182C0 / Update_Movie_Capture 0x918B20: the rectangle residue (2026-10-25, coordinator)

Rounds 3-10 (mss_forms2..9.py, logs mss7..15.log; run with `MSS_FORMS=<file> MSS_FLAGS=<flags> python mss2.py NAMES`):
about 50 forms. VC6 collapses every one into two canonical outputs of the block:
* U-shape (temporaries, zeros, assignments; also -= on temporaries, shared temporaries, inner scopes,
  LONG/int/unsigned, declaration positions, chained zeros): 7 rows. Loads right, left, bottom, top, then
  both subs; left in esi.
* V-shape (`bounds.right -= bounds.left` first): 18-19 rows. Loads left first (good) but hoists top,
  loads bottom late, stores the bottom edge early.
The client: left, right, bottom, sub, top (eax reused), store right, sub; the bottom edge stored after the
LockRect leas; only eax/ecx/edx used although esi and ebx are free. No form gives both halves.
Also tried: a second RECT (frame grows 0x10: out), /G3 /G4 /G5 /G6 /GB (no change except /G6, far off).
Next lead: not a local source form. The allocation may be decided function-wide by the E&B JPG path's
locals (declared how, live where), or by an E&B helper both functions share. Check the JPG-path source
form against the client's register use after the pixel loop before spending more here.
