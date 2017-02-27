// Copyright (c) Tyler Veness 2015-2017. All Rights Reserved.

#pragma once

#include <vector>

#include <SFML/Graphics/Font.hpp>
#include <SFML/OpenGL.hpp>

#include "KalmanFilter.hpp"

const unsigned int sen = 3;  // sensors

/**
 * A container for globals to pass to rendering functions
 */
class RenderData {
public:
    RenderData();

    std::vector<KalmanFilter> avgPos{sen, KalmanFilter(0.00004, 0.0004)};
    bool isConnected{false};
    bool haveValidData{false};

    std::vector<float> rawPos{sen, 0.f};
    bool useRawInput{false};

    sf::Font font;
};
