; Pocket265 syscall nmiDisable
; A.K. 2022

.segment "CODE"

.export _svc_nmiDisable
.proc _svc_nmiDisable: near
    BRK
.byte 16
    RTS
.endproc
