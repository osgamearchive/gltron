MenuC = {
   type = {
      menu = 1,
      list = 2,
      slider = 3,
      key = 4,
      string = 5, -- not used yet
      number = 6, -- not used yet
      action = 7
   },
   keys = {
      player1 = {},
      player2 = {},
      player3 = {},
      player4 = {}
   }
}

Menu = { 
   -- menu after startup
   current = "RootMenu",
   active = 1,
   -- Menu definitions
   RootMenu = { type = MenuC.type.menu, caption = "" },
   GameMenu = { type = MenuC.type.menu, caption = "Game" },
   GameRulesMenu = { type = MenuC.type.menu, caption = "Game Rules" },
   GameSettingsMenu = { type = MenuC.type.menu, caption = "Play Settings" },
   PlayerConfigMenu = { type = MenuC.type.menu, caption = "Configure Players" },
   KeyConfigMenu = { type = MenuC.type.menu, caption = "Configure Keys" },
   JoyConfigMenu = { type = MenuC.type.menu, caption = "Configure Joystick" },
   Player1_KeyMenu = { type = MenuC.type.menu, caption = "Player 1" },
   Player2_KeyMenu = { type = MenuC.type.menu, caption = "Player 2" },
   Player3_KeyMenu = { type = MenuC.type.menu, caption = "Player 3" },
   Player4_KeyMenu = { type = MenuC.type.menu, caption = "Player 4" },
   VideoMenu = { type = MenuC.type.menu, caption = "Video" },
   TextureMenu = { type = MenuC.type.menu, caption = "Texture Options" },
   DetailsMenu = { type = MenuC.type.menu, caption = "Detail Options" },
   ScreenMenu = { type = MenuC.type.menu, caption = "Screen Options" },
   AudioMenu = { type = MenuC.type.menu, caption = "Audio" },

   -- Item definitions
   -- RootMenu
   Quit = { 
      type = MenuC.type.action, caption = "Quit",
      action = function() c_quitGame(); end
   },

   -- Game
   StartGame = { 
      type = MenuC.type.action, caption = "Start Game",
      action = function() Menu.current = "RootMenu"; c_startGame(); end
   },
   ResetScores = { 
      type = MenuC.type.action, caption = "Reset Scores",
      action = function() c_resetScores(); end
   },

   -- Game Rules
   GameSpeed = { 
      type = MenuC.type.list, caption = "Game speed",
      labels = { "boring", "normal", "fast", "crazy" },
      values = { 5.0, 6.5, 8.5, 12.0 },
      read = function() return settings.speed; end, 
      store = function (value) settings.speed = value; c_resetGame(); end
   },
   BotSkill = {
      type = MenuC.type.list, caption = "Bot Skill",
      labels = { "dumb", "normal", "strong", "the MCP himself" },
      values = { 0, 1, 2, 3 },
      read = function() return settings.ai_level; end,
      store = function (value) settings.ai_level = value; end
   },
   ArenaSize = {
      type = MenuC.type.list, caption = "Arena Size",
      labels = { "tiny", "medium", "big", "vast", "extreme" },
      values = { 160, 240, 480, 720, 1200 },
      read = function() return settings.grid_size; end,
      store = function (value) settings.grid_size = value; c_resetGame(); end
   },
   EraseDeadPlayers = {    
      type = MenuC.type.list, caption = "Erase dead players",
      labels = { "off", "on" },
      values = { 0, 1 },
      read = function() return settings.erase_crashed; end,
      store = function(value) settings.erase_crashed = value; c_resetGame(); end
   },

   -- GameSettings
   FastFinish = {    
      type = MenuC.type.list, caption = "Fast finish",
      labels = { "off", "on" },
      values = { 0, 1 },
      read = function() return settings.fast_finish; end,
      store = function(value) settings.fast_finish = value; end
   },
   CameraMode = {    
      type = MenuC.type.list, caption = "Camera mode",
      labels = { "circling", "behind", "cockpit", "mouse" },
      values = { 0, 1, 2, 3 },
      read = function() return settings.camType; end,
      store = function(value) settings.camType = value; c_resetCamera(); end
   },
   Viewports = {    
      type = MenuC.type.list, caption = "Viewports",
      labels = { "single", "split", "4 player", "auto" },
      values = { 0, 1, 2, 3 },
      read = function() return settings.display_type; end,
      store = function(value) settings.display_type = value; end
   },
	 Map = {
			type = MenuC.type.list, caption = "2D Map",
			labels = { "none", "small", "medium", "huge" },
			values = { 0, 0.333, 0.5, 0.9 },
			read = function() return settings.map_ratio_w; end,
			store = function(value) 
								 settings.map_ratio_w = value;
								 settings.map_ratio_h = value;
							end
   },

   -- Player
   Player1 = {    
      type = MenuC.type.list, caption = "Player 1", 
      labels = { "Human", "Computer", "None" },
      values = { 0, 1, 2 },
      read = function() return settings.ai_player1; end,
      store = function(value) settings.ai_player1 = value; c_resetGame(); end
   },

   Player2 = {    
      type = MenuC.type.list, caption = "Player 2", 
      labels = { "Human", "Computer", "None" },
      values = { 0, 1, 2 },
      read = function() return settings.ai_player2; end,
      store = function(value) settings.ai_player2 = value; c_resetGame(); end
   },

   Player3 = {    
      type = MenuC.type.list, caption = "Player 3", 
      labels = { "Human", "Computer", "None" },
      values = { 0, 1, 2 },
      read = function() return settings.ai_player3; end,
      store = function(value) settings.ai_player3 = value; c_resetGame(); end
   },

   Player4 = {    
      type = MenuC.type.list, caption = "Player 4", 
      labels = { "Human", "Computer", "None" },
      values = { 0, 1, 2 },
      read = function() return settings.ai_player4; end,
      store = function(value) settings.ai_player4 = value; c_resetGame(); end
   },

   -- PlayerX_Key
   Player1_Left = { 
      type = MenuC.type.key, caption = "Player 1 Left",
      player = 1, event = "left"
	 },
   Player1_Right = { 
      type = MenuC.type.key, caption = "Player 1 Right",
      player = 1, event = "right"
   },
   Player2_Left = { 
      type = MenuC.type.key, caption = "Player 2 Left",
      player = 2, event = "left"
   },
   Player2_Right = { 
      type = MenuC.type.key, caption = "Player 2 Right",
      player = 2, event = "right"
   },
   Player3_Left = { 
      type = MenuC.type.key, caption = "Player 3 Left",
      player = 3, event = "left"
   },
   Player3_Right = { 
      type = MenuC.type.key, caption = "Player 3 Right",
      player = 3, event = "right"
   },
   Player4_Left = { 
      type = MenuC.type.key, caption = "Player 4 Left",
      player = 4, event = "left"
   },
   Player4_Right = { 
      type = MenuC.type.key, caption = "Player 4 Right",
      player = 4, event = "right"
   },

   -- Joystick
   Joy_Threshold = {
      type = MenuC.type.slider, caption = "Threshold",
      right = JoyThresholdUp,
      left = JoyThresholdDown,
      read = function() return format("%.0f%%", settings.joy_threshold * 100); end
   },

   -- TODO: fill in the rest of the items

   -- Video

   -- Texture
   Artpack = {
      type = MenuC.type.slider, caption = "Artpack",
      right = nextArtpack,
      left = previousArtpack,
      action = nextArtpack,
      read = function() return settings.current_artpack; end
   },
   Floor = {
      type = MenuC.type.list, caption = "Floor texture",
      labels = { "off", "on" },
      values = { 0, 1 },
      read = function() return settings.show_floor_texture; end,
      store = function(value) settings.show_floor_texture = value; end
   },
   Wall = {
      type = MenuC.type.list, caption = "Wall texture",
      labels = { "off", "on" },
      values = { 0, 1 },
      read = function() return settings.show_wall; end,
      store = function(value) settings.show_wall = value; end
   },
   Skybox = {
      type = MenuC.type.list, caption = "Skybox texture",
      labels = { "off", "on" },
      values = { 0, 1 },
      read = function() return settings.show_skybox; end,
      store = function(value) settings.show_skybox = value; end
   },
   Decals = {
      type = MenuC.type.list, caption = "Trail decals",
      labels = { "off", "on" },
      values = { 0, 1 },
      read = function() return settings.show_decals; end,
      store = function(value) settings.show_decals = value; end
   },
   Mipmaps = {
      type = MenuC.type.list, caption = "Use mipmaps",
      labels = { "off", "on" },
      values = { 0, 1 },
      read = function() return settings.use_mipmaps; end,
      store = function(value) settings.use_mipmaps = value; end
   },
   Filtering = {
      type = MenuC.type.list, caption = "Mipmap filter",
      labels = { "bilinear", "trilinear" },
      values = { 0, 1 },
      read = function() return settings.mipmap_filter; end,
      store = function(value) settings.mipmap_filter = value; end
   },
   WallScaling = {
      type = MenuC.type.list, caption = "Wall Scaling",
      labels = { "off", "on" },
      values = { 0, 1 },
      read = function() return settings.stretch_textures; end,
      store = function(value) settings.stretch_textures = value; end
   },

   -- Detail
   AlphaTrails = {
      type = MenuC.type.list, caption = "Transparent trails",
      labels = { "off", "on" },
      values = { 0, 1 }, 
      read = function() return settings.alpha_trails; end,     
      store = function(value) settings.alpha_trails = value; end
   },
   Halos = {
      type = MenuC.type.list, caption = "Halos",
      labels = { "off", "on" },
      values = { 0, 1 },
      read = function() return settings.show_glow; end,
      store = function(value) settings.show_glow = value; end
   },
   Lightcycles = {
      type = MenuC.type.list, caption = "Lightcycles",
      labels = { "off", "on" },
      values = { 0, 1 },
      read = function() return settings.show_model; end,
      store = function(value) settings.show_model = value; end
   },
   Recognizer = {
      type = MenuC.type.list, caption = "Recognizers",
      labels = { "off", "on" },
      values = { 0, 1 },
      read = function() return settings.show_recognizer; end,
      store = function(value) settings.show_recognizer = value; end
   },
   ImpactAnimation = {
      type = MenuC.type.list, caption = "Impact Animation",
      labels = { "off", "on" },
      values = { 0, 1 },
      read = function() return settings.show_impact; end,
      store = function(value) settings.show_impact = value; end
   }, 
   CycleLod = {
      type = MenuC.type.list, caption = "Cycle LOD",
      labels = { "ugly", "low", "high", "ultra-high" },
      values = { 3, 2, 1, 0 },
      read = function() return settings.lod; end,
      store = function(value) settings.lod = value; end
   },
   ShadowLod = {
      type = MenuC.type.list, caption = "Shadow LOD",
      labels = { "normal", "high" },
      values = { 0, 1 },
      read = function() return settings.shadow_lod; end,
      store = function(value) settings.shadow_lod = value; end
   },
   ShadowStyle = {
      type = MenuC.type.list, caption = "Shadow Type",
      labels = { "stencil", "normal" },
      values = { 1, 0 },
      read = function() return settings.use_stencil; end,
      store = function(value) settings.use_stencil = value; c_video_restart(); end
   },
   FPS_Counter = {
      type = MenuC.type.list, caption = "FPS counter",
      labels = { "off", "on" },
      values = { 0, 1 },
      read = function() return settings.show_fps; end,
      store = function(value) settings.show_fps = value; end
   },
   AI_Status = {
      type = MenuC.type.list, caption = "AI status",
      labels = { "off", "on" },
      values = { 0, 1 },
      read = function() return settings.show_ai_status; end,
      store = function(value) settings.show_ai_status = value; end
   },
   Scores = {
      type = MenuC.type.list, caption = "Scores",
      labels = { "off", "on" },
      values = { 0, 1 },
      read = function() return settings.show_scores; end,
      store = function(value) settings.show_scores = value; end
   },

   -- Screen
   _320x240 = {
      type = MenuC.type.action, caption = "320 x 240",
      action =  function() settings.width = 320; settings.height = 240; c_video_restart(); end
   },
   _512x384 = {
      type = MenuC.type.action, caption = "512 x 384",
      action =  function() settings.width = 512; settings.height = 384; c_video_restart(); end
   },
   _640x480 = {
      type = MenuC.type.action, caption = "640 x 480",
      action =  function() settings.width = 640; settings.height = 480; c_video_restart(); end
   },
   _800x600 = {
      type = MenuC.type.action, caption = "800 x 600",
      action =  function() settings.width = 800; settings.height = 600; c_video_restart(); end
   },
   _1024x768 = {
      type = MenuC.type.action, caption = "1024 x 768",
      action =  function() settings.width = 1024; settings.height = 768; c_video_restart(); end
   },
   _1280x1024 = {
      type = MenuC.type.action, caption = "1280 x 1024",
      action =  function() settings.width = 1280; settings.height = 1024; c_video_restart(); end
   },
   _1600x1200 = {
      type = MenuC.type.action, caption = "1600 x 1200",
      action =  function() settings.width = 1600; settings.height = 1200; c_video_restart(); end
   },
   WindowMode = {
      type = MenuC.type.list, caption = "Windowed",
      labels = { "off", "on" },
      values = { 0, 1 },
      read = function() return settings.windowMode; end,
      store = function(value) settings.windowMode = value; c_video_restart(); end
   },

	 TimeDemo = {
			type = MenuC.type.action, caption = "Time Demo",
			action = function() c_timedemo(); end
	 },

   -- Audio
   Music = {
      type = MenuC.type.list, caption = "Music",
      labels = { "off", "on" },
      read = function() return settings.playMusic; end,
      values = { 0, 1 },
      store = function(value) settings.playMusic = value; c_update_settings_cache() ; end
   },
   FX = {
      type = MenuC.type.list, caption = "Sound FX",
      labels = { "off", "on" },
      values = { 0, 1 },
      read = function() return settings.playEffects; end,
      store = function(value) settings.playEffects = value; end
   },
   Music_Volume = {
      type = MenuC.type.slider, caption = "Music Volume",
      right = function() MusicVolumeUp(); end,
      left = function() MusicVolumeDown(); end,
      read = function() return format("%.0f%%", settings.musicVolume * 100); end
   },
   FX_Volume = {
      type = MenuC.type.slider, caption = "FX Volume",
      right = FXVolumeUp,
      left = FXVolumeDown,
      read = function() return format("%.0f%%", settings.fxVolume * 100); end
   },
   Song = {
      type = MenuC.type.slider, caption = "Song",
      right = nextTrack,
      left = previousTrack,
      action = nextTrack,
      read = function()
								_,_,name = strfind(settings.current_track, "(.*)%..+")
								if name then
									 return name
								else
									 return settings.current_track
								end
						 end
   }
}

