; Pocket265 syscall nmiStart
; A.K. 2022

.segment "CODE"

.export _svc_nmiStart
.proc _svc_nmiStart: near
    BRK
.byte 17
    RTS
.endproc

