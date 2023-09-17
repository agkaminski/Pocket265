# Pocket265

Handheld PC based on the original 6502 CPU that fits into a pocket.

<img src="img/computer.jpg" width="45%"> <img src="img/bottom.jpg" width="45%">

## Features

- 12 digit 16-segment LED display (DL1414),
- optional 8x2 standard LCD (not supported yet in FW),
- 24 button keyboard,
- 8 KiB of ROM and 8 KiB of RAM,
- I2C 24xx EEPROM memory (16 bit addressing, up to 64 KB),
- built-in Li-Po power supply, charger and visual voltage indicator,
- monitor FW that allows memory editing and running user programs,
- expansion slot with 5 chip selects (up to 40 KiB of external I/O and/or memory),
- based on my Pocket65 project (github.com/agkaminski/Pocket65) with a partial compability.

# Memory map

| Start  | End    | Description |
|--------|--------|-------------|
| 0x0000 | 0x1FFF | RAM         |
| 0x2000 | 0x3FFF | External 1  |
| 0x4000 | 0x5FFF | External 2  |
| 0x6000 | 0x7FFF | External 3  |
| 0x8000 | 0x9FFF | External 4  |
| 0xA000 | 0xBFFF | External 5  |
| 0xC000 | 0xC3FF | Keyboard    |
| 0xC400 | 0xC7FF | Screen      |
| 0xC800 | 0xCBFF | NMI ack     |
| 0xCC00 | 0xCFFF | 4 bit GPIO  |
| 0xE000 | 0xFFFF | ROM         |

# Simulator

