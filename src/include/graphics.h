#ifndef GRAPHICS_CFG_H_
#define GRAPHICS_CFG_H_

#include <SFML/Graphics.hpp>
#include <SFML/Config.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/WindowStyle.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>

#include "config.h"
#include "mandelbrot.h"

void Render(sf::RenderWindow& window, const Mandelbrot::MSet& m_set);
void CheckWindowEvents(sf::RenderWindow& window, Mandelbrot::MSet& m_set);

#endif // GRAPHICS_CFG_H_