Menu.type.menu = 1
Menu.type.list = 2
Menu.type.slider = 3
Menu.type.string = 4
Menu.type.number = 5
Menu.type.action = 6

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

GameSpeed = { 
   type = Menu.type.list, caption = "Game speed",
   labels = { "boring", "normal", "fast", "crazy" },
   values = { 5.0, 6.5, 8.5, 12.0 },
   store = function (value) { speed = value; c_resetGame(); }
}

BotSkill = {
   type = Menu.type.list, caption = "Bot Skill",
   labels = { "dumb", "normal", "strong", "the MCP himself" },
   values = { 0, 1, 2, 3 },
   store = function (value) { ai_level = value; }
}

ArenaSize = {
   type = Menu.type.list, caption = "Arena Size",
   labels = { "tiny", "medium", "big", "vast", "extreme" },
   values = { 160, 240, 480, 720, 1200 },
   store = function (value) { grid_size = value; c_resetGame(); }
}
    
-- TODO: fill in the rest of the items

-- Menu entries
RootMenu.items = { GameMenu, VideoMenu, AudioMenu, Quit }

GameMenu.items = { 
   StartGame, GameRulesMenu, GameSettingsMenu, PlayerConfigMenu, KeyConfigMenu
}
  GameRulesMenu.items = { GameSpeed, BotSkill, ArenaSize, EraseDeadPlayers }
  GameSettingsMenu.items = { FastFinish, CameraMode, Viewports, ResetScores }
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
