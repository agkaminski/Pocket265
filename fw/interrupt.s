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

.import   _init, _stop
.export   _nmi_clr

.segment  "CODE"

; ---------------------------------------------------------------------------
; Non-maskable interrupt (NMI) acknowledge routing

.proc _nmi_clr: near
           STA $C800
           RTS
.endproc

.import _key_curr, _key_prev, _key_cooldown

.proc scan_keyb: near
                LDX #5                  ; loop counter
@row_loop:      LDY @lut0, X            ; row address offset
                LDA $C000, Y            ; get row data
                AND #$0F
                CMP #$0F                ; is a key pressed on the row?
                BNE @row_finish
                DEX
                BPL @row_loop           ; are we done?
                LDY #$FF                ; no key pressed
                BNE @debounce

@row_finish:    LDY @lut1, X            ; base key of a row
                CLC                     ; To avoid endless loop, just in case
@col_loop:      ROR A                   ; get next bit into carry
                BCC @debounce           ; have we found collumn?
                INY                     ; select next key in a row
                JMP @col_loop

@debounce:      CPY _key_prev           ; same key as before?
                BEQ @debounce_do
                STY _key_prev
                LDA #30                 ; start new cooldown
                STA _key_cooldown
                RTS

@debounce_do:   LDA _key_cooldown       ; still the same keycode
                BEQ @end                ; deboucing has been done already
                DEC _key_cooldown       ; decrement cooldown
                BNE @end                ; debouncing done?
                STY _key_curr           ; update new keycode
@end:           RTS

@lut0: .byte $3E, $3D, $3B, $37, $2F, $1F
@lut1: .byte $14, $0C, $08, $04, $00, $10

.endproc

.import _jiffies, _seconds

.proc handle_timer: near
                INC _jiffies
                BNE @L1
                LDX _jiffies + 1
                INX
                CPX #1024 / 256
                BMI @L0
                INC _seconds
                LDX #0
@L0:            STX _jiffies + 1
@L1:            RTS

.endproc

; ---------------------------------------------------------------------------
; Non-maskable interrupt (NMI) service routine

_nmi_int:       PHA
                TXA
                PHA
                TYA
                PHA
                CLD

                JSR scan_keyb
                JSR handle_timer
                JSR _nmi_clr

                PLA
                TAY
                PLA
                TAX
                PLA
                RTI

; ---------------------------------------------------------------------------
; Maskable interrupt (IRQ) service routine

_irq_int:       PHA
                TXA
                PHA
                TYA
                PHA
                CLD

                ; TODO

                PLA
                TAY
                PLA
                TAX
                PLA
                RTI

; ---------------------------------------------------------------------------
; BRK detected, stop

_break:         JMP _break             ; If BRK is detected, something very bad
                                       ;   has happened, so stop running

; Defines the interrupt vector table.

.segment  "VECTORS"

.addr      _nmi_int    ; NMI vector
.addr      _init       ; Reset vector
.addr      _irq_int    ; IRQ/BRK vector
