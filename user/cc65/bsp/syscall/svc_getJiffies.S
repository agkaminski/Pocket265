; Pocket265 syscall getJiffies
; A.K. 2022

.segment "CODE"

.export _svc_getJiffies
.proc _svc_getJiffies: near
    BRK
.byte 12
    RTS
.endproc
