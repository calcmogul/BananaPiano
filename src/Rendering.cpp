// =============================================================================
// File Name: Rendering.cpp
// Description: Provides helper functions for drawing various elements of the
//              GUI with OpenGL
// Author: Tyler Veness
// =============================================================================

#include "Rendering.hpp"
#include "GLUtils.hpp"

#include <SFML/OpenGL.hpp>

void renderConnectionIndicator(sf::RenderWindow* window, RenderData& data) {
    // Save projection matrix
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();

    // Set up projection matrix
    glLoadIdentity();
    glOrtho(0, window->getSize().x, window->getSize().y, 0, 0, 1);

    // Save modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // Set up modelview matrix
    glLoadIdentity();

    // Translate circle away from top left of window
    glTranslatef(36.f, 36.f, 0.f);

    if (data.isConnected) {
        if (data.haveValidData) {
            // Connected and valid data
            glColor3ub(0, 200, 0);
        }
        else {
            // Connected but no valid data
            glColor3ub(255, 220, 0);
        }
    }
    else {
        // Disconnected
        glColor3ub(200, 0, 0);
    }

    glDisable(GL_LIGHTING);
    drawCircle(18.f, 32.f);
    glEnable(GL_LIGHTING);

    // Restore modelview matrix
    glPopMatrix();

    // Restore projection matrix
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void renderNotes(sf::RenderWindow* window, RenderData& data) {
    window->setActive();

    // Set up window
    glViewport(0, 0, window->getSize().x, window->getSize().y);

    // Clear the buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderConnectionIndicator(window, data);

    window->display();
}

