# Assembler Test Suite

18 `.as` files covering every stage of the pipeline (pre-assembler,
first pass, second pass) plus successful end-to-end runs. Organized
by expected outcome so this doubles as a regression suite you can
re-run after any change.

## `valid/` — should assemble successfully, producing `.am`/`.ob`/`.ent`/`.ext`

| File | What it checks |
|---|---|
| `valid1_full_reference.as` | Every instruction format (R/I/J), all four directives, `.entry` + `.extern`. Byte-verified against the assignment's own worked example — a strong end-to-end regression check, not just a smoke test. |
| `valid2_macros.as` | A macro defined once, invoked twice. Check the generated `.am` to confirm both invocations were actually expanded. |
| `valid3_data_edge_ranges.as` | `.db`/`.dw`/`.dh` at the exact edges of their legal ranges (not just "some value that fits"), a multi-word `.asciz` string, register-indirect `jmp`. |
| `valid4_mcro_substring_safe.as` | The literal text "mcro" appears inside a comment and inside a quoted string. Neither is a real macro definition — this only passes if macro-start detection excludes comments/quoted content rather than doing a raw substring search. |

## `pre_assembler_failures/` — should fail before first pass; no output files

| File | What it checks |
|---|---|
| `pre1_macro_name_errors.as` | Reserved-word macro name, duplicate macro name, garbage text after a macro name — three errors, one run. |
| `pre2_garbage_before_mcro.as` | Text before the `mcro` keyword on a definition line (`asdas mcro FOO`). Must be flagged as an error, not silently treated as an ordinary line. |
| `pre3_mcroend_errors.as` | Garbage text before *and* after `mcroend`, on two separate macros. |
| `pre4_empty_macro.as` | A macro with zero body lines, then invoked. Either rejecting it at definition or accepting it as a no-op expansion is defensible — what matters is that it doesn't crash. This is the regression test if you've previously hit a segfault on an empty macro body. |

## `first_pass_failures/` — should fail during the first pass; no output files

| File | What it checks |
|---|---|
| `fp1_register_errors.as` | Register out of range (`$32`), leading zero (`$05`), non-numeric register. |
| `fp2_label_errors.as` | Label starting with a digit, a reserved-word label, a label over 31 characters, a duplicate label. |
| `fp3_comma_errors.as` | Leading comma, doubled comma, missing comma, trailing comma. |
| `fp4_directive_value_errors.as` | `.db` value out of `[-128,127]`, `.dh` value out of `[-32768,32767]`, `.asciz` with no closing quote. |
| `fp5_unknown_commands.as` | An unknown instruction name and an unknown directive name. |
| `fp6_line_too_long.as` | A (commented) source line padded past 80 characters, to check the length limit is enforced at read time. |

## `second_pass_failures/` — first pass succeeds, second pass fails; no output files

| File | What it checks |
|---|---|
| `sp1_undefined_label.as` | `jmp` and a conditional branch, both targeting a label that's never defined. Only catchable once the whole symbol table exists. |
| `sp2_external_in_branch.as` | An `.extern` symbol used as a conditional-branch target — illegal, since branches encode a relative distance that can't be computed for a symbol defined elsewhere. |
| `sp3_entry_extern_conflict.as` | The same symbol declared both `.extern` and `.entry`, tested in **both** orderings deliberately — one direction is easy to catch in the first pass, the other requires a second-pass check; a common place for only one direction to get implemented. |
| `sp4_entry_undefined.as` | `.entry` referring to a symbol that's never defined — legal to check only after the full file is scanned, since `.entry` can point at a label defined later. |

## Notes

- Every error-case file is written to trigger **multiple** distinct
  errors within its own stage, on purpose — a correct implementation
  should report all of them in one run rather than stopping at the
  first. If a file reports fewer errors than its description lists,
  that's worth investigating on its own.
- None of the failing files should produce *any* of `.am`/`.ob`/`.ent`/`.ext`
  — partial output on a failing file is itself a bug worth flagging.
- Comments in this language must occupy a whole line (first
  non-blank character is `;`) — there's no inline/trailing-comment
  syntax, so none of these files rely on one.
