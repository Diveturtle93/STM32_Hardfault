# STM32 Hardfault Handler

Eine in C implementierte Bibliothek für STM32-Mikrocontroller, die den Standard-HardFault-Handler
der HAL ersetzt. Bei einem HardFault gibt sie den Stack-Frame, die Fault-Statusregister und eine
Klartextbeschreibung der Fehlerursache über UART aus. Im Debug-Build löst sie zusätzlich einen
Software-Breakpoint aus, damit der Debugger direkt an der Fehlerstelle anhält.

## Beschreibung

Ein HardFault ist ein schwerwiegender CPU-Fehler auf ARM-Cortex-M-Mikrocontrollern, ausgelöst
z. B. durch ungültige Speicherzugriffe, fehlerhafte Instruktionen, Division durch Null oder
Bus-Fehler. Ohne eigenen Handler endet ein HardFault in einer Endlosschleife, ohne jede
Diagnosemöglichkeit.

Die Bibliothek fängt den HardFault in einem Assembler-Prolog ab. Der Handler ist als
`__attribute__((naked))` deklariert, damit der Compiler keinen eigenen Prolog davorsetzt, der
den Stack-Pointer verschiebt. Anhand von Bit 2 in
`EXC_RETURN` (`LR`) wählt sie den Stack-Pointer, der beim Fehler aktiv war (`MSP` oder `PSP`),
und übergibt ihn zusammen mit `EXC_RETURN` an `ReportHardFault()`. Dort werden die
SCB-Statusregister ausgelesen, Bit für Bit ausgewertet und als Fehlermeldung über UART ausgegeben.

Solange MemManage-, BusFault- und UsageFault-Handler nicht im `SHCSR` aktiviert sind, eskalieren
diese Fehler zum HardFault. `HFSR.FORCED` ist dann gesetzt, und die eigentliche Ursache steht im
`CFSR`.

## Dateien

| Datei         | Beschreibung                                                                            |
|---------------|-----------------------------------------------------------------------------------------|
| `hardfault.h` | Debug-Symbol, Version, Bitpositionen für HFSR und CFSR (MMFSR, BFSR, UFSR), Prototyp     |
| `hardfault.c` | Assembler-Prolog, Auswahl des Stack-Pointers und Fehlerausgabe über UART                 |

## Konfigurationsparameter

| Symbol            | Wirkung                                                                                     |
|-------------------|---------------------------------------------------------------------------------------------|
| `DEBUG`           | Setzt in `hardfault.h` das Symbol `DEBUG_HARDFAULT`. In STM32CubeIDE im Debug-Build gesetzt  |
| `DEBUG_HARDFAULT` | Nach der Ausgabe wird ein Software-Breakpoint (`BKPT #0`) ausgelöst                          |

Ohne `DEBUG_HARDFAULT` läuft der Handler nach der Ausgabe in eine Endlosschleife.

## Ausgegebene Informationen

### Stack-Frame

Beim Eintritt in die Exception sichert der Cortex-M-Kern automatisch folgende Register auf dem Stack:

| Register | Beschreibung                      |
|----------|-----------------------------------|
| R0–R3    | Allgemeine Register               |
| R12      | Allgemeines Register              |
| LR       | Link Register (Rücksprungadresse) |
| PC       | Program Counter (Fehleradresse)   |
| PSR      | Program Status Register           |

### Fault-Statusregister

| Register     | Beschreibung                                                       |
|--------------|--------------------------------------------------------------------|
| `HFSR`       | HardFault Status Register                                          |
| `CFSR`       | Configurable Fault Status Register (enthält MMFSR, BFSR, UFSR)     |
| `MMAR`       | MemManage Fault Address Register (Adresse des ungültigen Zugriffs) |
| `BFAR`       | Bus Fault Address Register (Adresse des Bus-Fehlers)               |
| `AFSR`       | Auxiliary Fault Status Register                                    |
| `EXC_RETURN` | Ausführungskontext beim Auftreten des Fehlers (Ausgabe unter `Other`) |

