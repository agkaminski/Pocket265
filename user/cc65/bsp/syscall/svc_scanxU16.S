; Pocket265 syscall scanxU16
; A.K. 2022

.segment "CODE"

.export _svc_scanxU16
.proc _svc_scanxU16: near
    PHA
    TXA
    TAY
    PLA
    TAX
    BRK
.byte 8
    PHA
    TXA
    TAY
    PLA
    TAX
    RTS
.endproc
