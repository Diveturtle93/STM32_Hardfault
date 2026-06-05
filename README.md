# STM32 Hardfault Handler
 
Eine in C implementierte Bibliothek für STM32-Mikrocontroller, die den
Standard-HardFault-Handler der HAL-Bibliothek ersetzt. Bei einem HardFault werden der
vollständige Stack-Frame, alle relevanten Fault-Statusregister und eine detaillierte
Klartextbeschreibung der Fehlerursache über UART ausgegeben. Im Debug-Build wird
zusätzlich ein Software-Breakpoint ausgelöst, damit der Debugger direkt an der
Fehlerstelle anhält.
 
## Beschreibung
 
Ein HardFault ist ein schwerwiegender CPU-Fehler auf ARM Cortex-M Mikrocontrollern,
der durch ungültige Speicherzugriffe, fehlerhafte Instruktionen, Division durch Null
oder Bus-Fehler ausgelöst wird. Ohne eigenen Handler endet ein HardFault in einer
Endlosschleife, ohne jegliche Diagnosemöglichkeit.
 
Diese Bibliothek fängt den HardFault in einem Assembler-Prolog ab, sichert den
Stack-Pointer korrekt (MSP oder PSP je nach Ausführungskontext) und übergibt die
Registerwerte an `ReportHardFault()`. Dort werden alle SCB-Statusregister ausgelesen,
Bit für Bit ausgewertet und als verständliche Fehlermeldung über UART ausgegeben.
 
## Dateien
 
| Datei           | Beschreibung                                                              |
|-----------------|---------------------------------------------------------------------------|
| `hardfault.h`   | Bitmasken für HFSR, CFSR (MMFSR, BFSR, UFSR) und Funktionsdeklaration    |
| `hardfault.c`   | Assembler-Prolog, Stack-Sicherung und vollständige Fehlerausgabe          |
 
## Ausgegebene Informationen
 
### Stack-Frame
 
Bei einem HardFault sichert der Cortex-M-Kern automatisch folgende Register auf dem Stack:
 
| Register | Beschreibung                        |
|----------|-------------------------------------|
| R0–R3    | Allgemeine Register                 |
| R12      | Allgemeines Register                |
| LR       | Link Register (Rücksprungadresse)   |
| PC       | Program Counter (Fehleradresse)     |
| PSR      | Program Status Register             |
 
### Fault-Statusregister
 
| Register | Beschreibung                                                        |
|----------|---------------------------------------------------------------------|
| `HFSR`   | HardFault Status Register                                           |
| `CFSR`   | Configurable Fault Status Register (enthält MMFSR, BFSR, UFSR)     |
| `MMAR`   | MemManage Fault Address Register (Adresse des ungültigen Zugriffs)  |
| `BFAR`   | Bus Fault Address Register (Adresse des Bus-Fehlers)                |
| `AFSR`   | Auxiliary Fault Status Register                                     |
| `EXC_RETURN` | Ausführungskontext beim Auftreten des Fehlers                   |
 
### Detailierte Fehlerauswertung
 
#### HardFault Status (HFSR)
 
| Bit        | Beschreibung                                       |
|------------|----------------------------------------------------|
| `FORCED`   | Eskalierter Fault von MMFSR, BFSR oder UFSR        |
| `VECTTBL`  | Bus-Fehler beim Lesen der Vektortabelle            |
 
#### MemManage Fault Status (MMFSR, Bits 0–7 in CFSR)
 
| Bit          | Beschreibung                                                         |
|--------------|----------------------------------------------------------------------|
| `IACCVIOL`   | Instruction-Fetch aus nicht ausführbarem Speicherbereich             |
| `DACCVIOL`   | Datenzugriff auf nicht erlaubten Speicherbereich                     |
| `MUNSTKERR`  | Zugriffsverletzung beim Unstacking (Exception-Rückkehr)              |
| `MSTKERR`    | Zugriffsverletzung beim Stacking (Exception-Eintritt)                |
| `MLSPERR`    | Fehler beim Lazy-Speichern des FPU-Zustands *(nur Cortex-M4F)*       |
| `MMARVALID`  | MMAR enthält die gültige Fehleradresse                               |
 
