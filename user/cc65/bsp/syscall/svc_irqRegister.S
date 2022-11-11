; Pocket265 syscall irqRegister
; A.K. 2022

.segment "CODE"

.export _svc_irqRegister
.proc _svc_irqRegister: near
    PHA
    TXA
    TAY
    PLA
    TAX
    BRK
.byte 0
    RTS
.endproc
