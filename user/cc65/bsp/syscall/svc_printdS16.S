; Pocket265 syscall printdS16
; A.K. 2022

.segment "CODE"

.export _svc_printdS16
.proc _svc_printdS16: near
    PHA
    TXA
    TAY
    PLA
    TAX
    BRK
.byte 7
    RTS
.endproc
