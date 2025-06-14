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

#include "SerialPort.hpp"

static constexpr int NUM_NOTES = 3;

int main() {
    SerialPort serialPort;

    sf::RenderWindow mainWin{sf::VideoMode{{72, 72}}, "Banana Piano",
                             sf::Style::Resize | sf::Style::Close,
                             sf::State::Windowed};
    mainWin.setFramerateLimit(25);

    // Used to store data read from serial port or socket
    std::string serialPortData;
    char curChar = '\0';
    char numRead = 0;

    std::vector<sf::Sound> sounds;
    std::vector<sf::SoundBuffer> buffers{NUM_NOTES};
    constexpr std::array NOTES{"g#", "a",  "bb", "b", "c",  "c#",
                               "d",  "eb", "e",  "f", "f#", "g"};
    std::vector<char> lastInput(NUM_NOTES, '1');

    for (size_t i = 0; i < NUM_NOTES; ++i) {
        if (!buffers[i].loadFromFile(
                std::format("data/piano-{}.wav", NOTES[i]))) {
            return 1;
        }
        sounds.emplace_back(buffers[i]);
    }

    bool haveValidData = false;

    while (mainWin.isOpen()) {
        while (auto event = mainWin.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                mainWin.close();
            }
        }

        // Attempt a connection
        if (!serialPort.isConnected()) {
            std::vector<std::string> ports = SerialPort::getSerialPorts();
            if (ports.size() > 0) {
                serialPort.connect(ports[0]);
            }
        }

        // Read line of serialPort data
        if (serialPort.isConnected()) {
            while ((numRead = serialPort.read(&curChar, 1)) > 0 &&
                   curChar != '\n' && curChar != '\0') {
                serialPortData += curChar;
            }

            if (numRead == -1) {
                // EOF has been reached (socket disconnected)
                serialPort.disconnect();
            } else if (curChar == '\n' && serialPortData.length() != 0) {
                // If curChar == '\n', there is a new line of complete data
                std::println("{}", serialPortData);

                if (serialPortData.length() == NUM_NOTES) {
                    haveValidData = true;

                    for (size_t i = 0; i < NUM_NOTES; ++i) {
                        if (serialPortData[i] == '0' && lastInput[i] == '1') {
                            sounds[i].play();
                        }

                        lastInput[i] = serialPortData[i];
                    }
                } else {
                    haveValidData = false;
                }

                // Reset serial data storage in preparation for new line of data
                serialPortData.clear();
                curChar = '\0';
                numRead = 0;
            }
        }

        mainWin.clear(sf::Color::White);

        // Render connection indicator
        sf::CircleShape circle{18.f};
        circle.setOrigin(circle.getGeometricCenter());
        circle.setPosition({36.f, 36.f});
        if (serialPort.isConnected()) {
            if (haveValidData) {
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
        mainWin.draw(circle);

        mainWin.display();
    }
}
