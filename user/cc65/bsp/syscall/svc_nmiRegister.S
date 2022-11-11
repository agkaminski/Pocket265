; Pocket265 syscall nmiRegister
; A.K. 2022

.segment "CODE"

.export _svc_nmiRegister
.proc _svc_nmiRegister: near
    PHA
    TXA
    TAY
    PLA
    TAX
    BRK
.byte 1
    RTS
.endproc
