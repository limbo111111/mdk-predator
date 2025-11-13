# Windows Docker Build - Schnell-Checkliste

## 🚀 Quick Start (5 Minuten)

```powershell
# 1. PowerShell öffnen (WIN + R → "powershell")
# 2. Zum Desktop navigieren
cd Desktop

# 3. Repository klonen
git clone https://github.com/limbo111111/mdk-predator.git
cd mdk-predator

# 4. Build starten
.\docker-build.ps1 -Clean
```

**Warten Sie 15-25 Minuten...**

```powershell
# 5. Überprüfen
ls build\docker-output\
```

Sollte zeigen:
- ✅ `mdk_predator.ppma` (2-3 MB)
- ✅ `mdk_predator.conf` (einige KB)

---

## ✓ Voraussetzungs-Checkliste

- [ ] **Docker Desktop** installiert
  ```powershell
  docker --version
  # Sollte ausgeben: Docker version 26.x.x
  ```

- [ ] **Docker Desktop** läuft
  - Systemtablett (rechts unten) → Docker Icon sollte aktiv sein
  - Oder: "Docker Desktop" aus Startmenü starten

- [ ] **WSL 2** aktiviert (für Docker)
  ```powershell
  wsl --list
  # Sollte eine Linux-Distribution zeigen
  ```

- [ ] **Git** installiert
  ```powershell
  git --version
  # Sollte ausgeben: git version x.x.x
  ```

- [ ] **Disk-Speicher** verfügbar: mindestens 20 GB frei

- [ ] **Internet-Verbindung** aktiv

---

## 🔨 Build-Checkliste

### Phase 1: Vorbereitung

- [ ] PowerShell öffnen
  ```powershell
  # (WIN + R → "powershell" ODER Windows-Taste → "powershell")
  ```

- [ ] Zum Projekt navigieren
  ```powershell
  cd Desktop\mdk-predator
  ```

- [ ] Skript-Berechtigung prüfen
  ```powershell
  Get-ExecutionPolicy
  # Falls "Restricted", dann:
  Set-ExecutionPolicy -ExecutionPolicy Bypass -Scope Process
  ```

### Phase 2: Build ausführen

- [ ] Standard-Build starten
  ```powershell
  .\docker-build.ps1 -Clean
  ```

- [ ] **ODER** mit Mayhem v2.3.1 explizit:
  ```powershell
  $env:MAYHEM_VERSION = "2.3.1"
  .\docker-build.ps1 -Clean
  ```

### Phase 3: Während des Builds

Achten Sie auf diese Meldungen (in dieser Reihenfolge):

- [ ] `[INFO] MDK-Predator Docker Build`
- [ ] `[INFO] Starting clean build...`
- [ ] `==> Cloning mayhem-firmware...` (oder "already present")
- [ ] `==> Requested mayhem version: 2.3.1`
- [ ] `==> Building libopencm3...`
  - ⏱️ **Dies dauert 5-10 Minuten** (NORMAL!)
- [ ] `==> Building mdk-predator application...`
  - ⏱️ **Dies dauert 3-5 Minuten** (NORMAL!)
- [ ] `==> Extracting .ppma file...`
- [ ] `[INFO] Build completed!`

### Phase 4: Output-Validierung

```powershell
# Nach Build-Abschluss ausführen:
ls build\docker-output\
```

Erforderliche Dateien:
- [ ] `mdk_predator.ppma` vorhanden
- [ ] Dateigröße: 1-3 MB
- [ ] `mdk_predator.conf` vorhanden
- [ ] Dateigröße: ~4 KB

---

## 💾 SD-Karte Deployment

### Schritt 1: SD-Karten-Laufwerk identifizieren

```powershell
# SD-Karten-Leser anschließen
# SD-Karte einlegen

# Verfügbare Laufwerke auflisten:
Get-Volume | Where-Object { $_.DriveType -eq 'Removable' }
```

