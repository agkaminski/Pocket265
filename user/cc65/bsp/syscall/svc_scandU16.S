; Pocket265 syscall scandU16
; A.K. 2022

.segment "CODE"

.export _svc_scandU16
.proc _svc_scandU16: near
    PHA
    TXA
    TAY
    PLA
    TAX
    BRK
.byte 9
    PHA
    TXA
    TAY
    PLA
    TAX
    RTS
.endproc
