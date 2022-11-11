; Pocket265 syscall putch
; A.K. 2022

.segment "CODE"

.export _svc_putch
.proc _svc_putch: near
    BRK
.byte 2
    RTS
.endproc
