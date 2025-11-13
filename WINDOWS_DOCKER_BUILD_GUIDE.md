# Docker Build für Windows - Mayhem v2.3.1

Diese Anleitung ist speziell für **Windows mit Docker Desktop** optimiert.

## ⚙️ Voraussetzungen

### 1. Docker Desktop installieren

1. **Download**: https://docs.docker.com/desktop/install/windows-install/
2. **Installation**: Installer ausführen und folgen Sie den Anweisungen
3. **WSL 2 Backend aktivieren** (wenn nicht automatisch aktiviert)
   - Windows-Startmenü → "Turn Windows features on or off"
   - Aktivieren Sie:
     - ☑ Hyper-V
     - ☑ Virtual Machine Platform
     - ☑ Windows Subsystem for Linux
   - Neustart erforderlich
4. **Docker Desktop starten**: Systemtablett (unten rechts) → Docker Desktop Icon

### 2. Git installieren (falls nicht vorhanden)

- **Download**: https://git-scm.com/download/win
- **Installation**: Standardoptionen ok

### 3. PowerShell Check

Öffnen Sie eine **PowerShell** (nicht CMD):
```powershell
docker --version
docker-compose --version
```

Erwartete Ausgabe:
```
Docker version 26.0.0, build 1efdf3f
Docker Compose version v2.24.0
```

## 📦 Repository klonen

Öffnen Sie PowerShell und führen aus:

```powershell
cd $env:USERPROFILE\Desktop
git clone https://github.com/limbo111111/mdk-predator.git
cd mdk-predator
```

## 🔨 Docker Build ausführen

### Option 1: Standard Build (empfohlen)

```powershell
.\docker-build.ps1 -Clean
```

**Was wird dabei ausgeführt:**
1. Docker-Image wird erstellt (enthält ARM Toolchain, CMake, libopencm3)
2. Mayhem-Firmware wird geklont
3. mdk-predator wird in die Firmware integriert
4. Build läuft
5. `.ppma` Datei wird erzeugt

**Erwartete Dauer:** 15-25 Minuten (erste Build), 3-5 Minuten (Wiederholungen)

### Option 2: Mit Mayhem v2.3.1 spezifisch

Um sicherzustellen, dass **genau v2.3.1** gebaut wird:

```powershell
$env:MAYHEM_VERSION = "2.3.1"
.\docker-build.ps1 -Clean
```

Oder direkt:
```powershell
# Temporär Environment-Variable setzen und bauen
cmd /c "set MAYHEM_VERSION=2.3.1 && docker-compose run --rm mdk-predator-build-clean"
```

### Option 3: Mit Ninja (schneller)

```powershell
.\docker-build.ps1 -Clean -Ninja
```

### Option 4: Firmware aktualisieren

```powershell
.\docker-build.ps1 -Clean -UpdateFirmware
```

## ✅ Build-Fortschritt überwachen

Während des Builds sehen Sie die Ausgabe live. Wichtige Indikatoren:

```
[INFO] MDK-Predator Docker Build
[INFO] Configuration:
  - Clean build: Yes
  - Update firmware: No
  - Build system: Make
  - Output directory: build\docker-output\
```

Dann:
```
==> Cloning mayhem-firmware...
==> Requested mayhem version: 2.3.1
[INFO] Using mayhem-firmware commit: <HASH>
==> Building libopencm3...
==> Building mdk-predator application...
==> Extracting .ppma file...
[INFO] Build completed!
```

## 📋 Build-Ergebnis überprüfen

Nach erfolgreichem Build:

```powershell
ls build\docker-output\
```

Erwartete Dateien:
```
    Directory: C:\Users\YourName\Desktop\mdk-predator\build\docker-output

Mode                 LastWriteTime         Length Name
----                 -------------         ------ ----
-a---          11/13/2025  12:34 PM        2.3MB mdk_predator.ppma
-a---          11/13/2025  12:34 PM        4.2KB mdk_predator.conf
```

## 🔴 Fehlerbehandlung für Windows

### Fehler: "Docker not found"

```powershell
docker --version
```

Falls Fehler:
- Docker Desktop ist nicht installiert
- Docker Desktop läuft nicht (starten Sie Docker Desktop aus dem Startmenü)
- PowerShell muss neu gestartet werden nach Docker-Installation

**Lösung:**
```powershell
# Terminal neu starten und versuchen
docker --version
```

### Fehler: "docker-compose not recognized"

```powershell
docker compose version
```

Falls neu nach Docker Desktop Update:
```powershell
docker compose version
```

### Fehler: "Access Denied" beim Schreiben in build\docker-output\

**Ursache:** Datei gesperrt von vorherigem Build

**Lösung:**
```powershell
# Build-Verzeichnis löschen
Remove-Item -Recurse -Force build\docker-output\
mkdir build\docker-output\

# Oder nur .ppma löschen
Remove-Item build\docker-output\*.ppma -Force

# Dann neu bauen
.\docker-build.ps1 -Clean
```

### Fehler: "Disk out of space"

Docker-Images brauchen ~5-10 GB:

```powershell
# Prüfe verfügbaren Speicher
(Get-PSDrive C).Free / 1GB

# Falls zu wenig: Docker-System bereinigen
docker system prune -a
```

