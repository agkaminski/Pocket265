; Pocket265 syscall putb
; A.K. 2022

.segment "CODE"

.export _svc_putb
.proc _svc_putb: near
    BRK
.byte 4
    RTS
.endproc
