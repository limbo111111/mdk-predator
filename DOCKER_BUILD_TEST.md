# Docker Build Test für Mayhem v2.3.1

Diese Anleitung beschreibt, wie Sie den mdk-predator mit Docker gegen Mayhem v2.3.1 bauen und testen.

## Voraussetzungen

- **Docker Desktop** installiert und laufend
  - Windows: https://docs.docker.com/desktop/install/windows-install/
  - macOS: https://docs.docker.com/desktop/install/mac-install/
  - Linux: `apt install docker.io docker-compose` (oder äquivalent)

Überprüfen Sie die Installation:
```bash
docker --version
docker-compose --version
```

## Schritt 1: Repository klonen

```bash
git clone https://github.com/limbo111111/mdk-predator.git
cd mdk-predator
```

## Schritt 2: Docker Build gegen Mayhem v2.3.1

### Option A: Standard-Build (empfohlen)

```bash
./docker-build.sh --mayhem-version 2.3.1 --clean
```

**Was passiert:**
1. Docker-Image wird erstellt (enthält ARM Toolchain, CMake, libopencm3)
2. Mayhem-Firmware v2.3.1 wird geklont
3. mdk-predator wird in `mayhem-firmware/firmware/application/external/mdk_predator/` integriert
4. `external.cmake` wird aktualisiert (Quellen registriert)
5. libopencm3 wird gebaut
6. mdk-predator wird kompiliert
7. `.ppma` Datei wird erzeugt

**Erwartete Dauer:** 15-25 Minuten (erste Build), 3-5 Minuten (Wiederholungen)

### Option B: Mit Ninja (schneller)

```bash
./docker-build.sh --mayhem-version 2.3.1 --clean --ninja
```

### Option C: Firmware aktualisieren + neu bauen

```bash
./docker-build.sh --mayhem-version 2.3.1 --update-firmware --clean
```

### Option D: Debug-Shell öffnen

Falls der Build fehlschlägt, öffnen Sie eine interaktive Shell:

```bash
./docker-build.sh --shell
```

Dann im Container:
```bash
cd /workspace/mayhem-firmware
make -j4 application  # Rebuild manuell
```

## Schritt 3: Build-Ergebnis überprüfen

Nach erfolgreichem Build sollte die `.ppma` Datei vorhanden sein:

```bash
ls -lh build/docker-output/
```

Erwartete Ausgabe:
```
-rw-r--r-- 1 user group 2.3M Nov 13 12:34 mdk_predator.ppma
-rw-r--r-- 1 user group  4.2K Nov 13 12:34 mdk_predator.conf
```

## Schritt 4: Auf SD-Karte deployen

1. SD-Karte einstecken (oder Image mounen)
2. Dateien kopieren:

```bash
# Auf Linux/macOS (anpassen Sie /Volumes/SDCARD/ oder /mnt/sdcard/)
cp build/docker-output/mdk_predator.ppma /Volumes/SDCARD/APPS/
cp mdk_predator.conf /Volumes/SDCARD/MDK-PREDATOR/config/

# Auf Windows (PowerShell als Admin):
# Copy-Item "build/docker-output/mdk_predator.ppma" "E:\APPS\"
# Copy-Item "mdk_predator.conf" "E:\MDK-PREDATOR\config\"
```

3. SD-Karte auswerfen
4. In PortaPack H4M einlegen
5. Im Mayhem-UI: `Apps → Utilities → MDK-Predator`

## Troubleshooting

### Fehler: "Docker not found"
- Docker Desktop ist nicht installiert oder nicht laufend
- **Lösung:** https://docs.docker.com/get-docker/

### Fehler: "Permission denied while trying to connect to Docker daemon"
```bash
# Linux: zu docker-Gruppe hinzufügen
sudo usermod -aG docker $USER
newgrp docker

# macOS: Docker Desktop neu starten
```

### Fehler: "mayhem-firmware clone failed"
- Netzwerk-Problem oder GitHub nicht erreichbar
- **Lösung:** `--update-firmware` weglassen, lokales Clone verwenden

### Fehler: "CMake not found" oder "ARM toolchain not found"
- Docker-Image-Build fehlgeschlagen
- **Lösung:** Docker-Image neu bauen:
```bash
./docker-build.sh --rebuild --clean
```

### Fehler: "libopencm3 make failed"
- libopencm3-Dependency nicht vorhanden
- **Lösung:** Im `docker-entrypoint.sh` wird automatisch gefetcht, falls nötig

```bash
# Falls manuell nötig:
cd /workspace/mayhem-firmware/firmware/libopencm3
make TARGETS=lpc43xx
```

### Build dauert sehr lange
- Erste Build dauert 20-30 Minuten (normal)
- Wiederholungen sollten 3-5 Minuten sein
- **Falls viel länger:** Disk-Space prüfen (`docker system df`)

## Umwelt-Variablen

Im `docker-build.sh` können Sie diese Variablen setzen:

```bash
# Mayhem-Version/Branch/Channel explizit setzen
MAYHEM_VERSION=2.3.1 ./docker-build.sh --clean

# Mayhem-Nightly verwenden
./docker-build.sh --nightly --clean

# Firmware-Update erzwingen
./docker-build.sh --update-firmware --clean

# Clean Build Output
./docker-build.sh --clean
```

## Datei-Struktur nach Build

```
build/
├── docker-output/
│   ├── mdk_predator.ppma    ← Finale PortaPack App
│   └── mdk_predator.conf    ← Konfiguration
├── mayhem-firmware/         ← Geklontes Mayhem Repo
│   └── firmware/
│       └── application/
│           └── external/
│               └── mdk_predator/  ← Unsere App kopiert
└── output/                  ← Docker Build-Output
```

## Validierung nach Build

Um zu überprüfen, dass die `.ppma` Datei korrekt ist:

```bash
# Datei-Größe prüfen (sollte 1-3 MB sein)
ls -lh build/docker-output/mdk_predator.ppma

# Ob es ein ELF-Binary ist
file build/docker-output/mdk_predator.ppma

# Symbole prüfen (optional)
arm-none-eabi-objdump -d build/docker-output/mdk_predator.ppma | head -50
```

## Nächste Schritte

1. ✅ Docker-Build erfolgreich
2. 📦 `mdk_predator.ppma` auf SD-Karte kopieren
3. 🔌 PortaPack H4M mit Mayhem v2.3.1 flashen
4. ▶️ App starten und Tests durchführen

---

**Hinweis:** Diese Build-Konfiguration ist für Mayhem v2.3.1 optimiert. Bei Änderungen der Mayhem-API sollte `docker-entrypoint.sh` aktualisiert werden.
