; Pocket265 syscall msleep
; A.K. 2022

.segment "CODE"

.export _svc_msleep
.proc _svc_msleep: near
    BRK
.byte 14
    RTS
.endproc
