-- OS X & PC Config.lua

-- global tables
tmp = {}

settings = { }
tracks = { }

app_version = 0.7191
version = app_version

-- debugging
settings.debug_output = 1

-- game
-- game rules
settings.speed = 8.5                      -- lightcycle speed
settings.energy = 4.8					  -- lightcycle energy
settings.energy_increase = 0.4            -- energy increase per second

settings.booster_on = 1                   -- booster option (0=off,1=on)
settings.booster_min = 1.0                -- min. booster value so it can be activated
-- settings.booster_max = 6.5                -- fully charged booster
settings.booster_use = 1.5                -- how much speed to add per second
settings.booster_speed_factor = 1.1
-- speed_factor: multiply player speed upon usage (per second)
settings.booster_speed_increase = 1.2
-- speed_increase: increase speed by constant value per second
settings.booster_speed_decrease = 0.8
-- speed decrease per second after booster ran out
-- settings.booster_regenerate = 0.4         -- regeneration of booster when not in use

settings.wall_accel_on = 0
settings.wall_accel_max = 20.0
settings.wall_accel_use = 1.0
settings.wall_accel_decrease = 0.8

settings.wall_buster_on = 1
-- wall_buster_on:
--   1: wall buster enabled
--   0: wall buster disabled

settings.wall_buster_use = 4.8
-- amount of energy consumed per second of wall_buster use
settings.wall_buster_min = 2.4
-- amount of energy needed to trigger wall_buster usage

-- settings.wall_buster_max = 1.0
-- settings.wall_buster_regenerate = 0.2

settings.players = 1
settings.ai_opponents = 3
settings.ai_level = 2
settings.current_level = "square.lua"
-- settings.current_level = "movie.lua"
settings.erase_crashed = 1
settings.map_scale = 1

-- game settings
settings.fast_finish = 1
settings.camType = 1
settings.display_type = 0

-- video
settings.artpacks = { }
settings.current_artpack = "classic"
-- detail settings
settings.mipmap_filter = 3
settings.alpha_trails = 0
settings.show_glow = 1
settings.reflection = -1
settings.show_recognizer = 1
settings.show_impact = 1
settings.lod = 1
settings.cycle_sharp_edges = 0
-- hud settings
settings.show_fps = 0
settings.show_speed = 1
settings.show_wall_buster = 0
settings.show_console = 0
settings.show_ai_status = 0
settings.show_scores = 0
settings.show_2d = 1

-- screen options
settings.width = 1280
settings.height = 720
settings.resolution = 3 -- 1280x720
settings.windowMode = 1

-- other settings (not available via menu)
settings.show_gl_logo = 1

settings.fov = 90
settings.znear = 0.5
settings.light_cycles = 1
settings.turn_cycle = 1
settings.line_spacing = 20
settings.softwareRendering = 0
settings.antialias_lines = 1
settings.bitdepth_32 = 1
settings.use_stencil = 1
settings.mouse_warp = 0
settings.model_backwards = 0
settings.screenSaver = 0

settings.shadow_volumes_walls = 0
settings.shadow_volumes_cycle = 1
settings.shadow_projective_cycle_on_floor = 1
settings.shadow_projective_trails_on_floor = 1
settings.shadow_projective_recognizer_on_floor = 1

settings.arena_outlines = 0

settings.joy_threshold = 0.10

-- audio
-- settings.tracks = { }
settings.current_track = "revenge_of_cats.it"
-- music & fx settings
settings.loopMusic = 1
-- TODO: debug setting 
settings.playEffects = 1
settings.playMusic = 1
-- settings.playEffects = 1
-- settings.playMusic = 1
settings.fxVolume = 0.2
settings.musicVolume = 0.5

-- input

settings.mouse_invert_x = 0
settings.mouse_invert_y = 0

-- #define CAM_CIRCLE_DIST 17
-- #define CAM_CIRCLE_Z 8.0f
--
-- #define CAM_FOLLOW_DIST 18
-- #define CAM_FOLLOW_Z 6.0f
-- #define CAM_FOLLOW_SPEED 0.05f
-- #define CAM_FOLLOW_SPEED_FACTOR 1.0f / 82.0f
-- #define CAM_SPEED 0.000349f
--
-- #define CAM_COCKPIT_Z 4
--
-- #define CAM_R_MIN 2.0
-- #define CAM_R_MAX 100
-- #define CAM_CHI_MIN PI / 6
-- #define CAM_CHI_MAX PI / 2 - PI / 6
--
-- #define CAM_DR 6.4f

PI = 3.141592654

settings.Camera = {
    CIRCLE_DIST = 17,
    Circling = {
        r = 17, phi = - PI / 8, chi = 0, phi_offset = 0,
        free_r = 1, free_phi = 0, free_chi = 1,
        interpolated_cam = 0, interpolated_target = 0, coupled = 0
    },
    Follow = {
        r = 18, phi = - PI / 8, chi = PI / 3, phi_offset = 0,
        free_r = 1, free_phi = 1, free_chi = 1,
        interpolated_cam = 1, interpolated_target = 0, coupled = 1
    },
    Cockpit = {
        r = 4, phi = PI, chi = 0, phi_offset = 0,
        free_r = 0, free_phi = 1, free_chi = 0,
        interpolated_cam = 0, interpolated_target = 1, coupled = 1
    },
    Manual = {
        r = 17, phi = - PI / 8, chi = 0, phi_offset = 0,
        free_r = 1, free_phi = 1, free_chi = 1,
        interpolated_cam = 0, interpolated_target = 0, coupled = 0
    },
    Offset = {
        r = 17, phi = - PI / 8, chi = PI / 3, phi_offset = 0,
        free_r = 1, free_phi = 1, free_chi = 1,
        interpolated_cam = 0, interpolated_target = 0, coupled = 0
    }
}
-- camera
settings.clamp_cam_r_min = 6
settings.clamp_cam_r_max =  45
settings.clamp_cam_chi_min =  PI / 8
settings.clamp_cam_chi_max =  3 * PI / 8 -- PI/3 in the C code

settings.camIsLocked = 0

settings.keys = {
    { -- player 1
        left = "A",
        right = "S",
        glance_left = "Q",
        glance_right = "W",
        boost = "E",
        bust = "D"
    },
    { -- player 2
       left = "J",
       right = "K",
       glance_left = "U",
       glance_right = "I",
       boost = "O",
       bust = "L"
   },
   { -- player 3
       left = "Left",
       right = "Right",
       glance_left = "Delete",
       glance_right = "End",
       boost = "Up",
       bust = "Down"
   },
   { -- player 4
       left = "Keypad 4",
       right = "Keypad 5",
       glance_left = "Keypad 7",
       glance_right = "Keypad 8",
       boost = "Keypad 9",
       bust = "Keypad 6"
   }
}

settings.wireframe = 0
-- these tables are not serialized to RCNAME (yet)
-- some values are set in artpack.lua
video = {} 
video.settings = {}
