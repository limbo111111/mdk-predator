/*
 * MDK-Predator PortaPack Mayhem Application
 * 
 * This file provides the PortaPack Mayhem application integration
 * for the MDK-Predator security research suite.
 */

#include "mdk_predator_app.hpp"
#include "portapack.hpp"
#include "baseband_api.hpp"
#include "string_format.hpp"
#include "ui_navigation.hpp"

extern "C" {
    #include "mdk_predator.h"
    #include "automotive/key_fob_analyzer.h"
    #include "wireless/wifi_analyzer.h"
    #include "wireless/bluetooth_analyzer.h"
    #include "wireless/subghz_analyzer.h"
    #include "crypto/crypto_analyzer.h"
}

namespace ui {

MDKPredatorView::MDKPredatorView(NavigationView& nav)
    : nav_(nav) {
    add_children({
        &labels,
        &button_automotive,
        &button_wifi,
        &button_bluetooth,
        &button_subghz,
        &button_crypto,
        &button_exit,
        &text_status,
        &console
    });

    // Initialize MDK-Predator core
    mdk_config_t config;
    config.default_module = MDK_MODULE_AUTOMOTIVE;
    config.hardware.hackrf_sample_rate = 2000000;
    config.hardware.hackrf_bandwidth = 1750000;
    config.hardware.hackrf_lna_gain = 8;
    config.hardware.hackrf_vga_gain = 20;
    config.hardware.hackrf_txvga_gain = 0;
    config.security.allow_transmit = false;
    config.security.require_confirmation = true;
    config.security.log_all_activity = true;

    if (mdk_predator_init(&config)) {
        text_status.set("Status: Initialized");
        log_message("MDK-Predator initialized successfully");
    } else {
        text_status.set("Status: Init failed");
        log_message("ERROR: Failed to initialize MDK-Predator");
    }

    // Button handlers
    button_automotive.on_select = [this](Button&) {
        this->nav_.push<AutomotiveView>();
    };

    button_wifi.on_select = [this](Button&) {
        this->nav_.push<WiFiView>();
    };

    button_bluetooth.on_select = [this](Button&) {
        this->nav_.push<BluetoothView>();
    };

    button_subghz.on_select = [this](Button&) {
        this->nav_.push<SubGHzView>();
    };

    button_crypto.on_select = [this](Button&) {
        this->nav_.push<CryptoView>();
    };

    button_exit.on_select = [this](Button&) {
        this->nav_.pop();
    };
}

MDKPredatorView::~MDKPredatorView() {
    mdk_predator_cleanup();
}

void MDKPredatorView::focus() {
    button_automotive.focus();
}

void MDKPredatorView::log_message(const std::string& message) {
    console.writeln(message);
}

// Automotive Module View
AutomotiveView::AutomotiveView(NavigationView& nav)
    : nav_(nav) {
    add_children({
        &labels,
        &button_key_fob,
        &button_rolling_code,
        &button_back,
        &text_status,
        &console
    });

    text_status.set("Automotive Module");

    button_key_fob.on_select = [this](Button&) {
        this->start_key_fob_analyzer();
    };

    button_rolling_code.on_select = [this](Button&) {
        this->start_rolling_code_tester();
    };

    button_back.on_select = [this](Button&) {
        this->nav_.pop();
    };
}

void AutomotiveView::start_key_fob_analyzer() {
    keyfob_config_t config;
    config.frequency = 433920000; // 433.92 MHz
    config.bandwidth = 200000;    // 200 kHz
    config.sample_rate = 2000000; // 2 MS/s
    config.mode = KEYFOB_MODE_RECEIVE;

    if (keyfob_analyzer_init(&config)) {
        console.writeln("Key Fob Analyzer: Started");
        console.writeln("Frequency: 433.92 MHz");
        console.writeln("Waiting for signals...");
    } else {
        console.writeln("ERROR: Failed to start analyzer");
    }
}

void AutomotiveView::start_rolling_code_tester() {
    rolling_code_config_t config;
    config.algorithm = ROLLING_CODE_KEELOQ;
    config.mode = ROLLING_CODE_MODE_PASSIVE;

    if (rolling_code_tester_init(&config)) {
        console.writeln("Rolling Code Tester: Started");
        console.writeln("Algorithm: KeeLoq");
        console.writeln("Mode: Passive Analysis");
    } else {
        console.writeln("ERROR: Failed to start tester");
    }
}

void AutomotiveView::focus() {
    button_key_fob.focus();
}

// WiFi Module View
WiFiView::WiFiView(NavigationView& nav)
    : nav_(nav) {
    add_children({
        &labels,
        &button_scan,
        &button_analyze,
        &button_back,
        &text_status,
        &console
    });

    text_status.set("WiFi Module");

    button_scan.on_select = [this](Button&) {
        this->start_network_scan();
    };

    button_analyze.on_select = [this](Button&) {
        console.writeln("Select network first");
    };

    button_back.on_select = [this](Button&) {
        this->nav_.pop();
    };
}

void WiFiView::start_network_scan() {
    wifi_config_t config;
    config.mode = WIFI_MODE_SCAN;
    config.channel = 0; // All channels

    if (wifi_analyzer_init(&config)) {
        console.writeln("WiFi Scanner: Started");
        console.writeln("Scanning all channels...");
    } else {
        console.writeln("ERROR: Failed to start scanner");
    }
}

void WiFiView::focus() {
    button_scan.focus();
}

// Bluetooth Module View
BluetoothView::BluetoothView(NavigationView& nav)
    : nav_(nav) {
    add_children({
        &labels,
        &button_scan,
        &button_enumerate,
        &button_back,
        &text_status,
        &console
    });

    text_status.set("Bluetooth Module");

    button_scan.on_select = [this](Button&) {
        this->start_device_scan();
    };

    button_enumerate.on_select = [this](Button&) {
        console.writeln("Select device first");
    };

    button_back.on_select = [this](Button&) {
        this->nav_.pop();
    };
}

void BluetoothView::start_device_scan() {
    bluetooth_config_t config;
    config.scan_type = BT_SCAN_BOTH;
    config.scan_duration = 10; // 10 seconds

    if (bluetooth_analyzer_init(&config)) {
        console.writeln("Bluetooth Scanner: Started");
        console.writeln("Scanning for devices...");
    } else {
        console.writeln("ERROR: Failed to start scanner");
    }
}

void BluetoothView::focus() {
    button_scan.focus();
}

// SubGHz Module View
SubGHzView::SubGHzView(NavigationView& nav)
    : nav_(nav) {
    add_children({
        &labels,
        &button_spectrum,
        &button_capture,
        &button_decode,
        &button_back,
        &text_status,
        &console
    });

    text_status.set("SubGHz RF Module");

    button_spectrum.on_select = [this](Button&) {
        this->start_spectrum_scan();
    };

    button_capture.on_select = [this](Button&) {
        this->start_signal_capture();
    };

    button_decode.on_select = [this](Button&) {
        console.writeln("Capture signal first");
    };

    button_back.on_select = [this](Button&) {
        this->nav_.pop();
    };
}

void SubGHzView::start_spectrum_scan() {
    subghz_config_t config;
    config.frequency_min = 300000000;  // 300 MHz
    config.frequency_max = 928000000;  // 928 MHz
    config.step_size = 100000;         // 100 kHz steps

    if (subghz_analyzer_init(&config)) {
        console.writeln("Spectrum Scanner: Started");
        console.writeln("Range: 300-928 MHz");
    } else {
        console.writeln("ERROR: Failed to start scanner");
    }
}

void SubGHzView::start_signal_capture() {
    console.writeln("Signal Capture: Ready");
    console.writeln("Press capture when ready");
}

void SubGHzView::focus() {
    button_spectrum.focus();
}

// Crypto Module View
CryptoView::CryptoView(NavigationView& nav)
    : nav_(nav) {
    add_children({
        &labels,
        &button_identify,
        &button_entropy,
        &button_back,
        &text_status,
        &console
    });

    text_status.set("Crypto Analysis Module");

    button_identify.on_select = [this](Button&) {
        console.writeln("Load data first");
    };

    button_entropy.on_select = [this](Button&) {
        this->start_entropy_analysis();
    };

    button_back.on_select = [this](Button&) {
        this->nav_.pop();
    };
}

void CryptoView::start_entropy_analysis() {
    crypto_config_t config;
    config.algorithm = CRYPTO_ALGORITHM_AES;
    config.key_length = 128;
    config.mode = CRYPTO_MODE_ANALYZE;

    if (crypto_analyzer_init(&config)) {
        console.writeln("Entropy Analyzer: Ready");
        console.writeln("Load data for analysis");
    } else {
        console.writeln("ERROR: Failed to initialize");
    }
}

void CryptoView::focus() {
    button_identify.focus();
}

} /* namespace ui */
