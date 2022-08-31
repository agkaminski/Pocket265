; Pocket265 cc65 crt0 file
; based on the guide: https://cc65.github.io/doc/customizing.html
; A.K. 2022

; ---------------------------------------------------------------------------
; crt0.s
; ---------------------------------------------------------------------------
;
; Startup code for cc65 (Single Board Computer version)

.export   _init, _exit
.import   _main

.export   __STARTUP__ : absolute = 1        ; Mark as startup
.import   __RAM_START__, __RAM_SIZE__       ; Linker generated

.import    copydata, zerobss, initlib, donelib

.include  "zeropage.inc"

; ---------------------------------------------------------------------------
; Place the startup code in a special segment

.segment  "STARTUP"

; ---------------------------------------------------------------------------
; A little light 6502 housekeeping

_init:    LDX     #$FF                 ; Initialize stack pointer to $01FF
          TXS
          CLD                          ; Clear decimal mode

; ---------------------------------------------------------------------------
; Set cc65 argument stack pointer

          LDA     #<(__RAM_START__ + __RAM_SIZE__)
          STA     sp
          LDA     #>(__RAM_START__ + __RAM_SIZE__)
          STA     sp+1

; ---------------------------------------------------------------------------
; Initialize memory storage

          JSR     zerobss              ; Clear BSS segment
          JSR     copydata             ; Initialize DATA segment
          JSR     initlib              ; Run constructors

; ---------------------------------------------------------------------------
; Call main()

          JSR     _main

; ---------------------------------------------------------------------------
; Back from main (this is also the _exit entry):  inifite loop

_exit:    JSR     donelib              ; Run destructors
          BRK

.import   __isr_break, __isr_nmi, __isr_irq
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

           JSR __isr_nmi

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

           JSR __isr_irq

           PLA
           TAY
           PLA
           TAX
           PLA
           RTI

; ---------------------------------------------------------------------------
; BRK detected, stop

_break:    JMP __isr_break

; Defines the interrupt vector table.

.segment  "VECTORS"

.addr      _nmi_int    ; NMI vector
.addr      _init       ; Reset vector
.addr      _irq_int    ; IRQ/BRK vector
