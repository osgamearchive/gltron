Menu.type.menu = 1
Menu.type.list = 2
Menu.type.slider = 3
Menu.type.string = 4
Menu.type.number = 5
Menu.type.action = 6

-- Menu definitions
RootMenu = { type = Menu.type.menu }
GameMenu = { type = Menu.type.menu }
GameRulesMenu = { type = Menu.type.action }

-- TODO: fill in the rest of the menus

-- Item definitions
StartGame = { type = Menu.type.action }
StartGame.action = function() c_startGame(); end

-- TODO: fill in the rest of the items

-- Menu entries
RootMenu.items = { GameMenu, VideoMenu, AudioMenu, Quit }

GameMenu.items = { 
   StartGame, GameRulesMenu, GameSettingsMenu, PlayerConfig, KeyConfigMenu
}
GameRulesMenu.items = { GameSpeed, BotSkill, ArenaSize, EraseDeadPlayers }
GameSettingsMenu.items = { FastFinish, CameraMode, Viewports, ResetScores }
PlayerConfig.items = { Player1, Player2, Player3, Player4 }
KeyConfig.items = { 
   Player1_KeyMenu, Player2_KeyMenu, Player3_KeyMenu,Player4_KeyMenu
}
Player1_KeyMenu = { Player1_Left, Player1_Right }
Player2_KeyMenu = { Player2_Left, Player2_Right }
Player3_KeyMenu = { Player3_Left, Player3_Right }
Player4_KeyMenu = { Player4_Left, Player4_Right }

VideoMenu.items = { TextureMenu, DetailsMenu, ScreenMenu }
TextureMenu.items = { 
   Artpack, Floor, Wall, Crash, Skybox, Decals, Mipmaps, Filtering, WallScaling
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
