// Existing content of the file

private:
    bool use_hardware_accel;
    uint32_t parallel_streams;
    mdk_device_info_t mdk_info;

    // New method declarations
    void detect_mdk(); // Declaration for MDK detection
    void report_status(); // Declaration for status reporting
