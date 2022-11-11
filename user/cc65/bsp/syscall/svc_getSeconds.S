; Pocket265 syscall getSeconds
; A.K. 2022

.segment "CODE"

.export _svc_getSeconds
.proc _svc_getSeconds: near
    BRK
.byte 13
    RTS
.endproc