Menu.SetParent = function ( menu )
										-- script_print("processing menu '" .. menu .. "'")
										local _,entry
										for _,entry in Menu[menu].items do
											 Menu[entry].parent = menu
											 -- script_print("processing item '" .. entry .. "'")
											 if Menu[entry].type == MenuC.type.menu then
													Menu.SetParent( entry )
											 end
										end
								 end

Menu.SetNames = function ()
									 local name,v
									 for name,v in Menu do
											if type(v) == "table" then
												 v.name = name
											end
									 end
								end

Menu.GotoParent = function ()
										 Menu.current = Menu[Menu.current].parent
										 Menu.active = 1
									end

Menu.Action = function ()
								 local menu = Menu[Menu.current].items[Menu.active]
								 local type = Menu[ menu  ].type
								 script_print("calling action of '" .. menu .. "', type " .. type )
								 MenuAction[ type ]( menu )
							end

Menu.Left = function ()
							 local menu = Menu[Menu.current].items[Menu.active]
							 local type = Menu[ menu  ].type
							 if type == MenuC.type.slider then
									script_print("calling left of '" .. menu .. "'")
									Menu[ menu ].left()
							 else
									-- script_print("calling action of '" .. menu .. "', type " .. type )
									-- MenuAction[ type ]( menu )      
							 end
						end

