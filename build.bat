@echo off
g++ ^
  main.cpp ^
  0-utils/rpg_utils.cpp ^
  1-Characters/character.cpp ^
  1-Characters/stat_utils.cpp ^
  1-Characters/npc_factory.cpp ^
  2-Enemies/enemies.cpp ^
  3-Story/story.cpp ^
  3-Story/act1.cpp ^
  3-Story/act2.cpp ^
  3-Story/act3.cpp ^
  3-Story/act4.cpp ^
  4-Saves/save_system.cpp ^
  5-Combat/CombatManager.cpp ^
  core/status_util.cpp ^
  core/Effect.cpp ^
  core/Ability.cpp ^
  core/AbilityLoader.cpp ^
  -std=c++17 -o game

if %errorlevel% neq 0 (
    echo Build failed.
    pause
    exit /b
)

echo Build successful. After you continue, type ".\game.exe" to run the game...
pause
