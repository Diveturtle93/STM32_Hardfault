# Changelog

Alle nennenswerten Änderungen an diesem Projekt werden hier dokumentiert.
Format nach [Keep a Changelog](https://keepachangelog.com/de/1.1.0/),
Versionierung nach [Semantic Versioning](https://semver.org/lang/de/).

## [Unveröffentlicht]

## [1.0.0] - 2026-11-01

Erste Veröffentlichung.

### Hinzugefügt
- `HardFault_Handler()` ersetzt den HardFault-Handler der HAL und wählt per `EXC_RETURN` den beim Fehler aktiven Stack-Pointer (`MSP` oder `PSP`). Der Handler ist `naked`, damit der Stack-Frame auch bei `-O0` richtig gelesen wird.
- Ausgabe des Stack-Frames (R0–R3, R12, LR, PC, PSR) über UART.
- Ausgabe der Fault-Statusregister `HFSR`, `CFSR`, `MMAR`, `BFAR`, `AFSR` und von `EXC_RETURN` über UART.
- `ReportHardFault()` wertet den Stack-Frame und die Fault-Register aus und gibt sie aus.
- Klartextauswertung aller Fehlerbits aus HFSR (`DEBUGEVT`, `FORCED`, `VECTTBL`), MMFSR, BFSR und UFSR.
- Im Debug-Build (`DEBUG_HARDFAULT`, gesetzt über `DEBUG`) hält ein Software-Breakpoint den Debugger nach der Ausgabe an.
- Versions-Defines `HARDFAULT_MAJOR`, `_MINOR`, `_PATCH` und `_DEV` in `hardfault.h`.
- README mit Beschreibung, Fehlerbildern, API, Verwendung und unterstützten Kernen.
- Dieses Changelog.

[Unveröffentlicht]: https://github.com/Diveturtle93/STM32_Hardfault/compare/v1.0.0...HEAD
[1.0.0]: https://github.com/Diveturtle93/STM32_Hardfault/releases/tag/v1.0.0
