cmake_minimum_required(VERSION 3.16)
project(OpenFacto LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_SHARED_LIBS "Build shared libs" OFF)

include(FetchContent)
FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG 5.5
)
set(FETCHCONTENT_QUIET OFF)
FetchContent_MakeAvailable(raylib)

add_executable(openfacto
  src/main.cpp
  src/game/Game.cpp
  src/core/Time.cpp
  src/core/Input.cpp
  src/world/World.cpp
  src/world/Gen.cpp
  src/render/Renderer.cpp
  src/ui/UI.cpp
  src/ecs/Entity.cpp
  src/systems/MovementSystem.cpp
  src/systems/MiningSystem.cpp
  src/systems/CraftSystem.cpp
  src/systems/AutomationSystem.cpp
  src/systems/PowerSystem.cpp
  src/systems/EnemySystem.cpp
  src/systems/CombatSystem.cpp
  src/data/Items.cpp
  src/data/Recipes.cpp
  src/data/TechTree.cpp
)

target_include_directories(openfacto PRIVATE src)
target_link_libraries(openfacto PRIVATE raylib)

if (MSVC)
  target_compile_options(openfacto PRIVATE /W4)
else()
  target_compile_options(openfacto PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(openfacto PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
