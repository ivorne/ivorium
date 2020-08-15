#pragma once

#include "ivorium_core/ivorium_core.hpp"
#include "ivorium_game/ivorium_game.hpp"
#include "ivorium_graphics/ivorium_graphics.hpp"
#include "ivorium_systems/ivorium_systems.hpp"
#include "ivorium_UI/ivorium_UI.hpp"
#include "ivorium_model/ivorium_model.hpp"

namespace iv
{

/**
	Initialises used libraries: GLFW, FreeImage.
    Registers resource basic types to ResourceManagementSystem.
*/
void ivorium_init();

/**
*/
void ivorium_destroy();

}
