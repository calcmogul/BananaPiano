// =============================================================================
// File Name: Rendering.hpp
// Description: Provides helper functions for drawing various elements of the
//              GUI with OpenGL
// Author: Tyler Veness
// =============================================================================

#ifndef RENDERING_HPP
#define RENDERING_HPP

#include <SFML/Graphics/RenderWindow.hpp>
#include <vector>
#include "RenderData.hpp"

void renderConnectionIndicator(sf::RenderWindow* window, RenderData& data);

void renderNotes(sf::RenderWindow* window, RenderData& data);

#endif // RENDERING_HPP

