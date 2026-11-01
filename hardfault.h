//----------------------------------------------------------------------
// Titel	:	hardfault.h
//----------------------------------------------------------------------
// Sprache	:	C
// Datum	:	16.05.2025
// Autor	:	Diveturtle93
// Projekt	:	Hardfault
// Quelle	:
//----------------------------------------------------------------------

// Sicherheitssymbol
//----------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------

// Dateiheader definieren
//----------------------------------------------------------------------
#ifndef INC_HARDFAULT_H_
#define INC_HARDFAULT_H_
//----------------------------------------------------------------------

// Einfuegen der standard Include-Dateien
//----------------------------------------------------------------------

//----------------------------------------------------------------------

// Einfuegen der STM Include-Dateien
//----------------------------------------------------------------------
#include "main.h"
//----------------------------------------------------------------------

// Einfuegen der eigenen Include-Dateien
//----------------------------------------------------------------------

//----------------------------------------------------------------------

// Definiere Debug-Symbole
//----------------------------------------------------------------------
#ifdef DEBUG
	#define DEBUG_HARDFAULT
#endif
//----------------------------------------------------------------------

// Version definieren
//----------------------------------------------------------------------
#define HARDFAULT_MAJOR							1
#define HARDFAULT_MINOR							0
#define HARDFAULT_PATCH							0
#define HARDFAULT_DEV							0
//----------------------------------------------------------------------

// Bitmaske definieren
//----------------------------------------------------------------------
#define CHECK_BIT(REG, POS)						((REG) & (1u << (POS)))
//----------------------------------------------------------------------

// Hard Fault Status Register
//----------------------------------------------------------------------
#define VECTTBL									((uint8_t)1u)				// Busfehler beim Lesen der Vektortabelle (Exception-Eintritt)
#define FORCED									((uint8_t)30u)				// Eskalierter Fault (MemManage, Bus, Usage), Ursache steht im CFSR
#define DEBUGEVT								((uint8_t)31u)				// Debug-Ereignis ohne aktiven Debugger (z.B. BKPT), eskaliert zum HardFault
//----------------------------------------------------------------------

// MemManage Fault Status Register (MMFSR; 0-7 bits in CFSR)
//----------------------------------------------------------------------
#define IACCVIOL								((uint8_t)0u)				// Befehlsabruf aus nicht ausfuehrbarem Bereich (XN)
#define DACCVIOL								((uint8_t)1u)				// Datenzugriff auf geschuetzten Bereich, Adresse in MMFAR
#define MUNSTKERR								((uint8_t)3u)				// MPU-Verletzung beim Unstacking (Exception-Ruecksprung)
#define MSTKERR									((uint8_t)4u)				// MPU-Verletzung beim Stacking (Exception-Eintritt)
#define MLSPERR									((uint8_t)5u)				// MPU-Verletzung beim Lazy-FP-Stacking, nur mit FPU (Cortex-M4F, M7)
#define MMARVALID								((uint8_t)7u)				// MMFAR enthaelt gueltige Fehleradresse
//----------------------------------------------------------------------

// Bus Fault Status Register (BFSR; 8-15 bits in CFSR)
//----------------------------------------------------------------------
#define IBUSERR									((uint8_t)8u)				// Busfehler beim Befehlsabruf
#define PRECISERR								((uint8_t)9u)				// Praeziser Datenbusfehler, Adresse in BFAR, PC zeigt auf Befehl
#define IMPRECISERR								((uint8_t)10u)				// Unpraeziser Datenbusfehler (gepufferter Schreibzugriff), keine Adresse
#define UNSTKERR								((uint8_t)11u)				// Busfehler beim Unstacking (Exception-Ruecksprung)
#define STKERR									((uint8_t)12u)				// Busfehler beim Stacking (Exception-Eintritt)
#define LSPERR									((uint8_t)13u)				// Busfehler beim Lazy-FP-Stacking, nur mit FPU (Cortex-M4F, M7)
#define BFARVALID								((uint8_t)15u)				// BFAR enthaelt gueltige Fehleradresse
//----------------------------------------------------------------------

// Usage Fault Status Register (UFSR; 16-25 bits in CFSR)
//----------------------------------------------------------------------
#define UNDEFINSTR								((uint8_t)16u)				// Undefinierter Befehl
#define INVSTATE								((uint8_t)17u)				// Ungueltiger Ausfuehrungszustand (Thumb-Bit = 0, z.B. Sprung auf gerade Adresse)
#define INVPC									((uint8_t)18u)				// Ungueltiger EXC_RETURN-Wert beim Exception-Ruecksprung
#define NOCP									((uint8_t)19u)				// Coprozessor nicht freigegeben (z.B. FPU-Befehl ohne Freigabe in CPACR)
#define UNALIGNED								((uint8_t)24u)				// Nicht ausgerichteter Zugriff, muss in CCR aktiviert sein (UNALIGN_TRP)
#define DIVBYZERO								((uint8_t)25u)				// Division durch Null (SDIV, UDIV), muss in CCR aktiviert sein (DIV_0_TRP)
//----------------------------------------------------------------------

// Funktionen definieren
//----------------------------------------------------------------------
void HardFault_Handler (void);
void ReportHardFault (uint32_t *stack_frame, uint32_t exc);
//----------------------------------------------------------------------

#endif /* INC_HARDFAULT_H_ */
//----------------------------------------------------------------------
