; Pocket265 syscall getKeyNb
; A.K. 2022

.segment "CODE"

.export _svc_getKeyNb
.proc _svc_getKeyNb: near
    BRK
.byte 11
    RTS
.endproc