### Detaillierte Fehlerauswertung

#### HardFault Status (HFSR)

| Bit       | Beschreibung                                |
|-----------|---------------------------------------------|
| `FORCED`  | Eskalierter Fault von MMFSR, BFSR oder UFSR |
| `VECTTBL` | Bus-Fehler beim Lesen der Vektortabelle     |

#### MemManage Fault Status (MMFSR, Bits 0–7 in CFSR)

| Bit         | Beschreibung                                                  |
|-------------|---------------------------------------------------------------|
| `IACCVIOL`  | Instruction-Fetch aus nicht ausführbarem Speicherbereich      |
| `DACCVIOL`  | Datenzugriff auf nicht erlaubten Speicherbereich              |
| `MUNSTKERR` | Zugriffsverletzung beim Unstacking (Exception-Rückkehr)       |
| `MSTKERR`   | Zugriffsverletzung beim Stacking (Exception-Eintritt)          |
| `MLSPERR`   | Fehler beim Lazy-Speichern des FPU-Zustands *(nur mit FPU)*   |
| `MMARVALID` | MMAR enthält die gültige Fehleradresse                        |

#### Bus Fault Status (BFSR, Bits 8–15 in CFSR)

| Bit           | Beschreibung                                                                |
|---------------|-----------------------------------------------------------------------------|
| `IBUSERR`     | Instruction-Bus-Fehler                                                      |
| `PRECISERR`   | Präziser Datenbusfehler – PC zeigt auf die fehlerhafte Instruktion          |
| `IMPRECISERR` | Impräziser Datenbusfehler – PC zeigt nicht direkt auf die fehlerhafte Stelle |
| `UNSTKERR`    | Bus-Fehler beim Unstacking                                                  |
| `STKERR`      | Bus-Fehler beim Stacking                                                    |
| `LSPERR`      | Fehler beim Lazy-Speichern des FPU-Zustands *(nur mit FPU)*                 |
| `BFARVALID`   | BFAR enthält die gültige Fehleradresse                                      |

#### Usage Fault Status (UFSR, Bits 16–25 in CFSR)

| Bit          | Beschreibung                                                        |
|--------------|---------------------------------------------------------------------|
| `UNDEFINSTR` | Undefinierter Befehl ausgeführt                                     |
| `INVSTATE`   | Illegale Nutzung des EPSR (z. B. Thumb-Bit nicht gesetzt)           |
| `INVPC`      | Ungültiger Wert in PC geladen (EXC_RETURN)                          |
| `NOCP`       | Zugriff auf nicht vorhandenen Koprozessor                           |
| `UNALIGNED`  | Nicht ausgerichteter Speicherzugriff *(muss im CCR aktiviert sein)* |
| `DIVBYZERO`  | Division durch Null (SDIV/UDIV) *(muss im CCR aktiviert sein)*      |

### Beispiel-Ausgabe

Impräziser Bus-Fehler, zum HardFault eskaliert (`HFSR.FORCED`, `CFSR.IMPRECISERR`):

```
!!!Hard Fault detected!!!

Stack frame:
R0 :        0x00000000
R1 :        0x20001234
R2 :        0x00000000
R3 :        0x00000000
R12:        0x00000000
LR :        0x0800ABCD
PC :        0x0800EF12
PSR:        0x61000000

Fault status:
HFSR:       0x40000000
CFSR:       0x00000400
MMAR:       0x00000000
BFAR:       0x00000000
AFSR:       0x00000000

Other:
EXC_RETURN: 0xFFFFFFF9

Details of the fault status:
Hard fault status:
 - Forced Hard fault.
MemManage fault status:
 - MMAR holds an invalid address.
Bus fault status:
 - BFAR holds an invalid address.
 - Data bus error has occurred, but the return address in the stack is not related to the fault.
Usage fault status:
```

## Fehlerbilder

