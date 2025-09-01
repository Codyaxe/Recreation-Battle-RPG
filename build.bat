@echo off
echo Building Battle RPG...
g++ -g ^
  -I"C:\Code\Projects\Battle RPG\include" ^
  "C:\Code\Projects\Battle RPG\main.cpp" ^
  "C:\Code\Projects\Battle RPG\src\actions\action.cpp" ^
  "C:\Code\Projects\Battle RPG\src\actions\attacks.cpp" ^
  "C:\Code\Projects\Battle RPG\src\actions\buffs.cpp" ^
  "C:\Code\Projects\Battle RPG\src\actions\debuffs.cpp" ^
  "C:\Code\Projects\Battle RPG\src\actions\items.cpp" ^
  "C:\Code\Projects\Battle RPG\src\actions\spells.cpp" ^
  "C:\Code\Projects\Battle RPG\src\actions\summons.cpp" ^
  "C:\Code\Projects\Battle RPG\src\actions\traits.cpp" ^
  "C:\Code\Projects\Battle RPG\src\core\character.cpp" ^
  "C:\Code\Projects\Battle RPG\src\core\components.cpp" ^
  "C:\Code\Projects\Battle RPG\src\core\effects.cpp" ^
  "C:\Code\Projects\Battle RPG\src\core\observer.cpp" ^
  "C:\Code\Projects\Battle RPG\src\core\status.cpp" ^
  "C:\Code\Projects\Battle RPG\src\ui\technical.cpp" ^
  -o "C:\Code\Projects\Battle RPG\main.exe"
if %ERRORLEVEL% EQU 0 (
    echo Build successful! Executable created: main.exe
) else (
    echo Build failed!
)
pause
