# SoT external ESP

Join discord to suggest changes/ask for help/pillage the seas or help build this project
https://discord.gg/V6MxwrT

To debug/play with source:

1. Install DirectX SDK: https://www.microsoft.com/en-ca/download/details.aspx?id=6812
You may need to uninstall all Visual C++ updates after 2010 to install SDK
2. Install Visual Studio: https://visualstudio.microsoft.com/
VS Community is free, and works fine
3. Download entire project, and open the solution. Run it via debug.
Any errors are probably from the DX3D SDK libraries not properly linked in project. 

To use the ESP without debugging, just download the .exe and run it while SoT is running. 

MUST BE IN WINDOWED MODE TO WORK. 

Current issues:
- X marks the spot doesn't work since 1.3 update. 
  - TeasureMaps is now an empty array. ```:UIslandDataAssetEntry:TreasureMaps:SoT_Athena_classes.hpp```
- Other player health bar isn't showing

To do:
- Fix current issues
- Make internal
- Add external map
  - List all players on server
  - Show active volcanos
  - Show player/ship position
  - Show X marks on island
  - Show selected nearby items (I.E Cursed balls, in barrels) MAYBE

Keys
- F8 Debug on
  - F7 Debug off
- F6 Hoarders items on
- F5 Order Of Souls items on
- F4 Merchants items on
- F3 All rep items off
