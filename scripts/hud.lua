eHUD = {
	Speed = 0,
	MaskSpeed = 1,
	MaskTurbo = 2,
	Console = 3,
	Map = 4,
	Scores = 5,
	FPS = 6,
	ElementCount = 7
}

-- 	drawHud: parameters
-- 	- Score (or -1 if disabled)
-- 	- AI status ("computer player" or "")
-- 	- Speed digital (absolute value)
-- 	- Speed analog (1 for default speed, > 1 during acceleration)
-- 	- Booster value (between 0 and 1)

function fabs(d)
	if(d < 0) then
		return -d
	else
		return d
	end
end

function getConfig(aspect)
	best = -1
	best_aspect = 999
	for i = 1,getn(HUDConfig) do
		if(fabs(HUDConfig[i].aspect - aspect) < best_aspect) then
			best_aspect = fabs(HUDConfig[i].aspect - aspect)
			best = i
		end
	end
	write(format("aspect: %f, config: %d (%f)\n", aspect, best, HUDConfig[best].aspect))
	return HUDConfig[best]
end

function drawHUD(width, height, score, ai, speed_digital, speed_analog, booster, fps, 
	pause_message, pause_color_r, pause_color_g, pause_color_b)
	config = getConfig(width / height)
	-- speedometer
	c_pushMatrix()
	c_translate(config.Speed.x, config.Speed.y, 0)
	c_drawHUDSurface(eHUD.Speed)
	drawSpeedDigital(speed_digital, config.Speed_Text, HUDColors.Speed)
	c_drawHUDMask(eHUD.MaskSpeed, 17)
	drawSpeedAnalog(speed_analog, HUDSpeedDial.circle, HUDSpeedDial.angles)
	c_drawHUDMask(eHUD.MaskTurbo, 18)
	drawTurbo(booster, HUDTurbo.rect, HUDTurbo.ranges)
	c_drawHUDMask(-1, -1)
	c_popMatrix()
	
	-- 2d map
	c_pushMatrix()
	c_translate(config.MapFrame.x, config.MapFrame.y, 0)
	c_drawHUDSurface(eHUD.Map)
	c_translate(config.Map.x, config.Map.y, 0)
	c_draw2D(
		config.Map.w,
		config.Map.h
		)
	c_popMatrix()
	
	-- Score
	if(settings.show_scores == 1) then
		c_pushMatrix()
		c_translate(config.ScoreFrame.x, config.ScoreFrame.y, 0)
		c_drawHUDSurface(eHUD.Scores)
		c_translate(config.Score.x, config.Score.y, 0)
		c_color(HUDColors.Score.r, HUDColors.Score.g, HUDColors.Score.b, HUDColors.Score.a)
		c_drawTextFitIntoRect(format("%d", score),
			config.Score.w, config.Score.h,
			FontFormat.ScaleFitVertically)
		c_popMatrix()
	end

	-- AI status
	if(settings.show_ai_status == 1) then
		c_pushMatrix()
		c_translate(config.AI_Status.x, config.AI_Status.y, 0)
		c_color(HUDColors.AI_Status.r, HUDColors.AI_Status.g, HUDColors.AI_Status.b, HUDColors.AI_Status.a)
 		c_drawTextFitIntoRect(
 			ai,
 			config.AI_Status.w, config.AI_Status.h,
 			FontFormat.ScaleFitHorizontally + 
 			FontFormat.ScaleFitVertically)
		c_popMatrix()
	end

	-- Console
	if(settings.show_console == 1) then
		c_pushMatrix()
		c_translate(config.Console.x, config.Console.y, 0)
		c_drawHUDSurface(eHUD.Console)
		c_translate(config.Console_Text.x, config.Console_Text.y, 0)
		for i = 0,config.Console.lines - 1 do
			-- write(format("[console] %d: %s\n", i, console_GetLine(i)))
			c_drawTextFitIntoRect(
				console_GetLine(i),
				config.Console_Text.w,
				config.Console_Text.h,
				FontFormat.ScaleFitVertically)
			c_translate(0, config.Console_Text.h + config.Console_Text.space, 0)
		end
		c_popMatrix()
		
	end
	
	-- FPS
	if(settings.show_fps == 1) then
		c_pushMatrix()
		c_translate(config.FPS.x, config.FPS.y, 0)
		c_drawHUDSurface(eHUD.FPS)
		c_translate(config.FPS_Text.x, config.FPS_Text.y, 0)
		c_drawTextFitIntoRect(
			format("%d", fps),
			config.FPS_Text.w,
			config.FPS_Text.h,
			FontFormat.ScaleFitVertically)
		
		c_popMatrix()
	end
	
	-- Pause Text
	
	if(pause_message) then
		c_pushMatrix()
		c_translate(config.Pause.x, config.Pause.y, 0)
		c_color(pause_color_r, pause_color_g, pause_color_b, 1)
		c_drawTextFitIntoRect(
			pause_message,
			config.Pause.w,
			config.Pause.h,
			FontFormat.ScaleFitVertically +
			FontFormat.AlignCenter)
		c_popMatrix()
	end
end