| Symptom | Ursache | Abhilfe |
|---------|---------|---------|
| Linker meldet `multiple definition of 'HardFault_Handler'` | Der von CubeMX erzeugte Handler in `stm32xxxx_it.c` ist noch vorhanden | Handler löschen und in CubeMX unter NVIC → Code generation für „Hard fault interrupt“ „Generate IRQ handler“ abwählen |
| Fehler kommt nach dem Neugenerieren mit CubeMX wieder | CubeMX legt den Handler bei jeder Generierung neu an | In CubeMX „Generate IRQ handler“ für den Hard fault abwählen |
| Keine Ausgabe auf UART | UART beim Fehler noch nicht initialisiert, z. B. Fehler vor `MX_USARTx_Init()` | Fehlerstelle mit dem Debugger eingrenzen, UART möglichst früh initialisieren |
| Board bleibt ohne angeschlossenen Debugger hängen | Debug-Build: `BKPT #0` ohne Debugger führt im HardFault zum Lockup | Release-Build ohne `DEBUG` auf das Board spielen |
| `DIVBYZERO` bzw. `UNALIGNED` wird nie gemeldet | Trap im `CCR` nicht aktiviert | Trap nach Schritt 3 der Verwendung aktivieren |

## API

```c
void HardFault_Handler (void);    // Ersetzt den HardFault-Handler der HAL, wird von der CPU aufgerufen
```

`HardFault_Handler()` wird nicht aus der Anwendung aufgerufen. Der Handler steht über seinen Namen
in der Vektortabelle des Startup-Codes.

## Verwendung

### 1. Dateien einbinden

`hardfault.h` und `hardfault.c` in das STM32-Projekt kopieren und den Header einbinden:

```c
#include "hardfault.h"
```

### 2. CubeMX-Handler entfernen

Den bestehenden `HardFault_Handler` in der von CubeMX erzeugten Datei `stm32xxxx_it.c` löschen
oder auskommentieren, da er durch diese Bibliothek ersetzt wird. Damit CubeMX ihn nicht neu
anlegt, unter NVIC → Code generation für „Hard fault interrupt“ „Generate IRQ handler“ abwählen.

### 3. Traps aktivieren (optional)

Division durch Null und nicht ausgerichtete Zugriffe lösen nur dann einen Fault aus, wenn sie im
`CCR` aktiviert sind:

```c
SCB->CCR |= SCB_CCR_DIV_0_TRP_Msk | SCB_CCR_UNALIGN_TRP_Msk;
```

## Hinweise

- Der `PC`-Wert im Stack-Frame zeigt bei präzisen Fehlern auf die Instruktion, die den Fehler
  ausgelöst hat. Mit der `.map`-Datei oder dem Disassembler in STM32CubeIDE lässt sich die
  Codezeile ermitteln.
- `EXC_RETURN` zeigt, in welchem Kontext der Fehler auftrat, z. B. `0xFFFFFFF9` (Thread-Modus,
  `MSP`) oder `0xFFFFFFFD` (Thread-Modus, `PSP`, z. B. in einem RTOS-Task).
- `MLSPERR` und `LSPERR` sind nur auf Kernen mit FPU relevant (Cortex-M4F, Cortex-M7).

## Unterstützte Kerne

Cortex-M3, Cortex-M4 und Cortex-M7 (ARMv7-M). Cortex-M0 und Cortex-M0+ (ARMv6-M) werden nicht
unterstützt, da ihnen der `IT`-Befehl und die Fault-Statusregister `CFSR`, `MMAR` und `BFAR` fehlen.

## Abhängigkeiten

- `main.h` – STM32 HAL (CMSIS, `SCB`)
- [`basicuart.h`](https://github.com/Diveturtle93/STM32_Basicuart) – UART-Sendefunktionen `uartTransmit()` und `uartTransmitNumber()`

## Lizenz

Dieses Projekt steht unter der [GPL-3.0 Lizenz](LICENSE).
