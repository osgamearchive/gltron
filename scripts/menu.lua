Menu.type.menu = 1
Menu.type.list = 2
Menu.type.slider = 3
Menu.type.key = 4
Menu.type.string = 5
Menu.type.number = 6
Menu.type.action = 7

-- Menu definitions
RootMenu = { type = Menu.type.menu, caption = "" }
GameMenu = { type = Menu.type.menu, caption = "Game" }
  GameRulesMenu = { type = Menu.type.action, caption = "Game Rules" }
  GameSettingsMenu = { type = Menu.type.action, caption = "Play Settings" }
  PlayerConfigMenu = { type = Menu.type.action, caption = "Configure Players" }
  KeyConfigMenu = { type = Menu.type.action, caption = "Configure Keys" }
    Player1_KeyMenu = { type = Menu.type.action, caption = "Player 1" }
    Player2_KeyMenu = { type = Menu.type.action, caption = "Player 2" }
    Player3_KeyMenu = { type = Menu.type.action, caption = "Player 3" }
    Player4_KeyMenu = { type = Menu.type.action, caption = "Player 4" }
VideoMenu = { type = Menu.type.action, caption = "Video" }
  TextureMenu = { type = Menu.type.action, caption = "Texture Options" }
  DetailsMenu = { type = Menu.type.action, caption = "Detail Options" }
  ScreenMenu = { type = Menu.type.action, caption = "Screen Options" }
AudioMenu = { type = Menu.type.action, caption = "Audio" }

-- Item definitions
StartGame = { type = Menu.type.action, caption = "Start Game" }
StartGame.action = function() c_startGame(); end
ResetScores = { type = Menu.type.action, caption = "Reset Scores" }
ResetScores.action = function() c_resetScors(); end

-- Game Rules
GameSpeed = { 
   type = Menu.type.list, caption = "Game speed",
   labels = { "boring", "normal", "fast", "crazy" },
   values = { 5.0, 6.5, 8.5, 12.0 },
   store = function (value) speed = value; c_resetGame(); end
}
BotSkill = {
   type = Menu.type.list, caption = "Bot Skill",
   labels = { "dumb", "normal", "strong", "the MCP himself" },
   values = { 0, 1, 2, 3 },
   store = function (value) ai_level = value; end
}
ArenaSize = {
   type = Menu.type.list, caption = "Arena Size",
   labels = { "tiny", "medium", "big", "vast", "extreme" },
   values = { 160, 240, 480, 720, 1200 },
   store = function (value) grid_size = value; c_resetGame(); end
}
EraseDeadPlayer = {    
   type = Menu.type.list, caption = "Erase dead players",
   labels = { "off", "on" },
   values = { 0, 1 },
   store = function(value) erase_crashed = value; c_resetGame(); end
}

-- GameSettings
Fast finish = {    
   type = Menu.type.list, caption = "Fast finish",
   labels = { "off", "on" },
   values = { 0, 1 },
   store = function(value) fast_finish = value; end
}
Camera Mode = {    
   type = Menu.type.list, caption = "Camera mode",
   labels = { "circling", "behind", "cockpit", "mouse" },
   values = { 0, 1, 2, 3 },
   store = function(value) camType = value; end
}
Viewports = {    
   type = Menu.type.list, caption = "Viewports",
   labels = { "single", "split", "4 player", "auto" },
   values = { 0, 1, 2, 3 },
   store = function(value) display_type = value; (); end
}

-- Player
Player1 = {    
   type = Menu.type.list, caption = "Player 1", 
   labels = { "Human", "Computer", "None" },
   values = { 0, 1, 2 },
   store = function(value) ai_player1 = value; c_resetGame(); end
}

Player2 = {    
   type = Menu.type.list, caption = "Player 2", 
   labels = { "Human", "Computer", "None" },
   values = { 0, 1, 2 },
   store = function(value) ai_player2 = value; c_resetGame(); end
}

Player3 = {    
   type = Menu.type.list, caption = "Player 3", 
   labels = { "Human", "Computer", "None" },
   values = { 0, 1, 2 },
   store = function(value) ai_player3 = value; c_resetGame(); end
}

