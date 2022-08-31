; Pocket265 cc65 interrupts
; based on the guide: https://cc65.github.io/doc/customizing.html
; A.K. 2022

; ---------------------------------------------------------------------------
; interrupt.s
; ---------------------------------------------------------------------------
;
; Interrupt handler.
;
; Checks for a BRK instruction and returns from all valid interrupts.

.import   _stop, _nmi, _irq
.export   _irq_int, _nmi_int

.segment  "CODE"

; ---------------------------------------------------------------------------
; Non-maskable interrupt (NMI) service routine

_nmi_int:  PHA
           TXA
           PHA
           TYA
           PHA

           TSX
           INX
           INX
           LDA $100,X
           AND #$10
           BNE _break

           JSR _nmi

           PLA
           TAY
           PLA
           TAX
           PLA
           RTI

; ---------------------------------------------------------------------------
; Maskable interrupt (IRQ) service routine

_irq_int:  PHA
           TXA
           PHA
           TYA
           PHA

           TSX
           INX
           INX
           LDA $100,X
           AND #$10
           BNE _break

           JSR _irq

           PLA
           TAY
           PLA
           TAX
           PLA
           RTI

; ---------------------------------------------------------------------------
; BRK detected, stop

_break:    JMP _stop              ; If BRK is detected, something very bad
                                  ;   has happened, so stop running
