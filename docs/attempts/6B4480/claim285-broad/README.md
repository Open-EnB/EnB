# Claim 285: null member initialization

Two forms compiled against immutable e7af06af. Both initialize `Unknown04` to
NULL in the member initializer, then allocate the dispatcher in the constructor
body after SList construction. One assigns the `new` expression directly; the
other uses the previously tested conditional assignment form. Neither matches
0x6B4480; both preserve all 44 original exact symbol/address and file/address
pairs. Original app flags, basename and physical assertion lines are preserved.
The JSON-encoded patches round-trip against `view.cpp` at e7af06af.

| Form | Source SHA-256 | Raw and masked target SHA-256 | Size |
|---|---|---|---:|
| null_member | d3a876a8beae64bf3b6948f5a721efb91904c51883d262bbdb5e9bb31c422d67 | 1365bd49fa428d920fa461b4fd47f5f97d8bc624f16616c396d528a7d7a0a453 | 128 |
| null_member_conditional | 1e18b3e9b2f2e319d90fd6a82c7262645fe1106b60d154c5f56a6976f6b48119 | ae0e19c3820ebe4b3de92b17be78d8a895747197bf94e7082177eaac07bfb0f4 | 144 |

Each has two unwind states. Both emit a real early zero store to the pointer,
save EDI and use it for zero; neither matches retail's initial stores. The
direct form still joins the null path; the conditional form splits the returns
but adds a second result test and uses EDI for the null store. Both call the
allocator and dispatcher constructor. The test concerns member NULL
initialization, distinct from an allocation in the member initializer, which
would alter the allocation and SList construction order. Scoped native unit
matching does not verify the full app or the full `__FILE__` path.
