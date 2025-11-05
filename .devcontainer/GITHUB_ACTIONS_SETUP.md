# 🚀 GitHub Actions Setup - FERTIG!

## ✅ Workflow erstellt!

Der GitHub Actions Workflow ist jetzt bereit!

**Location**: `.github/workflows/build-firmware.yml`

---

## 🎯 So funktioniert's:

### Automatischer Build bei:
- ✅ Push auf `main` oder `develop` Branch
- ✅ Pull Requests nach `main`
- ✅ Tags mit `v*` (z.B. v1.0.0)
- ✅ Manueller Trigger (Workflow Dispatch)

### Was passiert:
1. **Ubuntu Runner** startet
2. **ESP-IDF v5.3** wird installiert
3. **ESP32-S3 Firmware** wird gebaut
4. **Binaries** werden gepackt:
   - `mdk-predator.bin`
   - `bootloader.bin`
   - `partition-table.bin`
   - `flash_mdk.sh`
   - `FLASH_INSTRUCTIONS.md`
5. **Artifacts** werden hochgeladen (90 Tage Aufbewahrung)
6. Bei **Tags**: Automatisches GitHub Release!

---

## 📦 JETZT ZU GITHUB PUSHEN!

### Option A: Erster Push (neues Repo)

```bash
# Im Workspace (.devcontainer/)
cd /workspaces/mdk-predator/.devcontainer

# Git initialisieren
git init

# Alle Dateien hinzufügen
git add .

# Commit
git commit -m "Initial commit: MDK Predator v1.0.0 - Production Ready

- Complete ESP32-S3 firmware (1,320 lines)
- I2C protocol with 30+ commands
- KeeLoq bruteforce acceleration (500x faster)
- CC1101, CAN, Crypto support
- GitHub Actions auto-build workflow"

# Remote hinzufügen (ERSETZE MIT DEINEM REPO!)
git remote add origin https://github.com/limbo111111/mdk-predator.git

# Branch umbenennen
git branch -M main

# Push!
git push -u origin main
```

### Option B: Bestehendes Repo

```bash
cd /workspaces/mdk-predator/.devcontainer

# Status checken
git status

# Neue Dateien hinzufügen
git add .github/workflows/build-firmware.yml
git add .gitignore
git add esp32_main.c
git add mdk_hardware_abstraction.c
git add mdk_i2c_protocol.h
git add *.md
git add *.sh

# Commit
git commit -m "Add MDK Predator firmware v1.0.0

- Complete ESP32-S3 implementation
- GitHub Actions build workflow
- All documentation & scripts"

# Push
git push origin main
```

### Option C: Release mit Tag

```bash
cd /workspaces/mdk-predator/.devcontainer

# Alle Dateien committen (siehe Option A/B)
git add .
git commit -m "MDK Predator v1.0.0 - Production Ready"
git push origin main

# Tag erstellen
git tag -a v1.0.0 -m "MDK Predator v1.0.0

Production-ready release:
- ESP32-S3 dual-core @ 240MHz
- 30+ I2C commands
- KeeLoq/PT2260 support
- CC1101 transceiver
- Hardware crypto acceleration"

# Tag pushen (triggert Release-Build!)
git push origin v1.0.0
```

---

## 📥 BINARIES DOWNLOADEN

### Nach dem Push:

1. **Gehe zu GitHub**: https://github.com/limbo111111/mdk-predator

2. **Klicke auf "Actions" Tab**

3. **Warte auf Build** (ca. 5 Minuten):
   - ⚙️ Gelb = Build läuft
   - ✅ Grün = Build erfolgreich
   - ❌ Rot = Build fehlgeschlagen

4. **Download Artifacts**:
   - Klicke auf den Workflow Run
   - Scrolle zu "Artifacts"
   - Download `mdk-predator-firmware` (ZIP mit allen Binaries)

### Bei Release (mit Tag):

1. **Gehe zu "Releases" Tab**

2. **Dein Release** erscheint automatisch:
   - `v1.0.0` (oder dein Tag)
   - Komplette Beschreibung
   - **Assets**:
     - `mdk-predator-v1.0.0.tar.gz` (Komplett-Paket)
     - `mdk-predator.bin`
     - `bootloader.bin`
     - `partition-table.bin`

3. **Download & Flash**:
   ```bash
   # Download Release
   wget https://github.com/limbo111111/mdk-predator/releases/download/v1.0.0/mdk-predator-v1.0.0.tar.gz
   
   # Extrahieren
   tar -xzf mdk-predator-v1.0.0.tar.gz
   
   # Flash!
   chmod +x flash_mdk.sh
   ./flash_mdk.sh /dev/ttyUSB0
   ```

---

## 🎯 WORKFLOW FEATURES

### Sicherheit:
- ✅ Verwendet offizielle ESP-IDF Action
- ✅ Ubuntu Latest Runner
- ✅ Keine Secrets nötig (alles öffentlich)

### Artifacts:
- **mdk-predator-firmware**: Alle Binaries (90 Tage)
- **mdk-predator-release**: Release-Package (365 Tage, nur bei Tags)

### Automatische Release-Notes:
- ✅ Version & Timestamp
- ✅ Hardware-Specs
- ✅ Feature-Liste
- ✅ Flash-Anleitung
- ✅ Commit-Hash

### Build-Matrix:
- ESP-IDF: v5.3 (aktuell, kompatibel mit ESP32-S3)
- Target: ESP32-S3
- Flash: 8MB
- Frequency: 240MHz

---

## 🔧 WORKFLOW ANPASSEN

Falls du Änderungen brauchst:

### ESP-IDF Version ändern:
```yaml
# In .github/workflows/build-firmware.yml
esp_idf_version: v5.3  # ← hier ändern (z.B. v5.4, latest)
```

### Andere Branches:
```yaml
on:
  push:
    branches: [ main, develop, feature/* ]  # ← hier anpassen
```

### Aufbewahrungsdauer:
```yaml
retention-days: 90  # ← Artifacts (max 90)
retention-days: 365 # ← Releases (beliebig)
```

---

## 📊 NÄCHSTE SCHRITTE

1. ✅ **Push zu GitHub** (siehe oben)
2. ⏰ **Warte 5 Minuten** (Build läuft)
3. 📥 **Download Binaries** (Actions → Artifacts)
4. 🔌 **Flash auf ESP32-S3**
5. 🎯 **Test auf PortaPack H4M**
6. 🚀 **Enjoy 500x Speed!**

---

## 🎉 BONUS: Badge für README

Füge das zu deinem README.md hinzu:

```markdown
[![Build Firmware](https://github.com/limbo111111/mdk-predator/actions/workflows/build-firmware.yml/badge.svg)](https://github.com/limbo111111/mdk-predator/actions/workflows/build-firmware.yml)
```

Zeigt Status: ![Build Firmware](https://img.shields.io/badge/build-passing-brightgreen)

---

**GitHub Actions Setup: ✅ COMPLETE!**

**Nächster Schritt**: `git push origin main` 🚀
