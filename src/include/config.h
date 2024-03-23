#ifndef CONFIG_H_
#define CONFIG_H_

#include <SFML/Window/Keyboard.hpp>
#include <stddef.h>

// window config --------------------------------------------------------------

static const unsigned int kWindowWidth = 1920;
static const unsigned int kWindowHight = 1080;
static const char* kWindowTitle [[maybe_unused]] = "Mandelbrot!" ;

static const sf::Keyboard::Key kButtonMoveLeft    = sf::Keyboard::Left;
static const sf::Keyboard::Key kButtonMoveRight   = sf::Keyboard::Right;
static const sf::Keyboard::Key kButtonMoveUp      = sf::Keyboard::Up;
static const sf::Keyboard::Key kButtonMoveDown    = sf::Keyboard::Down;
static const sf::Keyboard::Key kButtonDefaultView = sf::Keyboard::Num0;
static const sf::Keyboard::Key kButtonZoomIn      = sf::Keyboard::Z;
static const sf::Keyboard::Key kButtonZoomOut     = sf::Keyboard::X;
static const sf::Keyboard::Key kButtonQuit        = sf::Keyboard::Q;

// compute config -------------------------------------------------------------

static const size_t kMaxIteration = 48;

// #define COMPUTE_NAIVE 1
// #define COMPUTE_ARRAY 1
#define COMPUTE_SIMD 1
// #define LOG_TIME 1

#endif // CONFIG_H_