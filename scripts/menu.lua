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
	 GameType = {
			type = MenuC.type.list, caption = "Game type",
			labels = { "classic", "booster", "wall accell.", "both" },
			values = { 0, 1, 2, 3 },
			read = 
				 function()
						value = 0;
						if(settings.booster_on == 1) then value = value + 1; end;
						if(settings.wall_accel_on == 1) then value = value + 2; end;
						return value;
					end,
			store = 
				 function (value)
						if(value >= 2) 
						then settings.wall_accel_on = 1;
						else settings.wall_accel_on = 0;
						end;
						if(value == 1 or value == 3) 
						then settings.booster_on = 1; 
						else settings.booster_on = 0;
						end;
						script_print("booster: " .. settings.booster_on);
						script_print("wall accel: " .. settings.wall_accel_on);
				 end
	 },
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
      store = function (value) settings.grid_size = value; c_reloadLevel(); end
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
      labels = { "circling", "behind", "cockpit", "mouse", "offset" },
      values = { 0, 1, 2, 3, 4 },
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
      type = MenuC.type.key, caption = "Turn Left",
      player = 1, event = "left"
	 },
   Player1_Right = { 
      type = MenuC.type.key, caption = "Turn Right",
      player = 1, event = "right"
   },
   Player1_GlanceLeft = { 
      type = MenuC.type.key, caption = "Glance Left",
      player = 1, event = "glance_left"
	 },
   Player1_GlanceRight = { 
      type = MenuC.type.key, caption = "Glance Right",
      player = 1, event = "glance_right"
   },
   Player1_Booster = { 
      type = MenuC.type.key, caption = "Booster",
      player = 1, event = "boost"
   },
   Player2_Left = { 
      type = MenuC.type.key, caption = "Turn Left",
      player = 2, event = "left"
	 },
   Player2_Right = { 
      type = MenuC.type.key, caption = "Turn Right",
      player = 2, event = "right"
   },
   Player2_GlanceLeft = { 
      type = MenuC.type.key, caption = "Glance Left",
      player = 2, event = "glance_left"
	 },
   Player2_GlanceRight = { 
      type = MenuC.type.key, caption = "Glance Right",
      player = 2, event = "glance_right"
   },
   Player2_Booster = { 
      type = MenuC.type.key, caption = "Booster",
      player = 2, event = "boost"
   },
   Player3_Left = { 
      type = MenuC.type.key, caption = "Turn Left",
      player = 3, event = "left"
	 },
   Player3_Right = { 
      type = MenuC.type.key, caption = "Turn Right",
      player = 3, event = "right"
   },
   Player3_GlanceLeft = { 
      type = MenuC.type.key, caption = "Glance Left",
      player = 3, event = "glance_left"
	 },
   Player3_GlanceRight = { 
      type = MenuC.type.key, caption = "Glance Right",
      player = 3, event = "glance_right"
   },
   Player3_Booster = { 
      type = MenuC.type.key, caption = "Booster",
      player = 3, event = "boost"
   },
   Player4_Left = { 
      type = MenuC.type.key, caption = "Turn Left",
      player = 4, event = "left"
	 },
   Player4_Right = { 
      type = MenuC.type.key, caption = "Turn Right",
      player = 4, event = "right"
   },
   Player4_GlanceLeft = { 
      type = MenuC.type.key, caption = "Glance Left",
      player = 4, event = "glance_left"
	 },
   Player4_GlanceRight = { 
      type = MenuC.type.key, caption = "Glance Right",
      player = 4, event = "glance_right"
   },
   Player4_Booster = { 
      type = MenuC.type.key, caption = "Booster",
      player = 4, event = "boost"
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
	Level = {
		type = MenuC.type.slider, caption = "Level",
		init = function() tmp.current_level = settings.current_level; end,
		right = nextLevel,
		left = previousLevel,
		action = nextLevel,
		read = function()
			_,_,name = strfind(tmp.current_level, "(.*)%..+")
			if name then
				return name
			else
				return tmp.current_level
			end
		end
	},

	Artpack = {
		type = MenuC.type.slider, caption = "Artpack",
		init = function() tmp.current_artpack = settings.current_artpack; end,
		right = nextArtpack,
		left = previousArtpack,
		action = nextArtpack,
		read = function() return tmp.current_artpack; end
	},
	 -- Details
   Filtering = {
      type = MenuC.type.list, caption = "Mipmap filter",
      labels = { "nearest", "linear", "mipmap", "trilinear" },
      values = { 0, 1, 2, 3 },
      read = function() return settings.mipmap_filter; end,
      store = function(value) 
								 settings.mipmap_filter = value
								 c_reloadArtpack()
							end
   },
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
	 Reflections = {
			type = MenuC.type.list, caption = "Reflections",
			labels = { 
				 "off", "low", "medium", "high", "very high", "perfect mirror",
				 "ask artpack"
			},
			values = { 0.0, 0.1, 0.3, 0.6, 0.8, 1.0, -1 },
			read = function() return settings.reflection; end,
	    store = function(value) settings.reflection = value; end
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
   Lod = {
      type = MenuC.type.list, caption = "Cycle Detail",
      labels = { "ugly", "low", "high", "ultra-high" },
      values = { 3, 2, 1, 0 },
      read = function() return settings.lod; end,
      store = function(value) settings.lod = value; end
   },
   -- Status Display
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
	Resolution = {
		type = MenuC.type.list, caption = "Resolution",
		init = function(menu)
			write("[resolution] running init\n")
			-- check if matching resolution is found
			tmp.resolution = 0
			local i
			local n = getn(Menu[menu].widths)
			for i = 1,n do
				if (settings.width == Menu[menu].widths[i]) and (settings.height == Menu[menu].heights[i]) then
					tmp.resolution = i
				end
			end
			if(tmp.resolution == 0) then
				Menu[menu].widths[n] = settings.width
				Menu[menu].heights[n] = settings.height
				-- Menu[menu].labels[n] = format(Menu[menu].labels[n], settings.width, settings.height)
				tmp.resolution = n
			end
			write(format("[resolution] resolution: %d\n", tmp.resolution))
		end,	
			
		labels = {
			"320 x 240", "512 x 384", "640 x 400",
			"640 x 480", "800 x 600", "1024 x 768",
			"1280 x 960", "1280 x 1024", "1400 x 1050",
			"1600 x 1024", "1600 x 1200", "1680 x 1050",
			"1920 x 1200", "custom" -- "custom(%d, %d)"
			},
		values = {
			1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14
		},
		widths = {
			320, 512, 640, 640, 800, 1024,
			1280, 1280, 1400, 1600, 1600, 1680,
			1920, 0
		},
		heights = {
			240, 384, 400, 480, 600, 768,
			960, 1024, 1050, 1024, 1200, 1050,
			1200, 0
		},
		read = function() return tmp.resolution; end,
		store = function(value) tmp.resolution = value; end
	},	

	Apply = {
		type = MenuC.type.action, caption = "Apply Changes",
		action = function()
			local restart = 0
			local reload_art = 0
			local reload_level = 0
			
			if settings.windowMode ~= tmp.windowMode then restart = 1; end
			if settings.width ~= Menu.Resolution.widths[tmp.resolution] then restart = 1; end
			if settings.height ~= Menu.Resolution.heights[tmp.resolution] then restart = 1; end
			settings.width = Menu.Resolution.widths[tmp.resolution]
			settings.height = Menu.Resolution.heights[tmp.resolution]
			settings.windowMode = tmp.windowMode
			
			if settings.current_level ~= tmp.current_level then reload_level = 1; end
			settings.current_level = tmp.current_level
			
			if settings.current_artpack ~= tmp.current_artpack then reload_art = 1; end
			settings.current_artpack = tmp.current_artpack
			
			if restart == 1 then
				c_video_restart()
			else 
				if reload_art == 1 then
					c_setArtPath()
					c_reloadArtpack()
				end
				if reload_level == 1 then
					c_reloadLevel()
				end
			end
		end
	},
		
	WindowMode = {
		type = MenuC.type.list, caption = "Windowed",
		init = function()
			tmp.windowMode = settings.windowMode
		end,
		labels = { "off", "on" },
		values = { 0, 1 },
		read = function() return tmp.windowMode; end,
		store = function(value) tmp.windowMode = value; end
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
      right = function() FXVolumeUp(); end,
      left = function() FXVolumeDown(); end,
      read = function() return format("%.0f%%", settings.fxVolume * 100); end
   },
   Song = {
      type = MenuC.type.slider, caption = "Song",
      right = function() nextTrack(); end,
      left = function() previousTrack(); end,
      action = function() nextTrack(); end,
      read = function()
								_,_,name = strfind(settings.current_track, "(.*)%..+")
								if name then
									 return name
								else
									 return settings.current_track
								end
						 end
   },
   LoopMusic = {
      type = MenuC.type.list, caption = "Loop Music",
      labels = { "off", "on" },
      values = { 0, 1 },
      read = function() return settings.loopMusic; end,
      store = function(value) settings.loopMusic = value; end
   },
}
-- Menu entries
Menu.RootMenu.items = { "GameMenu", "VideoMenu", "AudioMenu", "Quit" }

Menu.GameMenu.items = { 
   "StartGame", "ResetScores", "GameRulesMenu", 
   "GameSettingsMenu", "PlayerConfigMenu", "KeyConfigMenu", "JoyConfigMenu"
}

Menu.GameRulesMenu.items = { 
   "GameType", "GameSpeed", "BotSkill", "ArenaSize", "EraseDeadPlayers" 
}

Menu.GameSettingsMenu.items = { "FastFinish", "CameraMode", "Viewports", "Map" }
Menu.PlayerConfigMenu.items = { "Player1", "Player2", "Player3", "Player4" }
Menu.KeyConfigMenu.items = { 
   "Player1_KeyMenu", "Player2_KeyMenu", "Player3_KeyMenu", "Player4_KeyMenu"
}
Menu.JoyConfigMenu.items = { "Joy_Threshold" }
Menu.Player1_KeyMenu.items = { 
	"Player1_Left", "Player1_Right",
	"Player1_GlanceLeft", "Player1_GlanceRight",
	"Player1_Booster",
}
Menu.Player2_KeyMenu.items = { 
	"Player2_Left", "Player2_Right",
	"Player2_GlanceLeft", "Player2_GlanceRight",
	"Player2_Booster",
}
Menu.Player3_KeyMenu.items = { 
	"Player3_Left", "Player3_Right",
	"Player3_GlanceLeft", "Player3_GlanceRight",
	"Player3_Booster",
}
Menu.Player4_KeyMenu.items = { 
	"Player4_Left", "Player4_Right",
	"Player4_GlanceLeft", "Player4_GlanceRight",
	"Player4_Booster",
}

Menu.VideoMenu.items = {
	 "Level", "Artpack", 
	 "Resolution", "WindowMode", "Apply",
	 "DetailsMenu"
	 --, "TimeDemo" 
}

Menu.DetailsMenu.items = {
   "Filtering", "AlphaTrails", "Halos", "Reflections",
    -- "Lightcycles",
    "Recognizer", "Lod", "FPS_Counter", "AI_Status", "Scores"
}

Menu.AudioMenu.items = { 
	 "Music", "FX", "Music_Volume", "FX_Volume", "Song", "LoopMusic"
}