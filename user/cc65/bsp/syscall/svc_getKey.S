; Pocket265 syscall getKey
; A.K. 2022

.segment "CODE"

.export _svc_getKey
.proc _svc_getKey: near
    BRK
.byte 10
    RTS
.endproc
