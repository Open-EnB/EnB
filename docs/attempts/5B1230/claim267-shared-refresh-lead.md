# Deferred manufacturing refresh lead

Claim #267 ends at `0x5B1231`. The shared refresh function
`UIManuClass::Unknown_5B3640` lies outside that claim and was not independently
rebuilt. Its body was left unchanged. A future claim covering it could investigate
the standalone function together with its inlined copies in `0x5B1230` and
`0x5B16C0`.

## Retained state

The changed-field predicate copied from the exact refinery code recovers the
ManufactureComponents Cast holder at `0x5B7210`. Together with the earlier duration
changes, the manufacturing unit now has 145 exact file/address pairs. All earlier
matches remain intact. The constructor and window-open method are exact; the
`0x5B1230` update remains partial.

Its production form has a `0x3C` frame and the desired ManufactureComponents and
Validity Cast calls (`0x5B7210`, `0x5468D0`). The original refresh call stays in
source and inlines. However, the nested PreviousAttempts lookup calls the whole
`Unknown_Find_Valid_List` wrapper. Retail constructs the name in the parent and
calls the exact set-list helper at `0x5B6DB0` instead.

The matched relatives `0x5B1ED0`, `0x5B3850`, `0x5B3580`, and `0x5B6DB0` were
checked and preserved throughout. The standalone `0x5B3640` has a related existing
Cast/inlining mismatch; its older attempt history is available through
`python tools/re/attempts.py show 0x5B3640`.

## What the comparisons established

- Target-local expansion of the existing refresh logic with a direct
  PreviousAttempts helper over-inlines the lookup and later helpers, giving the
  wrong `0x70` frame.
- Retaining `Unknown_Get_List()` in that expansion recovers the name constructor
  and `0x5B6DB0` call, but later `0x5B6CA0` and `0x5B3580` still inline too far.
- Ending the Validity name's lifetime in that form restores the `0x3C` frame.
  This supports the name-slot lifetime explanation, but the later calls remain
  wrong. No expanded form was adopted.
- On the positive production base, preserving the original refresh call and
  using an implicit Validity-name argument reduces the residue to 12 masked
  rows, 9 with registers masked. It retains 249 instructions versus retail's 255
  and still lacks the `0x5B6DB0` call. An independent narrow name scope is
  canonical. Neither form was adopted.

All these trials preserve the 145 manufacturing file/address pairs. Name
construction before the aux-root read was checked. The shared refresh body and
all matched relatives were left unchanged.

## Suggested future scope

Compare the standalone refresh and both inlined callers under a claim covering
the shared function. Look for an actual helper/lifetime convention that explains
all three contexts while preserving the existing exact relatives. No new
signature error has been established. A private clone, inert wrapper, arbitrary
argument expression, or inline-control flag is not supported by the evidence.

Use fresh baselines after #267 lands. Preserve the duration representation,
components predicate, class layout, and all existing pair sets. Shared-header
changes require all-consumer checks; source changes to the nonexact refresh need
the appropriate claim before rebuilding it.

Evidence and independent replay recipes:

- [Matched-helper comparison](claim267-matched-helper-cross/results.json)
- [Focused lifetime results](claim267-name-lifetime-followup/results.json)
- [Astra's focused analysis](claim267-name-lifetime-followup/nested-helper-followup.md)

Analysis: gpt-6-astra (high) and gpt-6.1-sol (high). Experiments and audits:
gpt-6.1-sol (high). Accountable contributor: @vessellaj.
