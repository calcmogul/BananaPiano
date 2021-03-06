// Copyright (c) Tyler Veness 2015-2017. All Rights Reserved.

#include <bitset>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Window/Event.hpp>

#include "KalmanFilter.hpp"
#include "RenderData.hpp"
#include "Rendering.hpp"
#include "SerialPort.hpp"

const int numNotes = 3;

// Implements mouse input driver for 3D capacitor
int main() {
    RenderData renderData;

    SerialPort serialPort;

    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 0;

    // Setup
    sf::RenderWindow mainWin(sf::VideoMode(72, 72), "Banana Piano",
                             sf::Style::Resize | sf::Style::Close, settings);
    mainWin.setFramerateLimit(25);

    mainWin.setActive();

    // Set buffer clear values
    glClearColor(1.f, 1.f, 1.f, 1.f);
    glClearDepth(1.f);

    // Enable depth buffering
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);

    // Enable alpha blending
    glEnable(GL_BLEND);
    glEnable(GL_ALPHA_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Disable textures since they are unneeded
    glDisable(GL_TEXTURE_2D);

    // Declare lighting parameters
    GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat mat_shininess[] = {50.0};
    GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0};

    // Set lighting parameters
    glShadeModel(GL_SMOOTH);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    // Enable lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    // Used to store data read from serial port or socket
    std::string serialPortData;
    char curChar = '\0';
    char numRead = 0;

    std::vector<sf::Sound> sounds;
    std::vector<sf::SoundBuffer> buffers{numNotes};
    std::vector<std::string> notes{"g#", "a",  "bb", "b", "c",  "c#",
                                   "d",  "eb", "e",  "f", "f#", "g"};
    std::vector<char> lastInput(numNotes, '1');

    for (unsigned int i = 0; i < numNotes; i++) {
        if (!buffers[i].loadFromFile(std::string("sounds/piano-") + notes[i] +
                                     ".wav")) {
            return 1;
        }
        sounds.emplace_back(buffers[i]);
    }

    sf::Event event;
    while (mainWin.isOpen()) {
        while (mainWin.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
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
                std::cout << serialPortData << std::endl;

                if (serialPortData.length() == numNotes) {
                    renderData.haveValidData = true;

                    for (unsigned int i = 0; i < numNotes; i++) {
                        if (serialPortData[i] == '0' && lastInput[i] == '1') {
                            sounds[i].play();
                        }

                        lastInput[i] = serialPortData[i];
                    }
                } else {
                    renderData.haveValidData = false;
                }

                // Reset serial data storage in preparation for new line of data
                serialPortData.clear();
                curChar = '\0';
                numRead = 0;
            }
        }

        renderData.isConnected = serialPort.isConnected();

        renderNotes(&mainWin, renderData);
    }

    return 0;
}
