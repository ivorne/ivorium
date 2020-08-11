#pragma once

#include "TimeSystem/TimeSystem.hpp"
#include "TimeSystem/FrameUpdateClient.hpp"
#include "TimeSystem/TimeId.hpp"
#include "TimeSystem/FixedUpdateClient.hpp"
#include "TimeSystem/TimeManipulator.hpp"
#include "TimeSystem/Watch.hpp"
#include "fs.hpp"
#include "ResourceManagementSystem/ResourcePath.hpp"
#include "ResourceManagementSystem/StreamResource/StreamResourceSubprovider.hpp"
#include "ResourceManagementSystem/StreamResource/StreamResourceProvider.hpp"
#include "ResourceManagementSystem/ResourceAccess.hpp"
#include "ResourceManagementSystem/SingularResource.hpp"
#include "ResourceManagementSystem/ResourceProvider.hpp"
#include "ResourceManagementSystem/VirtualResource/VirtualResourceProvider.hpp"
#include "ResourceManagementSystem/ResourceManagementSystem.hpp"
#include "ResourceManagementSystem/DataStream.hpp"
#include "ResourceManagementSystem/Providers/FSResourceProvider.hpp"
#include "DVarSystem/DVarManipulator.hpp"
#include "DVarSystem/DVar_Field.hpp"
#include "DVarSystem/DVarSystem.hpp"
#include "DVarSystem/DVarProperty.hpp"
#include "DVarSystem/DVarCloner.hpp"
#include "DVarSystem/DVar.hpp"
#include "DVarSystem/DVarId.hpp"
#include "DVarSystem/DVarListener.hpp"
#include "Defs.hpp"
#include "ConfigFileSystem/ConfigFileSystem.hpp"
#include "RandomSystem/RandomId.hpp"
#include "RandomSystem/RandomSystem.hpp"
#include "RandomSystem/RandomGenerator.hpp"
#include "DelayedLoadSystem/DelayedLoad.hpp"
#include "DelayedLoadSystem/DelayedLoader.hpp"
#include "DelayedLoadSystem/DelayedLoadSystem.hpp"
#include "InputSystem/InputBindingSystem/InputBinder.hpp"
#include "InputSystem/InputBindingSystem/InputBindingListener.hpp"
#include "InputSystem/InputBindingSystem/InputBindingQuery.hpp"
#include "InputSystem/InputBindingSystem/InputId.hpp"
#include "InputSystem/InputBindingSystem/InputBindingSystem.hpp"
#include "InputSystem/InputEvent.hpp"
#include "InputSystem/SourceInputNode.hpp"
#include "InputSystem/Char_InputNode.hpp"
#include "InputSystem/InputNode.hpp"
#include "InputSystem/Key_InputNode.hpp"
#include "InputSystem/InputSystem/InputSystem.hpp"
#include "InputSystem/InputSystem/InputQuery.hpp"
#include "InputSystem/Position_InputNode.hpp"
#include "InputSystem/BlockAll_InputNode.hpp"
