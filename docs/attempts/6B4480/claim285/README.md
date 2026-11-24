# Claim 285: ViewClass constructor

Baseline f383c836; 11 compiled candidates, plus one successful control. The
initial sandboxed control failed before compilation because Wine could not bind
its socket; it was rerun successfully. Each adjacent JSON replay encodes a
complete unified patch against the baseline `view.cpp`; all were applied to a
fresh baseline and compared byte for byte with the tested source. App flags,
original basename, and physical assert lines 40, 78, 106 and 113 were preserved.
Whole-unit matching kept all 44 baseline exact symbol rows in every candidate;
the constructor was never exact. The basename-only build does not verify the
full-app `__FILE__` string or the full application.

Raw target-body SHA-256 (including object padding; relocation slots are zero in
these bodies, so masked hashes are identical):

- `285a42744d458504f74641f4d4b1d3efa7577409fc31a7404ef386ac6ec045fc`, 128 bytes: assign_return, local_return, via_reference, ref_init, ptr_init, return_new, assign_method.
- `6b8e03c7de75f4e7861e68dd7e819dffcd2bcbab0a2e9fe219a5d5a17a845317`, 144 bytes: conditional_assign, assign_if.
- `e5ce80a9f32ce3685b6a7cd4de3fe8683b2c8e4ec1ecb053cbf44ace723fcab1`, 128 bytes: separate_null.
- `13d5669cf1eb6d94be1a617742cc94f100762c8ece52537e1b72bd7c3a40b867`, 128 bytes: return_cond.

The direct and simple forwarding forms retain the joined allocation-null path.
Conditional assignment produces two epilogues, but saves an extra EDI and uses
it as null instead of retail's XOR EAX; separate_null adds a redundant store,
and return_cond adds a result test. No new target warrants relocation or EH
equivalence credit. A member initializer allocates before the already-observed
SList constructor, changing initialization and failure cleanup order, so was
not a valid candidate. A missing source-level reason for the retail split
remains the lead; inventing one solely to force it is unsupported.
