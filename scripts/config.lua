-- global tables

settings = { }
artpacks = { }
tracks = { }

-- game
-- game rules
settings.speed = 8.5
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
-- texture settings
settings.show_floor_texture = 1
settings.show_wall = 1
settings.show_crash_texture = 1
settings.show_skybox = 1
settings.show_decals = 1
settings.use_mipmaps = 1
settings.mipmap_filter = 1
settings.stretch_textures = 1
-- detail settings
settings.alpha_trails = 0
settings.show_glow = 1
settings.show_model = 1
settings.show_recognizer = 1
settings.show_impact = 1
settings.lod = 1
settings.shadow_lod = 1
settings.show_fps = 0
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

-- audio
settings.tracks = { }
settings.current_track = "song_revenge_of_cats.it"
-- music & fx settings
settings.playEffects = 1
settings.playMusic = 1
settings.fxVolume = 0.3
settings.musicVolume = 0.3

-- keys

settings.keys = { 
   { -- player 1
      left = 97, -- lower case 'a'
      right = 115, -- lower case 's'
   },
   { -- player 2
      left = 106, -- lower case 'j'
      right = 107 -- lower case 'k'
   },
   { -- player 3
      left = 276, -- cursor settings.left
      right = 275, -- cursor settings.right
   },
   { -- player 4
      left = 260, -- keypad settings.left/keypad 4
      right = 262, -- keybad settings.right/keypad 6
   }
}

-- colors

-- this setting is usually overridden in artpack.ini
clear_color = { 0, 0, 0, 0 }

menu_item = { 1, 1, 1, 1 }
menu_item_active1 = { .5, 1, 0, 1 }
menu_item_active2 = { 1, 0, 0, 1 }

model_diffuse_0 = { 1.000, 0.550, 0.140, 1.000 }
model_diffuse_1 = { 0.750, 0.020, 0.020, 1.000 }
model_diffuse_2 = { 0.120, 0.520, 0.600, 1.000 }
model_diffuse_3 = { 0.800, 0.800, 0.800, 1.000 }

model_specular_0 = { 0.500, 0.500, 0.000, 1.000 }
model_specular_1 = { 0.750, 0.020, 0.020, 1.000 }
model_specular_2 = { 0.120, 0.520, 0.600, 1.000 }
model_specular_3 = { 1.000, 1.000, 1.000, 1.000 }

trail_diffuse_0 = { 1.000, 0.850, 0.140, 0.600 }
trail_diffuse_1 = { 0.750, 0.020, 0.020, 0.600 }
trail_diffuse_2 = { 0.120, 0.520, 0.600, 0.600 }
trail_diffuse_3 = { 0.700, 0.700, 0.700, 0.600 }

