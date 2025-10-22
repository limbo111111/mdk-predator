/*
 * MDK-Predator PortaPack Mayhem Application Header
 */

#ifndef __MDK_PREDATOR_APP_H__
#define __MDK_PREDATOR_APP_H__

#include "ui.hpp"
#include "ui_widget.hpp"
#include "ui_navigation.hpp"
#include "ui_receiver.hpp"
#include "ui_font_fixed_8x16.hpp"
#include "message.hpp"
#include <string>

namespace ui {

// Forward declarations
class AutomotiveView;
class WiFiView;
class BluetoothView;
class SubGHzView;
class CryptoView;

// Main MDK-Predator application view
class MDKPredatorView : public View {
public:
    MDKPredatorView(NavigationView& nav);
    ~MDKPredatorView();
    
    void focus() override;
    std::string title() const override { return "MDK-Predator"; };

private:
    NavigationView& nav_;
    
    Labels labels {
        { { 2 * 8, 1 * 16 }, "MDK-Predator Security Suite", Color::light_grey() },
        { { 2 * 8, 2 * 16 }, "Mayhem-MDK Module", Color::light_grey() }
    };
    
    Button button_automotive {
        { 2 * 8, 4 * 16, 26 * 8, 2 * 16 },
        "Automotive Security"
    };
    
    Button button_wifi {
        { 2 * 8, 7 * 16, 26 * 8, 2 * 16 },
        "WiFi Analysis"
    };
    
    Button button_bluetooth {
        { 2 * 8, 10 * 16, 26 * 8, 2 * 16 },
        "Bluetooth Analysis"
    };
    
    Button button_subghz {
        { 2 * 8, 13 * 16, 26 * 8, 2 * 16 },
        "SubGHz RF"
    };
    
    Button button_crypto {
        { 2 * 8, 16 * 16, 26 * 8, 2 * 16 },
        "Crypto Analysis"
    };
    
    Button button_exit {
        { 2 * 8, 19 * 16, 26 * 8, 2 * 16 },
        "Exit"
    };
    
    Text text_status {
        { 2 * 8, 22 * 16, 26 * 8, 16 },
        "Status: Ready"
    };
    
    Console console {
        { 0, 24 * 8, 240, 8 * 8 }
    };
    
    void log_message(const std::string& message);
};

// Automotive module view
class AutomotiveView : public View {
public:
    AutomotiveView(NavigationView& nav);
    void focus() override;
    std::string title() const override { return "Automotive"; };

private:
    NavigationView& nav_;
    
    Labels labels {
        { { 2 * 8, 1 * 16 }, "Automotive Security", Color::light_grey() }
    };
    
    Button button_key_fob {
        { 2 * 8, 3 * 16, 26 * 8, 2 * 16 },
        "Key Fob Analyzer"
    };
    
    Button button_rolling_code {
        { 2 * 8, 6 * 16, 26 * 8, 2 * 16 },
        "Rolling Code Tester"
    };
    
    Button button_back {
        { 2 * 8, 19 * 16, 26 * 8, 2 * 16 },
        "Back"
    };
    
    Text text_status {
        { 2 * 8, 22 * 16, 26 * 8, 16 },
        ""
    };
    
    Console console {
        { 0, 24 * 8, 240, 8 * 8 }
    };
    
    void start_key_fob_analyzer();
    void start_rolling_code_tester();
};

// WiFi module view
class WiFiView : public View {
public:
    WiFiView(NavigationView& nav);
    void focus() override;
    std::string title() const override { return "WiFi"; };

private:
    NavigationView& nav_;
    
    Labels labels {
        { { 2 * 8, 1 * 16 }, "WiFi Analysis", Color::light_grey() }
    };
    
    Button button_scan {
        { 2 * 8, 3 * 16, 26 * 8, 2 * 16 },
        "Network Scanner"
    };
    
    Button button_analyze {
        { 2 * 8, 6 * 16, 26 * 8, 2 * 16 },
        "Security Analysis"
    };
    
    Button button_back {
        { 2 * 8, 19 * 16, 26 * 8, 2 * 16 },
        "Back"
    };
    
    Text text_status {
        { 2 * 8, 22 * 16, 26 * 8, 16 },
        ""
    };
    
    Console console {
        { 0, 24 * 8, 240, 8 * 8 }
    };
    
    void start_network_scan();
};

// Bluetooth module view
class BluetoothView : public View {
public:
    BluetoothView(NavigationView& nav);
    void focus() override;
    std::string title() const override { return "Bluetooth"; };

private:
    NavigationView& nav_;
    
    Labels labels {
        { { 2 * 8, 1 * 16 }, "Bluetooth Analysis", Color::light_grey() }
    };
    
    Button button_scan {
        { 2 * 8, 3 * 16, 26 * 8, 2 * 16 },
        "Device Scanner"
    };
    
    Button button_enumerate {
        { 2 * 8, 6 * 16, 26 * 8, 2 * 16 },
        "Service Enumeration"
    };
    
    Button button_back {
        { 2 * 8, 19 * 16, 26 * 8, 2 * 16 },
        "Back"
    };
    
    Text text_status {
        { 2 * 8, 22 * 16, 26 * 8, 16 },
        ""
    };
    
    Console console {
        { 0, 24 * 8, 240, 8 * 8 }
    };
    
    void start_device_scan();
};

// SubGHz module view
class SubGHzView : public View {
public:
    SubGHzView(NavigationView& nav);
    void focus() override;
    std::string title() const override { return "SubGHz RF"; };

private:
    NavigationView& nav_;
    
    Labels labels {
        { { 2 * 8, 1 * 16 }, "SubGHz RF Analysis", Color::light_grey() }
    };
    
    Button button_spectrum {
        { 2 * 8, 3 * 16, 26 * 8, 2 * 16 },
        "Spectrum Scanner"
    };
    
    Button button_capture {
        { 2 * 8, 6 * 16, 26 * 8, 2 * 16 },
        "Signal Capture"
    };
    
    Button button_decode {
        { 2 * 8, 9 * 16, 26 * 8, 2 * 16 },
        "Protocol Decoder"
    };
    
    Button button_back {
        { 2 * 8, 19 * 16, 26 * 8, 2 * 16 },
        "Back"
    };
    
    Text text_status {
        { 2 * 8, 22 * 16, 26 * 8, 16 },
        ""
    };
    
    Console console {
        { 0, 24 * 8, 240, 8 * 8 }
    };
    
    void start_spectrum_scan();
    void start_signal_capture();
};

// Crypto module view
class CryptoView : public View {
public:
    CryptoView(NavigationView& nav);
    void focus() override;
    std::string title() const override { return "Crypto"; };

private:
    NavigationView& nav_;
    
    Labels labels {
        { { 2 * 8, 1 * 16 }, "Crypto Analysis", Color::light_grey() }
    };
    
    Button button_identify {
        { 2 * 8, 3 * 16, 26 * 8, 2 * 16 },
        "Algorithm Identifier"
    };
    
    Button button_entropy {
        { 2 * 8, 6 * 16, 26 * 8, 2 * 16 },
        "Entropy Analysis"
    };
    
    Button button_back {
        { 2 * 8, 19 * 16, 26 * 8, 2 * 16 },
        "Back"
    };
    
    Text text_status {
        { 2 * 8, 22 * 16, 26 * 8, 16 },
        ""
    };
    
    Console console {
        { 0, 24 * 8, 240, 8 * 8 }
    };
    
    void start_entropy_analysis();
};

} /* namespace ui */

#endif /*__MDK_PREDATOR_APP_H__*/
