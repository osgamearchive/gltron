; config file for Innosetup

[Setup]
AppName=GLtron
AppVerName=GLtron version 0.60
AppCopyright=Copyright © 1999-2001 Andreas Umbach
DefaultDirName={pf}\GLtron
DefaultGroupName=GLtron
UninstallDisplayIcon={app}\GLtron.exe

[Dirs]
Name: "{app}\data"
Name: "{app}\data"
Name: "{app}\art"
Name: "{app}\art\default"
Name: "{app}\art\metalTron"
Name: "{app}\art\arcade_spots"
Name: "{app}\art\biohazard"
Name: "{app}\music"

[Files]
Source: "art\arcade_spots\skybox5.png"; DestDir: "{app}\art\arcade_spots\skybox5.png";
Source: "art\arcade_spots\skybox4.png"; DestDir: "{app}\art\arcade_spots\skybox4.png";
Source: "art\arcade_spots\skybox3.png"; DestDir: "{app}\art\arcade_spots\skybox3.png";
Source: "art\arcade_spots\skybox2.png"; DestDir: "{app}\art\arcade_spots\skybox2.png";
Source: "art\arcade_spots\skybox1.png"; DestDir: "{app}\art\arcade_spots\skybox1.png";
Source: "art\arcade_spots\skybox0.png"; DestDir: "{app}\art\arcade_spots\skybox0.png";
Source: "art\arcade_spots\gltron_wall_4.png"; DestDir: "{app}\art\arcade_spots\gltron_wall_4.png";
Source: "art\arcade_spots\gltron_wall_3.png"; DestDir: "{app}\art\arcade_spots\gltron_wall_3.png";
Source: "art\arcade_spots\gltron_wall_2.png"; DestDir: "{app}\art\arcade_spots\gltron_wall_2.png";
Source: "art\arcade_spots\gltron_wall_1.png"; DestDir: "{app}\art\arcade_spots\gltron_wall_1.png";
Source: "art\arcade_spots\gltron_traildecal.png"; DestDir: "{app}\art\arcade_spots\gltron_traildecal.png";
Source: "art\arcade_spots\gltron_floor.png"; DestDir: "{app}\art\arcade_spots\gltron_floor.png";
Source: "art\arcade_spots\artpack.lua"; DestDir: "{app}\art\arcade_spots\artpack.lua";
Source: "art\biohazard\skybox5.png"; DestDir: "{app}\art\biohazard\skybox5.png";
Source: "art\biohazard\skybox4.png"; DestDir: "{app}\art\biohazard\skybox4.png";
Source: "art\biohazard\skybox3.png"; DestDir: "{app}\art\biohazard\skybox3.png";
Source: "art\biohazard\skybox2.png"; DestDir: "{app}\art\biohazard\skybox2.png";
Source: "art\biohazard\skybox1.png"; DestDir: "{app}\art\biohazard\skybox1.png";
Source: "art\biohazard\skybox0.png"; DestDir: "{app}\art\biohazard\skybox0.png";
Source: "art\biohazard\gltron_wall_4.png"; DestDir: "{app}\art\biohazard\gltron_wall_4.png";
Source: "art\biohazard\gltron_wall_3.png"; DestDir: "{app}\art\biohazard\gltron_wall_3.png";
Source: "art\biohazard\gltron_wall_2.png"; DestDir: "{app}\art\biohazard\gltron_wall_2.png";
Source: "art\biohazard\gltron_wall_1.png"; DestDir: "{app}\art\biohazard\gltron_wall_1.png";
Source: "art\biohazard\gltron_traildecal.png"; DestDir: "{app}\art\biohazard\gltron_traildecal.png";
Source: "art\biohazard\gltron_trail.png"; DestDir: "{app}\art\biohazard\gltron_trail.png";
Source: "art\biohazard\gltron_logo.png"; DestDir: "{app}\art\biohazard\gltron_logo.png";
Source: "art\biohazard\gltron_floor.png"; DestDir: "{app}\art\biohazard\gltron_floor.png";
Source: "art\biohazard\gltron_crash.png"; DestDir: "{app}\art\biohazard\gltron_crash.png";
Source: "art\biohazard\gltron.png"; DestDir: "{app}\art\biohazard\gltron.png";
Source: "art\biohazard\babbage.1.png"; DestDir: "{app}\art\biohazard\babbage.1.png";
Source: "art\biohazard\babbage.0.png"; DestDir: "{app}\art\biohazard\babbage.0.png";
Source: "art\biohazard\artpack.ini"; DestDir: "{app}\art\biohazard\artpack.ini";
Source: "art\biohazard\artpack.lua"; DestDir: "{app}\art\biohazard\artpack.lua";
Source: "art\default\xenotron.1.png"; DestDir: "{app}\art\default\xenotron.1.png";
Source: "art\default\xenotron.0.png"; DestDir: "{app}\art\default\xenotron.0.png";
Source: "art\default\test.bitmap.png"; DestDir: "{app}\art\default\test.bitmap.png";
Source: "art\default\skybox5.png"; DestDir: "{app}\art\default\skybox5.png";
Source: "art\default\skybox4.png"; DestDir: "{app}\art\default\skybox4.png";
Source: "art\default\skybox3.png"; DestDir: "{app}\art\default\skybox3.png";
Source: "art\default\skybox2.png"; DestDir: "{app}\art\default\skybox2.png";
Source: "art\default\skybox1.png"; DestDir: "{app}\art\default\skybox1.png";
Source: "art\default\skybox0.png"; DestDir: "{app}\art\default\skybox0.png";
Source: "art\default\gltron_wall_4.png"; DestDir: "{app}\art\default\gltron_wall_4.png";
Source: "art\default\gltron_wall_3.png"; DestDir: "{app}\art\default\gltron_wall_3.png";
Source: "art\default\gltron_wall_2.png"; DestDir: "{app}\art\default\gltron_wall_2.png";
Source: "art\default\gltron_wall_1.png"; DestDir: "{app}\art\default\gltron_wall_1.png";
Source: "art\default\gltron_traildecal.png"; DestDir: "{app}\art\default\gltron_traildecal.png";
Source: "art\default\gltron_trail.png"; DestDir: "{app}\art\default\gltron_trail.png";
Source: "art\default\gltron_logo.png"; DestDir: "{app}\art\default\gltron_logo.png";
Source: "art\default\gltron_floor.png"; DestDir: "{app}\art\default\gltron_floor.png";
Source: "art\default\gltron_crash.png"; DestDir: "{app}\art\default\gltron_crash.png";
Source: "art\default\gltron_bitmap.png"; DestDir: "{app}\art\default\gltron_bitmap.png";
Source: "art\default\gltron.png"; DestDir: "{app}\art\default\gltron.png";
Source: "art\default\babbage.1.png"; DestDir: "{app}\art\default\babbage.1.png";
Source: "art\default\babbage.0.png"; DestDir: "{app}\art\default\babbage.0.png";
Source: "art\default\artpack.ini"; DestDir: "{app}\art\default\artpack.ini";
Source: "art\default\artpack.lua"; DestDir: "{app}\art\default\artpack.lua";
Source: "art\metalTron\skybox5.png"; DestDir: "{app}\art\metalTron\skybox5.png";
Source: "art\metalTron\skybox4.png"; DestDir: "{app}\art\metalTron\skybox4.png";
Source: "art\metalTron\skybox3.png"; DestDir: "{app}\art\metalTron\skybox3.png";
Source: "art\metalTron\skybox2.png"; DestDir: "{app}\art\metalTron\skybox2.png";
Source: "art\metalTron\skybox1.png"; DestDir: "{app}\art\metalTron\skybox1.png";
Source: "art\metalTron\skybox0.png"; DestDir: "{app}\art\metalTron\skybox0.png";
Source: "art\metalTron\gltron_wall_4.png"; DestDir: "{app}\art\metalTron\gltron_wall_4.png";
Source: "art\metalTron\gltron_wall_3.png"; DestDir: "{app}\art\metalTron\gltron_wall_3.png";
Source: "art\metalTron\gltron_wall_2.png"; DestDir: "{app}\art\metalTron\gltron_wall_2.png";
Source: "art\metalTron\gltron_wall_1.png"; DestDir: "{app}\art\metalTron\gltron_wall_1.png";
Source: "art\metalTron\gltron_floor.png"; DestDir: "{app}\art\metalTron\gltron_floor.png";
Source: "art\metalTron\artpack.lua"; DestDir: "{app}\art\metalTron\artpack.lua";
Source: "art\metalTron\artpack.ini"; DestDir: "{app}\art\metalTron\artpack.ini";
Source: "data\babbage.ftx"; DestDir: "{app}\data\babbage.ftx";
Source: "data\fonts.txt"; DestDir: "{app}\data\fonts.txt";
Source: "data\game_crash.ogg"; DestDir: "{app}\data\game_crash.ogg";
Source: "data\game_crash.wav"; DestDir: "{app}\data\game_crash.wav";
Source: "data\game_engine.ogg"; DestDir: "{app}\data\game_engine.ogg";
Source: "data\game_engine.wav"; DestDir: "{app}\data\game_engine.wav";
Source: "data\game_recognizer.ogg"; DestDir: "{app}\data\game_recognizer.ogg";
Source: "data\game_recognizer.wav"; DestDir: "{app}\data\game_recognizer.wav";
Source: "data\lightcycle-high.obj"; DestDir: "{app}\data\lightcycle-high.obj";
Source: "data\lightcycle-low.obj"; DestDir: "{app}\data\lightcycle-low.obj";
Source: "data\lightcycle-med.obj"; DestDir: "{app}\data\lightcycle-med.obj";
Source: "data\lightcycle.mtl"; DestDir: "{app}\data\lightcycle.mtl";
Source: "data\recognizer.mtl"; DestDir: "{app}\data\recognizer.mtl";
Source: "data\recognizer.obj"; DestDir: "{app}\data\recognizer.obj";
Source: "data\recognizer_quad.obj"; DestDir: "{app}\data\recognizer_quad.obj";
Source: "data\settings.txt"; DestDir: "{app}\data\settings.txt";
Source: "data\test.fbmp"; DestDir: "{app}\data\test.fbmp";
Source: "data\xenotron.ftx"; DestDir: "{app}\data\xenotron.ftx";
Source: "music\song_revenge_of_cats.it"; DestDir: "{app}\music\song_revenge_of_cats.it";
Source: "scripts\audio.lua"; DestDir: "{app}\scripts\audio.lua";
Source: "scripts\basics.lua"; DestDir: "{app}\scripts\basics.lua";
Source: "scripts\config.lua"; DestDir: "{app}\scripts\config.lua";
Source: "scripts\menu.lua"; DestDir: "{app}\scripts\menu.lua";
Source: "scripts\menu_functions.lua"; DestDir: "{app}\scripts\menu_functions.lua";
Source: "scripts\save.lua"; DestDir: "{app}\scripts\save.lua";
Source: "scripts\video.lua"; DestDir: "{app}\scripts\video.lua";
Source: "SDL.dll"; DestDir: "{app}\SDL.dll";
Source: "png.dll"; DestDir: "{app}\png.dll";
Source: "z.dll"; DestDir: "{app}\z.dll";
Source: "README.html"; DestDir: "{app}\README.html"; Flags: isreadme
Source: "gltron.exe"; DestDir: "{app}\gltron.exe";

[Icons]
Name: "{group}\GLtron"; Filename: "{app}\GLtron.exe"; WorkingDir: "{app}";
Name: "{group}\GLtron documentation"; Filename: "{app}\Readme.html";
Name: "{group}\Uninstall GLtron"; Filename: "{app}\unins000.exe";

[UninstallDelete]
Type: files; Name: "{app}\gltron.ini"
Type: files; Name: "{app}\stderr.txt"
Type: files; Name: "{app}\stdout.txt"

