; Pocket265 syscall sleep
; A.K. 2022

.segment "CODE"

.export _svc_sleep
.proc _svc_sleep: near
    BRK
.byte 15
    RTS
.endproc