### Fehler: WSL 2 nicht aktiviert

```powershell
wsl --list
```

Falls nicht vorhanden, öffnen Sie PowerShell **als Admin**:
```powershell
wsl --install -d Ubuntu
Restart-Computer
```

Danach Docker Desktop neu starten.

### Build hängt bei "Building libopencm3..."

Das ist **normal** — kann 5-10 Minuten dauern. Geduld!

Falls nach 30 Minuten noch hängend:
```powershell
# Docker-Prozesse überprüfen
docker ps

# Falls problematisch, Container stoppen
docker stop <CONTAINER_ID>

# Neuer Versuch
.\docker-build.ps1 -Rebuild -Clean
```

### Fehler: "mayhem-firmware clone failed"

Meist Netzwerk-Problem:

```powershell
# Internet-Verbindung prüfen
ping github.com

# Mit Update erzwingen
.\docker-build.ps1 -Clean -UpdateFirmware
```

## 💾 SD-Karte vorbereiten (Windows)

### Schritt 1: SD-Karte mounten

1. SD-Karten-Leser anschließen
2. SD-Karte in Reader einlegen
3. Windows erkennt Laufwerk (normalerweise E:\, F:\, etc.)

```powershell
# Verfügbare Laufwerke auflisten
Get-Volume | Where-Object { $_.DriveType -eq 'Removable' }
```

### Schritt 2: Dateien kopieren

Ersetzen Sie `E:\` mit Ihrem tatsächlichen SD-Karten-Laufwerk:

```powershell
# Zielverzeichnisse erstellen (falls nicht vorhanden)
New-Item -ItemType Directory -Path "E:\APPS" -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Path "E:\MDK-PREDATOR\config" -ErrorAction SilentlyContinue

# Dateien kopieren
Copy-Item "build\docker-output\mdk_predator.ppma" "E:\APPS\"
Copy-Item "mdk_predator.conf" "E:\MDK-PREDATOR\config\"

# Überprüfen
ls "E:\APPS\mdk_predator.ppma"
ls "E:\MDK-PREDATOR\config\mdk_predator.conf"
```

### Schritt 3: SD-Karte auswerfen

```powershell
# SD-Karte sicher auswerfen (Windows Explorer)
# Rechtsklick auf Laufwerk → Auswerfen

# Oder per PowerShell:
$drive = Get-Volume -DriveLetter E
$drive | Get-Partition | Get-Disk | Get-PhysicalDisk
```

## 🎮 PortaPack Test

1. SD-Karte in PortaPack H4M einlegen
2. PortaPack anschalten (mit Mayhem v2.3.1)
3. Im Menü: **Apps → Utilities → MDK-Predator**
4. App sollte starten

## 📊 Performance-Tipps für Windows

### Schnellerer Build mit Ninja

```powershell
.\docker-build.ps1 -Ninja -Clean
```

**Vorteil:** ~20% schneller als Make

### Docker-Ressourcen erhöhen

Settings öffnen:
1. Docker Desktop Systemtablett-Icon → Settings
2. Resources
3. CPUs: auf max verfügbar setzen
4. Memory: mindestens 4 GB, besser 8 GB
5. Disk image size: mindestens 50 GB

### WSL 2 Performance optimieren

`%USERPROFILE%\.wslconfig` erstellen:

```ini
[interop]
enabled=true
appendWindowsPath=true

[wsl2]
memory=8GB
processors=4
swap=2GB
localhostForwarding=true

[experimental]
sparseVhd=true
```

Dann WSL neu starten:
```powershell
wsl --shutdown
```

## 📝 Skript-Debugging

Falls `.\docker-build.ps1` nicht läuft:

```powershell
# ExecutionPolicy prüfen
Get-ExecutionPolicy

# Falls Restricted, temporär ändern:
Set-ExecutionPolicy -ExecutionPolicy Bypass -Scope Process

# Danach Skript ausführen
.\docker-build.ps1 -Clean
```

## ✨ Vollständige Build-Sequenz für Windows

```powershell
# 1. Repository klonen
cd $env:USERPROFILE\Desktop
git clone https://github.com/limbo111111/mdk-predator.git
cd mdk-predator

# 2. Mayhem v2.3.1 spezifisch bauen
$env:MAYHEM_VERSION = "2.3.1"
.\docker-build.ps1 -Clean

# 3. Build-Ergebnis überprüfen
ls build\docker-output\

# 4. SD-Karte vorbereiten
New-Item -ItemType Directory -Path "E:\APPS" -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Path "E:\MDK-PREDATOR\config" -ErrorAction SilentlyContinue

# 5. Dateien kopieren
Copy-Item "build\docker-output\mdk_predator.ppma" "E:\APPS\"
Copy-Item "mdk_predator.conf" "E:\MDK-PREDATOR\config\"

# 6. SD-Karte auswerfen und in PortaPack einlegen
# 7. App starten: Apps → Utilities → MDK-Predator
```

---

**Hinweis:** Bei Fragen oder Problemen, GitHub Issues mit `[WINDOWS-BUILD]` Tag erstellen.