Menu.Right = function ()
								local menu = Menu[Menu.current].items[Menu.active]
								local type = Menu[ menu  ].type
								if type == MenuC.type.slider then
									 script_print("calling right of '" .. menu .. "'")
									 Menu[ menu ].right()
								else
									 -- script_print("calling action of '" .. menu .. "', type " .. type )
									 -- MenuAction[ type ]( menu )
								end
						 end


Menu.Next = function ()
							 if Menu.active < getn(Menu[Menu.current].items) then
									Menu.active = Menu.active + 1
							 else
									Menu.active = 1
							 end
						end

Menu.Previous = function ()
									 if Menu.active > 1 then 
											Menu.active = Menu.active - 1
									 else
											Menu.active = getn(Menu[Menu.current].items)
									 end
								end

-- Menu entries
Menu.RootMenu.items = { "GameMenu", "VideoMenu", "AudioMenu", "Quit" }

Menu.GameMenu.items = { 
   "StartGame", "ResetScores", "GameRulesMenu", 
   "GameSettingsMenu", "PlayerConfigMenu", "KeyConfigMenu", "JoyConfigMenu"
}

Menu.GameRulesMenu.items = { 
   "GameSpeed", "BotSkill", "ArenaSize", "EraseDeadPlayers" 
}

Menu.GameSettingsMenu.items = { "FastFinish", "CameraMode", "Viewports", "Map" }
Menu.PlayerConfigMenu.items = { "Player1", "Player2", "Player3", "Player4" }
Menu.KeyConfigMenu.items = { 
   "Player1_KeyMenu", "Player2_KeyMenu", "Player3_KeyMenu", "Player4_KeyMenu"
}
Menu.JoyConfigMenu.items = { "Joy_Threshold" }
Menu.Player1_KeyMenu.items = { "Player1_Left", "Player1_Right" }
Menu.Player2_KeyMenu.items = { "Player2_Left", "Player2_Right" }
Menu.Player3_KeyMenu.items = { "Player3_Left", "Player3_Right" }
Menu.Player4_KeyMenu.items = { "Player4_Left", "Player4_Right" }

Menu.VideoMenu.items = { 
	 "TextureMenu", "DetailsMenu", 
	 "ScreenMenu", "TimeDemo" 
}

Menu.TextureMenu.items = { 
   "Artpack", "Floor", "Wall", "Skybox", 
   "Decals", "Mipmaps", "Filtering", "WallScaling"
}
Menu.DetailsMenu.items = {
   "AlphaTrails", "Halos", "Lightcycles", "Recognizer", "ImpactAnimation", 
   "CycleLod", "ShadowLod", "ShadowStyle", "FPS_Counter", "AI_Status", "Scores"
}
Menu.ScreenMenu.items = {
   "_320x240", "_512x384", "_640x480", "_800x600", "_1024x768", "_1280x1024",
   "_1600x1200", "WindowMode"
}

Menu.AudioMenu.items = { "Music", "FX", "Music_Volume", "FX_Volume", "Song" }

-- initialization code
Menu.SetNames()
Menu.SetParent( "RootMenu" )



