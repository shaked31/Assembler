MAIN: add $3,$5,$9
mcro GEN_MAC
la val1
jmp NEXT
mcroend
LOOP: ori $9,-5,$2
GEN_MAC
NEXT: move $20,$4
STR: .asciz "aBcd"