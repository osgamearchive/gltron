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
Name: "{app}\art\movie"
Name: "{app}\music"

[Files]
Source: "data\babbage.ftx"; DestDir: "{app}\data\babbage.ftx";
Source: "data\fonts.txt"; DestDir: "{app}\data\fonts.txt";
Source: "data\game_crash.wav"; DestDir: "{app}\data\game_crash.wav";
Source: "data\game_engine.wav"; DestDir: "{app}\data\game_engine.wav";
Source: "data\game_recognizer.wav"; DestDir: "{app}\data\game_recognizer.wav";
Source: "data\lightcycle-high.obj"; DestDir: "{app}\data\lightcycle-high.obj";
Source: "data\lightcycle-low.obj"; DestDir: "{app}\data\lightcycle-low.obj";
Source: "data\lightcycle-med.obj"; DestDir: "{app}\data\lightcycle-med.obj";
Source: "data\lightcycle.mtl"; DestDir: "{app}\data\lightcycle.mtl";
Source: "data\recognizer.mtl"; DestDir: "{app}\data\recognizer.mtl";
Source: "data\recognizer.obj"; DestDir: "{app}\data\recognizer.obj";
Source: "data\recognizer_quad.obj"; DestDir: "{app}\data\recognizer_quad.obj";
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