Notieren Sie das Laufwerk (z.B. `E:\` oder `F:\`):
- [ ] SD-Karten-Laufwerk: **__:**

### Schritt 2: Verzeichnisse vorbereiten

```powershell
# ERSETZE "E:\" mit Ihrem tatsächlichen Laufwerk!

New-Item -ItemType Directory -Path "E:\APPS" -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Path "E:\MDK-PREDATOR\config" -ErrorAction SilentlyContinue
```

- [ ] `APPS` Verzeichnis erstellt
- [ ] `MDK-PREDATOR\config` Verzeichnis erstellt

### Schritt 3: Dateien kopieren

```powershell
# ERSETZE "E:\" mit Ihrem tatsächlichen Laufwerk!

Copy-Item "build\docker-output\mdk_predator.ppma" "E:\APPS\"
Copy-Item "mdk_predator.conf" "E:\MDK-PREDATOR\config\"
```

- [ ] `mdk_predator.ppma` nach E:\APPS\ kopiert
- [ ] `mdk_predator.conf` nach E:\MDK-PREDATOR\config\ kopiert

### Schritt 4: Überprüfung

```powershell
# Überprüfe ob Dateien vorhanden sind:
ls "E:\APPS\mdk_predator.ppma"
ls "E:\MDK-PREDATOR\config\mdk_predator.conf"
```

- [ ] Beide Dateien sind sichtbar

### Schritt 5: SD-Karte auswerfen

```powershell
# Option 1: Windows Explorer
# → Rechtsklick auf SD-Karten-Laufwerk
# → "Auswerfen"

# Option 2: PowerShell (Admin erforderlich)
# Nicht erforderlich für normalen Gebrauch
```

- [ ] SD-Karte sicher ausgeworfen

---

## 🎮 PortaPack Test

### Vor dem Start

- [ ] PortaPack H4M angeschlossen
- [ ] Mayhem v2.3.1 läuft (überprüfen Sie Firmware-Info)
- [ ] SD-Karte in PortaPack eingelegt

### App starten

- [ ] Im PortaPack-Menü: **Apps → Utilities**
- [ ] **MDK-Predator** sollte in der Liste sein
- [ ] Klicken Sie auf MDK-Predator

### Grundtests

**WiFi Module:**
- [ ] "WiFi" Tab vorhanden
- [ ] "Scan Networks" Button funktioniert
- [ ] Netzwerke werden angezeigt

**Bluetooth Module:**
- [ ] "Bluetooth" Tab vorhanden
- [ ] "Scan Devices" Button funktioniert
- [ ] BT-Geräte werden angezeigt

**SubGHz Module:**
- [ ] "SubGHz" Tab vorhanden
- [ ] "Scan Spectrum" Button funktioniert
- [ ] Frequenz-Daten werden angezeigt

### Keine Fehler

- [ ] Keine Crash-Meldungen
- [ ] App lädt ohne zu hängen
- [ ] App kann beendet werden

---

## 🔴 Häufige Fehler auf Windows

| Fehler | Ursache | Lösung |
|--------|--------|--------|
| "Docker not found" | Docker nicht installiert/läuft | Docker Desktop installieren + starten |
| "Access Denied" | Datei gesperrt | `Remove-Item -Recurse -Force build\docker-output\` + Neustart |
| Build hängt bei libopencm3 | Kompilierung dauert lange | Geduld! 5-10 Min normal |
| "WSL 2 not found" | WSL 2 nicht aktiviert | `wsl --install` + Neustart |
| SD-Karte nicht gefunden | Nicht gemountet | Leser anschließen, Karte einlegen |

---

## ⚡ Performance optimieren

### Option 1: Ninja verwenden (20% schneller)

```powershell
.\docker-build.ps1 -Clean -Ninja
```

### Option 2: Docker-Ressourcen erhöhen

1. Docker Desktop Systemtablett-Icon → Settings
2. Resources Tab
3. CPUs erhöhen: von 2 auf 4+ (je nach System)
4. Memory erhöhen: mindestens 4 GB (besser 8 GB)
5. Anwenden + Docker neu starten

### Option 3: Zwischenspeicher nutzen

Nach erstem erfolgreichem Build:

```powershell
# Ohne -Clean (viel schneller!)
.\docker-build.ps1
```

---

## 📊 Build-Statistiken (typisch für Windows)

| Schritt | Dauer | Status |
|---------|-------|--------|
| Docker Image erstellen | 2-3 Min | (nur beim 1. Mal) |
| Mayhem Firmware klonen | 2-3 Min | (nur beim 1. Mal) |
| libopencm3 bauen | 5-10 Min | ✓ Normal |
| mdk-predator Compile | 3-5 Min | ✓ Normal |
| .ppma Extraction | <1 Min | ✓ Normal |
| **Gesamt (1. Build)** | **~25-35 Min** | ✓ |
| **Gesamt (Wiederholung)** | **~5-10 Min** | ✓ |

---

## ✨ Fertig!

Wenn alle Häkchen gesetzt sind, haben Sie erfolgreich:

✅ mdk-predator gegen Mayhem v2.3.1 gebaut
✅ `.ppma` Datei erzeugt
✅ Auf SD-Karte deployed
✅ Auf PortaPack gestartet

**Glückwunsch!** 🎉

---

**Probleme?** GitHub Issue erstellen mit Tag `[WINDOWS-BUILD]`
