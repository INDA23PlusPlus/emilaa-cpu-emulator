## Computer emulator
Yes.
### How to:
| List of instructions and syntax |                                              |
|:--------------------------------|:---------------------------------------------|
| Rn                              | n-th register, from 0x0 to 0x7               |
| Cx                              | 32-bit constant, in hexadecimal.             |
| 0x10 dst src                    | Store value src in dst.                      |
| 0x11 a b                        | Add register / value b to register a.        |
| 0x12 a b                        | Multiply register / value b to register a.   |
| 0x13 a b                        | Compare a to b (a < b)                       |
| 0x14 dst                        | If a cmp was false, go to dst.               |
| 0x15 dst                        | If a cmp was true, go to dst.                |
| 0x16 a                          | Write register / value a to stdout.          |

The emulator has 8 signed 32-bit registers labeled form 0x0 to 0x7.