Player4 = {    
   type = Menu.type.list, caption = "Player 4", 
   labels = { "Human", "Computer", "None" },
   values = { 0, 1, 2 },
   store = function(value) ai_player4 = value; c_resetGame(); end
}

-- PlayerX_Key
Player1_Left = { 
   type = Menu.type.key 
}
-- TODO: fill in the rest of the items

-- Video

-- Texture
Artpack = {
   Menu.type.list, caption = "Artpack",
   labels = { } -- initialized by directory listing code
   values = { } -- initialized by directory listing code
   store = function(value) artpack = value; c_reloadTextures(); end
}
Floor = {
   Menu.type.list, caption = "Floor texture",
   labels = { "off", "on" },
   values = { 0, 1 },
   store = function(value) show_floor_texture = value; end
}
Wall = {
   Menu.type.list, caption = "Wall texture",
   labels = { "off", "on" },
   values = { 0, 1 },
   store = function(value) show_wall = value; end
}
Floor = {
   Menu.type.list, caption = "Floor texture",
   labels = { "off", "on" },
   values = { 0, 1 },
   store = function(value) show_floor_texture = value; end
}
Crash = {
   Menu.type.list, caption = "Crash texture",
   labels = { "off", "on" },
   values = { 0, 1 },
   store = function(value) show_crash_texture = value; end
}
Skybox = {
   Menu.type.list, caption = "Skybox texture",
   labels = { "off", "on" },
   values = { 0, 1 },
   store = function(value) show_skybox = value; end
}
Decals = {
   Menu.type.list, caption = "Trail decals",
   labels = { "off", "on" },
   values = { 0, 1 },
   store = function(value) show_decals = value; end
}
Mipmaps = {
   Menu.type.list, caption = "Use mipmaps",
   labels = { "off", "on" },
   values = { 0, 1 },
   store = function(value) use_mipmaps = value; end
}
Filtering = {
   Menu.type.list, caption = "Mitmap filter",
   labels = { "bilinear", "trilinear" },
   values = { 0, 1 },
   store = function(value) mipmap_filter = value; end
}
WallScaling = {
   Menu.type.list, caption = "Wall Scaling",
   labels = { "off", "on" },
   values = { 0, 1 },
   store = function(value) stretch_textures = value; end
}
-- Detail
   
-- Menu entries
RootMenu.items = { GameMenu, VideoMenu, AudioMenu, Quit }

GameMenu.items = { 
   StartGame, ResetScores, GameRulesMenu, 
   GameSettingsMenu, PlayerConfigMenu, KeyConfigMenu
}
  GameRulesMenu.items = { GameSpeed, BotSkill, ArenaSize, EraseDeadPlayers }
  GameSettingsMenu.items = { FastFinish, CameraMode, Viewports }
  PlayerConfigMenu.items = { Player1, Player2, Player3, Player4 }
  KeyConfig.items = { 
    Player1_KeyMenu, Player2_KeyMenu, Player3_KeyMenu,Player4_KeyMenu
  }
    Player1_KeyMenu.items = { Player1_Left, Player1_Right }
    Player2_KeyMenu.items { Player2_Left, Player2_Right }
    Player3_KeyMenu.items { Player3_Left, Player3_Right }
    Player4_KeyMenu.items { Player4_Left, Player4_Right }

VideoMenu.items = { TextureMenu, DetailsMenu, ScreenMenu }
  TextureMenu.items = { 
    Artpack, Floor, Wall, Crash, Skybox, 
    Decals, Mipmaps, Filtering, WallScaling
  }
  DetailMenu.items = {
    AlphaTrails, Halos, Lightcycles, Recognizer, CycleLod, ShadowLod, 
    FPS_Counter, AI_Status, Scores
  }
  ScreenMenu.items = {
    _320x240, _512x384, _640x480, _800x600, _1024x768, _1280x1024,
    _1600x1200, WindowMode
  }

AudioMenu.items = { Music, FX, Music_Volume, FX_Volume, Song }
