; Pocket265 syscall puts
; A.K. 2022

.segment "CODE"

.export _svc_puts
.proc _svc_puts: near
    PHA
    TXA
    TAY
    PLA
    TAX
    BRK
.byte 3
    RTS
.endproc
