-- global tables

settings = { }
artpacks = { }
tracks = { }

-- debugginb
settings.debug_output = 1

-- game
-- game rules
settings.speed = 8.5

settings.booster_on = 1
settings.booster_min = 1.0
settings.booster_max = 6.5
settings.booster_use = 1.0
settings.booster_decrease = 0.8
settings.booster_regenerate = 0.4

settings.wall_accel_on = 0
settings.wall_accel_limit = 20.0
settings.wall_accel_use = 1.0
settings.wall_accel_decrease = 0.8

settings.ai_level = 2
settings.grid_size = 720
settings.erase_crashed = 1
settings.map_ratio_w = 0.333
settings.map_ratio_h = 0.333

settings.ai_player1 = 0
settings.ai_player2 = 1
settings.ai_player3 = 1
settings.ai_player4 = 1
-- game settings
settings.fast_finish = 1
settings.camType = 1
settings.display_type = 3

-- video
settings.artpacks = { }
settings.current_artpack = "classic"
-- detail settings
settings.mipmap_filter = 3
settings.alpha_trails = 0
settings.show_glow = 1
settings.show_recognizer = 1
settings.show_impact = 1
settings.lod = 1

settings.show_fps = 0
settings.show_console = 1
settings.show_ai_status = 1
settings.show_scores = 1

-- screen options
settings.width = 800
settings.height = 600
settings.windowMode = 1

-- other settings (not available via menu)
settings.show_gl_logo = 1

settings.fov = 105
settings.znear = 0.5
settings.light_cycles = 1
settings.turn_cycle = 1
settings.line_spacing = 20
settings.softwareRendering = 0
settings.antialias_lines = 1
settings.bitdepth_32 = 0
settings.use_stencil = 0
settings.mouse_warp = 0
settings.model_backwards = 0
settings.screenSaver = 0

settings.joy_threshold = 0.10

-- audio
settings.tracks = { }
settings.current_track = "revenge_of_cats.it"
-- music & fx settings
settings.playEffects = 1
settings.playMusic = 1
settings.fxVolume = 0.2
settings.musicVolume = 0.5

-- keys

settings.keys = { 
   { -- player 1
      left = 97, -- lower case 'a'
      right = 115, -- lower case 's'
      glance_left = 113, -- lower case 'q'
      glance_right = 119, -- lower case 'w'
			boost = 101, -- lower case 'd'
   },
   { -- player 2
      left = 106, -- lower case 'j'
      right = 107, -- lower case 'k'
      glance_left = 117, -- lower case 'u'
      glance_right = 105, -- lower case 'i'
			boost = 108, -- lower case 'l'
   },
   { -- player 3
      left = 276, -- cursor left
      right = 275, -- cursor right
      glance_left = 127, -- 'del'
      glance_right = 279, -- 'end'
			boost = 274, -- cursor down
   },
   { -- player 4
      left = 260, -- keypad 4
      right = 262, -- keypad 6
      glance_left = 263, -- keypad 7
      glance_right = 265, -- keypad 9
			boost = 261, -- keypad 5
   }
}

-- these tables are not serialized to RCNAME (yet)
-- some values are set in artpack.lua
video = {} 
video.settings = {}
