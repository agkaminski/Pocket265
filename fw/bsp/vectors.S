; Pocket265 cc65 interrupt vectors
; based on the guide: https://cc65.github.io/doc/customizing.html
; A.K. 2022

; ---------------------------------------------------------------------------
; vectors.s
; ---------------------------------------------------------------------------
;
; Defines the interrupt vector table.

.import    _init, _nmi_int, _irq_int

.segment  "VECTORS"

.addr      _nmi_int    ; NMI vector
.addr      _init       ; Reset vector
.addr      _irq_int    ; IRQ/BRK vector
