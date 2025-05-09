@echo off
g++ ^
  main.cpp ^
  src/Characters/character.cpp ^
  src/Characters/npc_factory.cpp ^
  src/Combat/Ability.cpp ^
  src/Combat/AbilityLoader.cpp ^
  src/Combat/CombatManager.cpp ^
  src/Combat/Effect.cpp ^
  src/Combat/Status.cpp ^
  src/Utilities/rpg_utils.cpp ^
  src/Utilities/save_system.cpp ^
  src/Utilities/stat_utils.cpp ^
  src/Utilities/trade.cpp ^
  src/enemies.cpp ^
  src/story/act1.cpp ^
  src/story/story.cpp ^
  -std=c++17 -Iinclude -o game.exe

if %errorlevel% neq 0 (
    echo Build failed.
    pause
    exit /b
)

echo Build successful. After you continue, type ".\game.exe" to run the game...
pause
