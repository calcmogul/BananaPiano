// Copyright (c) Tyler Veness

#pragma once

#include <string>
#include <string_view>
#include <vector>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

/**
 * Provides an interface for communicating with an Arduino via the serial port
 */
class SerialPort {
public:
    // Initialize SerialPort communication with the given COM port
    explicit SerialPort(std::string_view port_name = "")
        : m_port_name{port_name} {}

    /* Close the connection
     * NOTE: for some reason you can't connect again before exiting the program
     * and running it again.
     */
    ~SerialPort() { disconnect(); }

    /* Initialize SerialPort communication with the given COM port. If NULL is
     * passed as an argument, the previously assigned name will be used. This
     * should be done in the case of a reconnection attempt.
     */
    void connect(std::string port_name = "");

    /* Close the connection
     * NOTE: for some reason you can't connect again before exiting the program
     * and running it again.
     */
    void disconnect();

    /* Read data in a buffer, if num_chars is greater than the maximum number of
     * bytes available, it will return only the bytes available. The function
     * return -1 when nothing could be read, the number of bytes actually read.
     */
    int read(char* buffer, unsigned int num_chars);

    /* Writes data from a buffer through the SerialPort connection. Returns
     * true on success; returns false on failure.
     */
    bool write(char* buffer, unsigned int num_chars);

    // Check if we are actually connected
    bool is_connected() const { return m_connected; }

    static std::vector<std::string> get_serial_ports();

private:
#ifdef _WIN32
    // SerialPort comm handler
    HANDLE serial_handle;

    // Get various information about the connection
    COMSTAT m_status;

    // Keep track of last error
    DWORD m_errors;
#else
    int m_fd;
#endif

    // Contains OS-specific name for serial port
    std::string m_port_name;

    // Connection status
    bool m_connected = false;
};