Pocket265 simulator is there to make FW developement easier, but it can be also used to get a feel for the computer usage. Only UART is not simulated. It uses [simak65](https://github.com/agkaminski/simak65) simulator code.

## Usage

<pre>
$ ./pocket265-sim [OPTIONS]
</pre>

where options are:
- -c: path to the computer ROM (obligatory),
- -f: simulated CPU frequency, from 0 (max speed, no throttling) to 20000000 (Hz),
- -r: simulated RAM size in bytes,
- -e: simulated EEPROM file (will be updated after end of the simulation, if write is performed!),
- -h: usage.

## Building

To run the simulation first build and install [simak65](https://github.com/agkaminski/simak65):
<pre>
$ (git submodule update --init && cd simak65 && make all && sudo make install)
</pre>

then build the FW:
<pre>
$ (cd fw && make all)
</pre>

then build the simulator:
<pre>
$ (cd sim && make all)
</pre>

## Running the simulation

In the `sim/` directory is a script that starts the simulator with memory initialized with Pocket265 firmware - `runsim.sh`. To run the simulator:
<pre>
$ cd sim
$ ./runsim.sh
</pre>

Simulator is started and after boot sequence the main screen is presented in the terminal:

<pre>
[MON 0800 >00]

[ F1  ][ F2  ][ F3  ][ F4  ]
[ C   ][ D   ][ E   ][ F   ]
[ 8   ][ 9   ][ A   ][ B   ]
[ 4   ][ 5   ][ 6   ][ 7   ]
[ 0   ][ 1   ][ 2   ][ 3   ]
[ INC ][ DEC ][ SEL ][ GO  ]
</pre>

Keyboard keys presses are redirected to the simulated Pocket265 computer. Key mapping is presented below:

| Keyboard | Pocket265    |
|----------|--------------|
| 0-9, a-f | hex keyboard |
| h        | INC          |
| j        | DEC          |
| k        | SEL          |
| l        | GO           |
| u        | F1           |
| i        | F2           |
| o        | F3           |
| p        | F4           |

# Monitor how-to

## Startup

First welcome splash-screen is displayed:

<pre>
POCKET265
</pre>

After that version and copyright information is scrolled over and a memory test is performed:

<pre>
6144 B FREE
</pre>

By default 6144 bytes are free (2 KiB are reseved for ROM), the memory can be expanded by expansion slot card.

After 1 seconds computer proceeds to the main mode of operation:

<pre>
MON 0800 >00
</pre>

## Memory edit

In the main mode and byte modification mode selected (`>` pointing to the memory value) memory editing is possible. To modify the memory content press a key `0` to `F`. The old value is then shifted 4 binary places to the left and new nibble is inserted at the youngest position. Previous 4 oldest bits are lost.

Example - entering `0xBA` at the current address:

We start with the desired memory cell selected (in this case the cell at address `0x0800`):

<pre>
MON 0800 >00
</pre>

press `B` button to enter 0xB:

<pre>
MON 0800 >0B
</pre>

then press `A` button to finish entering the byte:

<pre>
MON 0800 >BA
</pre>

Done!

## Address selection

The address can be modified in two ways:

### INC/DEC buttons

To select a next or previous memory cell press `INC` or `DEC` button respectively. The address will be incremented/decremented by one.

### Entering new address

Big leaps through memory space are not very convenient using incrementation and decrementation. One can change the address very similar to changing a memory value. To enter the address editing mode press `SEL` key. `<` will point to the address to confirm mode selection:

<pre>
MON 0800< 00
</pre>

In this mode address can be modified the same way memory can be modified - when key `0-F` is pressed the old address is shifted 4 bits to the left and new value is inserted at the youngest position.

To exit the address editing mode (and return to the memory value editing mode) press `SEL` key again.

## Executing user program

To start a user program provide its entry point in the address field (using the address edition mode) and press `GO` key.

## F1 key - auto increment mode

`F1` key toggles auto-increment mode. When active it is indicated by a `+` sign next to the address:

<pre>
MON 0800+>00
</pre>

If this mode is active then when a whole byte is entered (i.e. two `0-F` keys presses) the address will be incremented automatically. This mode is very useful for a binary program listing input, as no additional key press between bytes inputs is needed.

## F2 key - monitor menu

`F2` key displays monitor menu which contains additional ROM functions. To show a next item press `INC` key, press `DEC` for a previous one. To select an item press `GO`.

### 1: COPY MEM

Copy memory range. A destination address is an address currently set in the monitor. User is prompted for a source address and length (both inputed as 4 digit hexadecimal numbers).

### 2: I2C SAVE

Save data to the I2C memory. A source address is an address currently set in the monitor. User is prompted for a destination address on the device and length (both inputed as 4 digit hexadecimal numbers).

### 3: I2C LOAD

Load data from the I2C memory. A destination address is an address currently set in the monitor. User is prompted for a source address on the device and length (both inputed as 4 digit hexadecimal numbers).

### 4: UART COM

Enable serial communication (available on the mini USB port via USB-UART bridge). 200 baud, 1 stop, no parity. Communication is very similar to pressing keys on the computer keyboard and it's mostly dedicated for scripting use. Each valid character is echoed back, data stream should be slow (i.e. sender should wait for an echo before sending a next character, as UART function is performed in software via bit-bang and it's half-duplex only). Valid characters:

- `0-9`, `a-f`, `A-F` - hex numbers to set address or data,
- `@` - set mode to address editing mode,
- `#` - set mode to data editing mode,
- `!` - start program at current address.

Auto increment mode is always enabled for serial communication.

### 5: VERSION

Display FW version (git hash/tag).

### 6: CLR MEM

Clear user memory (set all bytes to 0).

# Programming

User program examples are provided in the `user/example` directory, the user program template is provided in the `user` directory along with ROM calls user library and Makefile.

User is free to use whole memory, exluding:

| Start  | End    | Description       |
|--------|--------|-------------------|
| 0x00C0 | 0x00FF | ROM ZP area       |
| 0x0100 | 0x01FF | HW stack          |
| 0x0200 | 0x07FF | RAM for ROM usage |
| 0xC000 | 0xCFFF | HW handled by ROM |

## Interrupts

Interrupt vectors are located in ROM area, so they cannot be modified by an application code. Monitor can redirect IRQ and NMI to user handlers - there are two ROM calls to register an user interrupt handler:

- `svc_irqRegister` - register a IRQ handler,
- `svc_nmiRegister` - register a NMI handler.

The handler address is passed in X (low byte) and Y (high byte) registers.

Use `RTS` instruction to return from the handler insted of `RTI` - monitor handles return from the interrupt part.

## System calls

### Convention

When X and Y registers are used to pass 16-bit value, X holds low byte and Y holds high byte. ROM calls do not destroy unrelated registers/memory contents.

### `svc_irqRegister`

Register a user IRQ handler and enable interrupts.

- Arguments: X, Y - handler address.
- Returns: none.

### `svc_nmiRegister`

Register a user NMI handler.

- Arguments: X, Y - handler address.
- Returns: none.

### `svc_putch`

Displays an ASCII character on the screen.

- Arguments: A - character.
- Returns: none.

### `svc_puts`

Displays a null-terminated ASCII string on the screen.

- Arguments: X, Y - string address.
- Returns: none.

### `svc_putb`

Display a hex byte on the screen.

- Arguments: A - byte.
- Returns: none.

### `svc_prinxU16`

Print a 16-bit number stored in registers X and Y.

- Arguments: X, Y - number.
- Returns: none.

### `svc_printdU16`

Print a 16-bit number in registers X and Y as a unsigned decimal.
- Arguments: X, Y - number.
- Returns: none.

### `svc_printdS16`

Print a 16-bit number in registers X and Y as a signed decimal.

- Arguments: X, Y - number.
- Returns: none.

### `svc_scanxU16`

Fetch a 16-bit unsigned number from the user (hexadecimal input).

- Arguments: X, Y - ASCII string (prompt).
- Returns: X, Y - number.

### `svc_scandU16`

Fetch a 16-bit unsigned number from the user (decimal input).

- Arguments: X, Y - ASCII string (prompt).
- Returns: X, Y - number.

### `svc_getKey`

Fetch a key code, block until a new key is pressed.

- Arguments: none.
- Returns: A - key code.

### `svc_getKeyNb`

Fetch a current key code.

- Arguments: none.
- Returns: A - key code.

### `svc_getJiffies`

Fetch a current miliseconds timer value (one byte).

- Arguments: none.
- Returns: A - jiffies.

### `svc_getSeconds`

Fetch a current seconds timer value (one byte).

- Arguments: none.
- Returns: A - seconds.

### `svc_msleep`

Sleep for 0-255 ms.

- Arguments: A - time (ms).
- Returns: none.

### `svc_sleep`

Sleep for 0-255 s.

- Arguments: A - time (s).
- Returns: none.

### `svc_nmiDisable`

Disable NMI.

- Arguments: none
- Returns: none.

### `svc_nmiStart`

Restart NMI.

- Arguments: none.
- Returns: none.

# License

Free for non-commercial use and educational purposes. See LICENSE.md for details.
