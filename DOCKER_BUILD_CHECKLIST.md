# MDK-Predator Docker Build Test Checklist

## Phase 1: Vorbereitung ✓

- [ ] Docker Desktop installiert und laufend (`docker --version`)
- [ ] Repository geklont: `git clone ... && cd mdk-predator`
- [ ] Alle Commits gepullt: `git pull origin main`
- [ ] `docker-build.sh` ist ausführbar: `chmod +x docker-build.sh`

## Phase 2: Docker Build gegen Mayhem v2.3.1

### Build-Befehl
```bash
./docker-build.sh --mayhem-version 2.3.1 --clean
```

### Erwartete Ausgaben während des Build:

- [ ] `[INFO] Docker is installed` ✓
- [ ] `[INFO] MDK-Predator Docker Build` ✓
- [ ] `==> Cloning mayhem-firmware...` (oder `[INFO] mayhem-firmware already present`) ✓
- [ ] `==> Requested mayhem version: 2.3.1` ✓
- [ ] `[INFO] Using mayhem-firmware commit: <HASH>` ✓
- [ ] `==> Building libopencm3...` ✓
- [ ] `[INFO] libopencm3 build completed` ✓
- [ ] `==> Building mdk-predator application...` ✓
- [ ] `==> Extracting .ppma file...` ✓
- [ ] `[INFO] Build completed!` ✓

### Build-Fehler-Szenarien

Falls der Build fehlschlägt:

1. **CMake-Fehler**
   ```bash
   ./docker-build.sh --rebuild --clean
   ```

2. **libopencm3-Fehler**
   ```bash
   ./docker-build.sh --shell
   # Im Container:
   cd /workspace/mayhem-firmware/firmware/libopencm3
   make TARGETS=lpc43xx -j4
   ```

3. **Mayhem-Repo-Fehler**
   ```bash
   ./docker-build.sh --update-firmware --clean
   ```

## Phase 3: Build-Artefakte überprüfen

### Datei-Validierung

```bash
# Überprüfe Output-Verzeichnis
ls -lh build/docker-output/
```

- [ ] `mdk_predator.ppma` existiert
- [ ] Dateigröße: 1-3 MB (normal)
- [ ] `mdk_predator.conf` existiert
- [ ] Beide Dateien sind lesbar

### Binary-Validierung (optional)

```bash
# Überprüfe ob es ein ARM ELF-Binary ist
file build/docker-output/mdk_predator.ppma

# Sollte ausgeben: ELF 32-bit LSB executable, ARM, ...
```

### Symbolprüfung (optional)

```bash
# Überprüfe Funktionssymbole
arm-none-eabi-objdump -t build/docker-output/mdk_predator.ppma | grep -E "wifi_|bluetooth_|subghz_"

# Sollte unsere Wireless-Funktionen zeigen:
# - wifi_analyzer_init
# - wifi_scan_networks
# - bluetooth_analyzer_init
# - bluetooth_scan_devices
# - subghz_analyzer_init
# - subghz_scan_spectrum
```

## Phase 4: Deployment auf SD-Karte

### Datei kopieren

**Linux/macOS:**
```bash
# SD-Karte mounten (normalerweise /Volumes/SDCARD oder /mnt/sdcard)
cp build/docker-output/mdk_predator.ppma /Volumes/SDCARD/APPS/
cp mdk_predator.conf /Volumes/SDCARD/MDK-PREDATOR/config/
```

**Windows (PowerShell als Admin):**
```powershell
Copy-Item "build/docker-output/mdk_predator.ppma" "E:\APPS\"
Copy-Item "mdk_predator.conf" "E:\MDK-PREDATOR\config\"
```

- [ ] Dateien auf SD-Karte kopiert
- [ ] SD-Karte ausgeworfen
- [ ] SD-Karte sicher in PortaPack H4M eingelegt

## Phase 5: Test auf PortaPack H4M (mit Mayhem v2.3.1)

### App-Start

- [ ] PortaPack angeschlossen und eingeschaltet
- [ ] Mayhem v2.3.1 läuft
- [ ] Im Menü: `Apps → Utilities → MDK-Predator`
- [ ] App startet ohne Fehler
- [ ] GUI wird angezeigt

### Grundfunktionalität testen

#### WiFi Analyzer
- [ ] App-Reiter "WiFi" vorhanden
- [ ] "Scan Networks" Button funktioniert
- [ ] Netzwerke werden angezeigt
- [ ] Signal-Stärke (RSSI) angezeigt
- [ ] Sicherheitstyp erkannt (WPA2/WPA3/Open)

#### Bluetooth Analyzer
- [ ] App-Reiter "Bluetooth" vorhanden
- [ ] "Scan Devices" Button funktioniert
- [ ] BT-Geräte werden erkannt
- [ ] Gerätenamen und MACs angezeigt
- [ ] RSSI wird angezeigt

#### SubGHz Analyzer
- [ ] App-Reiter "SubGHz" vorhanden
- [ ] "Scan Spectrum" Button funktioniert
- [ ] Frequenzen werden gescannt
- [ ] Signal-Peaks werden erkannt
- [ ] Modulation erkannt (ASK/FSK/etc.)

### Keine Crash-Tests

- [ ] App lädt Konfiguration korrekt
- [ ] Keine Segfaults oder Hangs
- [ ] Debug-Logging funktioniert (falls aktiviert)
- [ ] App kann beendet werden (kein Freeze)

## Phase 6: Build-Artefakte archivieren

```bash
# Backup für späteren Vergleich
mkdir -p build/releases/v2.3.1
cp build/docker-output/mdk_predator.ppma build/releases/v2.3.1/
cp build/docker-output/mdk_predator.conf build/releases/v2.3.1/
```

- [ ] Release-Dateien gesichert

## Ergebnisse

### ✅ Erfolgreich
- [ ] Docker-Build kompiliert ohne Fehler
- [ ] `.ppma` Datei erzeugt
- [ ] Binary lädt auf PortaPack
- [ ] Alle Module ansprechbar
- [ ] Keine Crash-Fehler

### ⚠️ Probleme
- [ ] Dokumentieren und in `DOCKER_BUILD_TEST_ISSUES.md` notieren
- [ ] GitHub Issue erstellen mit Fehler-Logs

## Build-Validierungs-Summary

| Schritt | Status | Notizen |
|---------|--------|---------|
| Docker Setup | ✓/✗ | |
| Mayhem v2.3.1 Clone | ✓/✗ | |
| libopencm3 Build | ✓/✗ | |
| mdk-predator Compile | ✓/✗ | |
| .ppma Extraction | ✓/✗ | |
| File Validation | ✓/✗ | |
| Deployment to SD | ✓/✗ | |
| PortaPack Launch | ✓/✗ | |
| WiFi Module Test | ✓/✗ | |
| Bluetooth Module Test | ✓/✗ | |
| SubGHz Module Test | ✓/✗ | |

---

**Kontakt:** Falls Fehler auftreten, GitHub Issues mit `[DOCKER-BUILD]` Tag erstellen.
