; Pocket265 syscall printxU16
; A.K. 2022

.segment "CODE"

.export _svc_printxU16
.proc _svc_printxU16: near
    PHA
    TXA
    TAY
    PLA
    TAX
    BRK
.byte 5
    RTS
.endproc
