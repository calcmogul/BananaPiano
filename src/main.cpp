// Copyright (c) Tyler Veness

#include <array>
#include <format>
#include <print>
#include <string>
#include <vector>

#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include "serial_port.hpp"

static constexpr int NUM_NOTES = 3;

int main() {
    SerialPort serial_port;

    sf::RenderWindow main_window{sf::VideoMode{{72, 72}}, "Banana Piano",
                                 sf::Style::Resize | sf::Style::Close,
                                 sf::State::Windowed};
    main_window.setFramerateLimit(25);

    // Used to store data read from serial port or socket
    std::string serial_port_data;
    char cur_char = '\0';
    char num_read = 0;

    std::vector<sf::Sound> sounds;
    sounds.reserve(NUM_NOTES);

    std::vector<sf::SoundBuffer> buffers;
    buffers.reserve(NUM_NOTES);

    constexpr std::array NOTES{"g#", "a",  "bb", "b", "c",  "c#",
                               "d",  "eb", "e",  "f", "f#", "g"};
    for (const auto& note : NOTES) {
        buffers.emplace_back(std::format("data/piano-{}.wav", note));
        sounds.emplace_back(buffers.back());
    }

    std::vector<char> last_input{NUM_NOTES, '1'};
    bool have_valid_data = false;

    while (main_window.isOpen()) {
        while (auto event = main_window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                main_window.close();
            }
        }

        // Attempt a connection
        if (!serial_port.is_connected()) {
            auto ports = SerialPort::get_serial_ports();
            if (ports.size() > 0) {
                serial_port.connect(ports[0]);
            }
        }

        // Read line of serial_port data
        if (serial_port.is_connected()) {
            while ((num_read = serial_port.read(&cur_char, 1)) > 0 &&
                   cur_char != '\n' && cur_char != '\0') {
                serial_port_data += cur_char;
            }

            if (num_read == -1) {
                // EOF has been reached (socket disconnected)
                serial_port.disconnect();
            } else if (cur_char == '\n' && serial_port_data.length() != 0) {
                // If cur_char == '\n', there is a new line of complete data
                std::println("{}", serial_port_data);

                if (serial_port_data.length() == NUM_NOTES) {
                    have_valid_data = true;

                    for (size_t i = 0; i < NUM_NOTES; ++i) {
                        if (serial_port_data[i] == '0' &&
                            last_input[i] == '1') {
                            sounds[i].play();
                        }

                        last_input[i] = serial_port_data[i];
                    }
                } else {
                    have_valid_data = false;
                }

                // Reset serial data storage in preparation for new line of data
                serial_port_data.clear();
                cur_char = '\0';
                num_read = 0;
            }
        }

        main_window.clear(sf::Color::White);

        // Render connection indicator
        sf::CircleShape circle{18.f};
        circle.setOrigin(circle.getGeometricCenter());
        circle.setPosition({36.f, 36.f});
        if (serial_port.is_connected()) {
            if (have_valid_data) {
                // Connected and valid data
                circle.setFillColor(sf::Color{0, 200, 0});
            } else {
                // Connected but no valid data
                circle.setFillColor(sf::Color{255, 220, 0});
            }
        } else {
            // Disconnected
            circle.setFillColor(sf::Color{200, 0, 0});
        }
        main_window.draw(circle);

        main_window.display();
    }
}