#### Bus Fault Status (BFSR, Bits 8–15 in CFSR)
 
| Bit            | Beschreibung                                                                    |
|----------------|---------------------------------------------------------------------------------|
| `IBUSERR`      | Instruction-Bus-Fehler                                                          |
| `PRECISERR`    | Präziser Datenbustfehler – PC zeigt auf die fehlerhafte Instruktion             |
| `IMPRECISERR`  | Impräziser Datenbustfehler – PC zeigt nicht direkt auf die fehlerhafte Stelle   |
| `UNSTKERR`     | Bus-Fehler beim Unstacking                                                      |
| `STKERR`       | Bus-Fehler beim Stacking                                                        |
| `LSPERR`       | Fehler beim Lazy-Speichern des FPU-Zustands *(nur Cortex-M4F)*                  |
| `BFARVALID`    | BFAR enthält die gültige Fehleradresse                                          |
 
#### Usage Fault Status (UFSR, Bits 16–25 in CFSR)
 
| Bit           | Beschreibung                                                              |
|---------------|---------------------------------------------------------------------------|
| `UNDEFINSTR`  | Undefinierter Befehl ausgeführt                                           |
| `INVSTATE`    | Illegale Nutzung des EPSR (z. B. Thumb-Bit nicht gesetzt)                 |
| `INVPC`       | Ungültiger Wert in PC geladen (EXC_RETURN)                                |
| `NOCP`        | Zugriff auf nicht vorhandenen Koprozessor                                 |
| `UNALIGNED`   | Nicht ausgerichteter Speicherzugriff *(muss im CCR aktiviert sein)*       |
| `DIVBYZERO`   | Division durch Null (SDIV/UDIV) *(muss im CCR aktiviert sein)*            |
 
## Verwendung
 
### 1. Dateien einbinden
 
`hardfault.h` und `hardfault.c` in das STM32-Projekt kopieren. Den bestehenden
`HardFault_Handler` in der von CubeMX generierten Datei `stm32xxxx_it.c`
**löschen oder auskommentieren**, da er durch diese Bibliothek ersetzt wird.
 
Den Header einbinden:
 
```c
#include "hardfault.h"
```
 
### 2. Verhalten im Debug-Build
 
Ist das Symbol `DEBUG` definiert, löst der Handler am Ende der Ausgabe automatisch einen
Software-Breakpoint (`BKPT #0`) aus. Der Debugger hält dann direkt an, und die UART-Ausgabe
liefert alle nötigen Informationen zur Fehleranalyse.
 
Im Release-Build (ohne `DEBUG`) läuft der Handler nach der Ausgabe in eine Endlosschleife.
 
### 3. Beispiel-Ausgabe
 
```
!!!Hard Fault detected!!!
 
Stack frame:
R0 : 0x00000000
R1 : 0x20001234
R2 : 0x00000000
R3 : 0x00000000
R12: 0x00000000
LR : 0x0800abcd
PC : 0x0800ef12
PSR: 0x61000000
 
Fault status:
HFSR: 0x40000000
CFSR: 0x00000400
MMAR: 0x00000000
BFAR: 0x00000000
AFSR: 0x00000000
 
Details of the fault status:
Hard fault status:
 - Forced Hard fault.
Bus fault status:
 - BFAR holds an invalid address.
 - Instruction bus error.
```
 
## Hinweise
 
- Der `PC`-Wert im Stack-Frame zeigt auf die Instruktion, die den Fehler ausgelöst hat.
In Kombination mit der `.map`-Datei oder dem Disassembler in STM32CubeIDE lässt sich die
genaue Codezeile ermitteln.
- `DIVBYZERO` und `UNALIGNED` werden nur erkannt, wenn sie im `CCR`-Register (Configuration
and Control Register) aktiviert sind.
- `MLSPERR` und `LSPERR` sind nur auf Cortex-M4F und Cortex-M7 (mit FPU) relevant.

## Abhängigkeiten
 
- `main.h` – STM32 HAL
- [`basicuart.h`](https://github.com/Diveturtle93/STM32_Basicuart) – UART-Sende- und Empfangsfunktionen

## Lizenz
 
Dieses Projekt steht unter der [GPL-3.0 Lizenz](LICENSE).
