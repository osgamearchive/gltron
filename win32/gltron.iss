; config file for Innosetup

[Setup]
AppName=GLtron
AppVerName=GLtron version 0.70-alpha-1
AppCopyright=Copyright © 1999-2003 Andreas Umbach
DefaultDirName={pf}\GLtron
DefaultGroupName=GLtron
UninstallDisplayIcon={app}\GLtron.exe

[Dirs]
Name: "{app}\data"
Name: "{app}\scripts"
Name: "{app}\art"
Name: "{app}\art\classic"
Name: "{app}\art\metalTron"
Name: "{app}\art\default"
Name: "{app}\music"
Name: "{app}\docs"

[Files]
Source: "data\babbage.ftx"; DestDir: "{app}\data\";
Source: "data\fonts.txt"; DestDir: "{app}\data\";
Source: "data\game_crash.wav"; DestDir: "{app}\data\";
Source: "data\game_engine.wav"; DestDir: "{app}\data\";
Source: "data\game_recognizer.wav"; DestDir: "{app}\data\";
Source: "data\lightcycle-high.obj"; DestDir: "{app}\data\";
Source: "data\lightcycle-low.obj"; DestDir: "{app}\data\";
Source: "data\lightcycle-med.obj"; DestDir: "{app}\data\";
Source: "data\lightcycle.mtl"; DestDir: "{app}\data\";
Source: "data\recognizer.mtl"; DestDir: "{app}\data\";
Source: "data\recognizer.obj"; DestDir: "{app}\data\";
Source: "data\recognizer_quad.obj"; DestDir: "{app}\data\";
Source: "data\test.fbmp"; DestDir: "{app}\data\";
Source: "data\xenotron.ftx"; DestDir: "{app}\data\";
Source: "art\classic\artpack.lua"; DestDir: "{app}\art\classic\";
Source: "art\classic\babbage.0.png"; DestDir: "{app}\art\classic\";
Source: "art\classic\babbage.1.png"; DestDir: "{app}\art\classic\";
Source: "art\classic\gltron_floor.png"; DestDir: "{app}\art\classic\";
Source: "art\classic\gltron_trail.png"; DestDir: "{app}\art\classic\";
Source: "art\classic\gltron_traildecal.png"; DestDir: "{app}\art\classic\";
Source: "art\classic\gltron_wall_1.png"; DestDir: "{app}\art\classic\";
Source: "art\classic\gltron_wall_2.png"; DestDir: "{app}\art\classic\";
Source: "art\classic\gltron_wall_3.png"; DestDir: "{app}\art\classic\";
Source: "art\classic\gltron_wall_4.png"; DestDir: "{app}\art\classic\";
Source: "art\classic\readme.txt"; DestDir: "{app}\art\classic\";
Source: "art\classic\skybox0.png"; DestDir: "{app}\art\classic\";
Source: "art\classic\skybox1.png"; DestDir: "{app}\art\classic\";
Source: "art\classic\skybox2.png"; DestDir: "{app}\art\classic\";
Source: "art\classic\skybox3.png"; DestDir: "{app}\art\classic\";
Source: "art\classic\skybox4.png"; DestDir: "{app}\art\classic\";
Source: "art\classic\skybox5.png"; DestDir: "{app}\art\classic\";
Source: "art\classic\xenotron.0.png"; DestDir: "{app}\art\classic\";
Source: "art\classic\xenotron.1.png"; DestDir: "{app}\art\classic\";
Source: "art\default\artpack.lua"; DestDir: "{app}\art\default\";
Source: "art\default\babbage.0.png"; DestDir: "{app}\art\default\";
Source: "art\default\babbage.1.png"; DestDir: "{app}\art\default\";
Source: "art\default\gltron.png"; DestDir: "{app}\art\default\";
Source: "art\default\gltron_bitmap.png"; DestDir: "{app}\art\default\";
Source: "art\default\gltron_floor.png"; DestDir: "{app}\art\default\";
Source: "art\default\gltron_impact.png"; DestDir: "{app}\art\default\";
Source: "art\default\gltron_logo.png"; DestDir: "{app}\art\default\";
Source: "art\default\gltron_trail.png"; DestDir: "{app}\art\default\";
Source: "art\default\gltron_traildecal.png"; DestDir: "{app}\art\default\";
Source: "art\default\gltron_wall_1.png"; DestDir: "{app}\art\default\";
Source: "art\default\gltron_wall_2.png"; DestDir: "{app}\art\default\";
Source: "art\default\gltron_wall_3.png"; DestDir: "{app}\art\default\";
Source: "art\default\gltron_wall_4.png"; DestDir: "{app}\art\default\";
Source: "art\default\skybox0.png"; DestDir: "{app}\art\default\";
Source: "art\default\skybox1.png"; DestDir: "{app}\art\default\";
Source: "art\default\skybox2.png"; DestDir: "{app}\art\default\";
Source: "art\default\skybox3.png"; DestDir: "{app}\art\default\";
Source: "art\default\skybox4.png"; DestDir: "{app}\art\default\";
Source: "art\default\skybox5.png"; DestDir: "{app}\art\default\";
Source: "art\default\test.bitmap.png"; DestDir: "{app}\art\default\";
Source: "art\default\xenotron.0.png"; DestDir: "{app}\art\default\";
Source: "art\default\xenotron.1.png"; DestDir: "{app}\art\default\";
Source: "art\metalTron\artpack.lua"; DestDir: "{app}\art\metalTron\";
Source: "art\metalTron\gltron_floor.png"; DestDir: "{app}\art\metalTron\";
Source: "art\metalTron\gltron_wall_1.png"; DestDir: "{app}\art\metalTron\";
Source: "art\metalTron\gltron_wall_2.png"; DestDir: "{app}\art\metalTron\";
Source: "art\metalTron\gltron_wall_3.png"; DestDir: "{app}\art\metalTron\";
Source: "art\metalTron\gltron_wall_4.png"; DestDir: "{app}\art\metalTron\";
Source: "art\metalTron\skybox0.png"; DestDir: "{app}\art\metalTron\";
Source: "art\metalTron\skybox1.png"; DestDir: "{app}\art\metalTron\";
Source: "art\metalTron\skybox2.png"; DestDir: "{app}\art\metalTron\";
Source: "art\metalTron\skybox3.png"; DestDir: "{app}\art\metalTron\";
Source: "art\metalTron\skybox4.png"; DestDir: "{app}\art\metalTron\";
Source: "art\metalTron\skybox5.png"; DestDir: "{app}\art\metalTron\";
Source: "music\revenge_of_cats.it"; DestDir: "{app}\music\";
Source: "scripts\artpack.lua"; DestDir: "{app}\scripts\";
Source: "scripts\audio.lua"; DestDir: "{app}\scripts\";
Source: "scripts\basics.lua"; DestDir: "{app}\scripts\";
Source: "scripts\config.lua"; DestDir: "{app}\scripts\";
Source: "scripts\joystick.lua"; DestDir: "{app}\scripts\";
Source: "scripts\layout.lua"; DestDir: "{app}\scripts\";
Source: "scripts\main.lua"; DestDir: "{app}\scripts\";
Source: "scripts\menu.lua"; DestDir: "{app}\scripts\";
Source: "scripts\menu_functions.lua"; DestDir: "{app}\scripts\";
Source: "scripts\save.lua"; DestDir: "{app}\scripts\";
Source: "scripts\video.lua"; DestDir: "{app}\scripts\";
Source: "MSVCR70.dll"; DestDir: "{app}\";
Source: "SDL.dll"; DestDir: "{app}\";
Source: "libpng.dll"; DestDir: "{app}\";
Source: "mikmod.dll"; DestDir: "{app}\";
Source: "ogg.dll"; DestDir: "{app}\";
Source: "sdl_sound.dll"; DestDir: "{app}\";
Source: "smpeg.dll"; DestDir: "{app}\";
Source: "vorbis.dll"; DestDir: "{app}\";
Source: "vorbisfile.dll"; DestDir: "{app}\";
Source: "zlib.dll"; DestDir: "{app}\";
Source: "docs\README.html"; DestDir: "{app}\docs\"; Flags: isreadme
Source: "gltron.exe"; DestDir: "{app}\";

[Icons]
Name: "{group}\GLtron"; Filename: "{app}\GLtron.exe"; WorkingDir: "{app}";
Name: "{group}\GLtron documentation"; Filename: "{app}\Readme.html";
Name: "{group}\Uninstall GLtron"; Filename: "{app}\unins000.exe";

[UninstallDelete]
Type: files; Name: "{app}\gltron.ini"
Type: files; Name: "{app}\stderr.txt"
Type: files; Name: "{app}\stdout.txt"

