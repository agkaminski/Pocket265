; Pocket265 syscall printdU16
; A.K. 2022

.segment "CODE"

.export _svc_printdU16
.proc _svc_printdU16: near
    PHA
    TXA
    TAY
    PLA
    TAX
    BRK
.byte 6
    RTS
.endproc
