// Copyright (c) Tyler Veness

#include "serial_port.hpp"

#include <algorithm>
#include <cstring>
#include <print>
#include <string>
#include <vector>

#ifndef _WIN32
#include <dirent.h>  // Method for listing serial ports
#include <fcntl.h>   // File control definitions
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>  // POSIX terminal control definitions
#include <unistd.h>   // UNIX standard function definitions

#include <cerrno>  // Error number definitions
#endif

void SerialPort::connect(std::string port_name) {
    // Disconnect before reconnecting or connecting to a different serial port
    if (m_connected) {
        disconnect();
    }

    if (port_name != "") {
        // Update name if necessary
        m_port_name = port_name;
    } else if (m_port_name == "") {
        // If no stored name, don't attempt a connection
        return;
    }

// Try to connect to the given port
#ifdef _WIN32
    serial_handle =
        CreateFile(port_name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr,
                   OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
#else
    m_fd = open(port_name.c_str(), O_RDONLY | O_NOCTTY | O_NDELAY);
#endif

// Check if connection was successful
#ifdef _WIN32
    if (serial_handle == INVALID_HANDLE_VALUE) {
        if (GetLastError() == ERROR_FILE_NOT_FOUND) {
            std::print("{}: Unable to open {}: No such file or directory",
                       __FILE__, port_name);
        } else {
            std::println("{}: Unable to open {}", __FILE__, port_name);
        }

        return;
    }
#else
    if (m_fd == -1) {
        std::println("{}: Unable to open {}: No such file or directory",
                     __FILE__, m_port_name);

        return;
    } else {
        fcntl(m_fd, F_SETFL, FNDELAY);
    }
#endif

// Set baud rate
#ifdef _WIN32
    DCB serial_port_params = {0};

    // Try to get the current comm parameters
    if (!GetCommState(serial_handle, &serial_port_params)) {
        // If impossible, show an error
        std::println("{}: Unable to retrieve current serial parameters for {}",
                     __FILE__, port_name);

        return;
    } else {
        // Define serial connection parameters for the Arduino board
        serial_port_params.BaudRate = CBR_115200;
        serial_port_params.ByteSize = 8;
        serial_port_params.StopBits = ONESTOPBIT;
        serial_port_params.Parity = NOPARITY;

        // Set the parameters and check for their proper application
        if (!SetCommState(serial_handle, &serial_port_params)) {
            std::println("{}: Unable to set serial port parameters for {}",
                         __FILE__, port_name);

            return;
        }
    }
#else
    struct termios options;

    // Get the current options for the port
    tcgetattr(m_fd, &options);

    // Set the baud rates
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);

    // Enable the receiver and set local mode
    options.c_cflag |= (CLOCAL | CREAD);

    // Set the new options for the port
    tcsetattr(m_fd, TCSANOW, &options);
#endif

    m_connected = true;
}

void SerialPort::disconnect() {
    // Disconnect if necessary
    if (m_connected) {
#ifdef _WIN32
        CloseHandle(serial_handle);
#else
        close(m_fd);
#endif
        m_connected = false;
    }
}

int SerialPort::read(char* buffer, unsigned int num_chars) {
#ifdef _WIN32
    // Number of bytes we'll have read
    DWORD bytes_read;
#else
    // Number of bytes we'll have read
    int bytes_read;
#endif

#ifdef _WIN32
    // Use the ClearCommError function to get status info on the SerialPort port
    ClearCommError(serial_handle, &m_errors, &m_status);

    // Check if there is something to read
    if (m_status.cbInQue > 0) {
        /* If there is we check if there is enough data to read the required
         * number of characters, if not we'll read only the available
         * characters to prevent locking of the application.
         */
        // Number of bytes that will actually be read
        unsigned int toRead;
        if (m_status.cbInQue > num_chars) {
            toRead = num_chars;
        } else {
            toRead = m_status.cbInQue;
        }

        /* Try to read the require number of chars, and return the number of
         * read bytes on success
         */
        if (ReadFile(serial_handle, buffer, toRead, &bytes_read, nullptr)) {
            return bytes_read;
        } else if (GetLastError() != ERROR_IO_PENDING) {
            return -1;
        } else {
            buffer[0] = '\0';
            return 0;
        }
    } else {
        // There are no bytes to read
        return 0;
    }
#else
    bytes_read = ::read(m_fd, buffer, num_chars);

    // Check for disconnection
    if (bytes_read == 0) {
        struct stat file_stats;
        if (stat(m_port_name.c_str(), &file_stats) == -1) {
            return -1;
        } else {
            return 0;
        }
    } else if (bytes_read == -1 && errno != EAGAIN) {
        return -1;
    } else {
        return std::max(bytes_read, 0);
    }
#endif
}

bool SerialPort::write(char* buffer, unsigned int num_chars) {
#ifdef _WIN32
    DWORD bytes_sent = 0;
#else
    int bytes_sent = 0;
#endif

#ifdef _WIN32
    // Try to write the buffer on the serial port
    if (!WriteFile(serial_handle, static_cast<void*>(buffer), num_chars,
                   &bytes_sent, 0)) {
        // Write failed, so retrieve comm error
        ClearCommError(serial_handle, &m_errors, &m_status);

        return false;
    } else {
        return true;
    }
#else
    unsigned int pos = 0;

    while (pos < num_chars) {
        bytes_sent = ::write(m_fd, &buffer[pos], num_chars);

        if (bytes_sent > 0) {
            pos += bytes_sent;
        } else if (bytes_sent == -1 && errno != EAGAIN) {
            return false;
        }
    }

    return true;
#endif
}

std::vector<std::string> SerialPort::get_serial_ports() {
    std::vector<std::string> ports;

#ifdef _WIN32
    HKEY reg_adapters;
    LONG res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, AdapterKey, 0, KEY_READ,
                            &reg_adapters);

    for (DWORD index = 0; res == ERROR_SUCCESS; index++) {
        char sub_key_name[255];
        DWORD name = 255;
        res = RegEnumKeyEx(reg_adapters, index, sub_key_name, &name, nullptr,
                           nullptr, nullptr, nullptr);
        if (res != ERROR_SUCCESS) {
            break;
        }
        // do something with name
    }
#else
    DIR* d;
    struct dirent* dir;
    d = opendir("/dev");
    if (d) {
        while ((dir = readdir(d)) != nullptr) {
            if (dir->d_type == DT_CHR) {
                // for (auto& name : {"ttyUSB"}) {
                for (auto& name : {"ttyACM", "ttyUSB"}) {
                    if (std::strncmp(dir->d_name, name, 6) == 0) {
                        std::string tmp = "/dev/";
                        tmp += dir->d_name;
                        ports.emplace_back(tmp);
                    }
                }
            }
        }

        closedir(d);
    }
#endif

    return ports;
}
