-- config for 1024x768
-- no scaling

HUDConfig = {
	Speed = { x = 776, y = 0 },
	SpeedText = { top = 60, left = 150, right = 194, bottom = 88 },
	MapFrame = { x = 10, y = 10 },
	Map = { x = 10, y = 10, w = 190, h = 190 },
	ScoreFrame = { x = 220, y = 10 },
	Score = { x = 10, y = 4, w = 30, h = 24 },
	AI_Status = { x = 400, y = 728, w = 228, h = 30 },
	Console = { x = 10, y = 648 },
	FPS = { x = 850, y = 648 },
}

HUDColors = {
	Speed = { r = 0, g = 0, b = .4, a = 1 },
	Score = { r = 1, g = 1, b = 0, a = 1 },
	AI_Status = { r = 1, g = 1, b = 1, a = 1 },
}
-- TODO: console, FPS

M_PI = 3.14159265358979323846

function angle_MathFromClock360(angle)
	-- convert from [0..360] to [0..2PI]
	angle = angle * 2 * M_PI / 360
	-- move 0 to 'east', and reverse direction
	angle = M_PI / 2 - angle
	return angle
end

HUDSpeedDial = {
	circle = {
		center = { x = 174, y = 75 },
		inner_radius = 23,
		radius = 66
	},
	angles = {
	-- angles, and speed range for green area
		{
		arc_start = {
			angle = angle_MathFromClock360(0),
			color = { r = 0, g = .6, b = 0 },
			speed = 0,
		},
	 	arc_end = { 
			angle = angle_MathFromClock360(160),
			color = { r = 0, g = 1, b = 0 },
			speed = 0.5,
		},
		},
	-- angles, and speed range for green to yellow fade
		{
		arc_start = {
			angle = angle_MathFromClock360(160),
			color = { r = 0, g = 1, b = 0 },
			speed = 0.5,
		},
	 	arc_end = { 
			angle = angle_MathFromClock360(190),
			color = { r = 1, g = 1, b = 0 },
			speed = 0.7,
		},
		},
	-- angles, and speed range for yellow area
		{
		arc_start = {
			angle = angle_MathFromClock360(190),
			color = { r = 1, g = 1, b = 0 },
			speed = 0.7,
		},
	 	arc_end = { 
			angle = angle_MathFromClock360(270),
			color = { r = 1, g = 1, b = 0 },
			speed = 0.8,
		},
		},
	-- angles, and speed range for yellow to red fade
		{
		arc_start = {
			angle = angle_MathFromClock360(270),
			color = { r = 1, g = 1, b = 0 },
			speed = 0.8,
		},
	 	arc_end = { 
			angle = angle_MathFromClock360(300),
			color = { r = 1, g = 0, b = 0 },
			speed = 0.9,
		},
		},
	-- angles, and speed range for red area
		{
		arc_start = {
		angle = angle_MathFromClock360(300),
		color = { r = 1, g = 0, b = 0 },
		speed = 0.9,
		},
	 	arc_end = { 
			angle = angle_MathFromClock360(359),
			color = { r = 1, g = 0, b = 0 },
			speed = 1.0,
		},
		},
	}
}

HUDTurbo = {
	rect = { top = 5, left = 5, right = 130, bottom = 45 },
	ranges = {
		{
		-- width, and charge range for red area
			range_start = {
				color = { r = .6, g = 0, b = 0 },
				charge = 0.0,
				width = 0.0,
			},
			range_end = {
				color = { r = 1, g = 0, b = 0 },
				charge = 0.3,
				width = 0.3,
			},
		},
		-- width, and charge range for yellow to red fade
		{
			range_start = {
				color = { r = 1, g = 0, b = 0 },
				charge = 0.3,
				width = 0.3,
			},
			range_end = {
				color = { r = 1, g = 1, b = 0 },
				charge = 0.5,
				width = 0.5,
			},
		},
		-- width, and charge range for green to yellow fade
		{
			range_start = {
				color = { r = 1, g = 1, b = 0 },
				charge = 0.5,
				width = 0.5,
			},
			range_end = {
				color = { r = 0, g = 1, b = 0 },
				charge = 0.7,
				width = 0.7,
			},
		},
		-- width, and charge range for green area
		{
			range_start = {
				color = { r = 0, g = 1, b = 0 },
				charge = 0.7,
				width = 0.7,
			},
			range_end = {
				color = { r = 0, g = .6, b = 0 },
				charge = 1.0,
				width = 1.0,
			},
		}
	}
}