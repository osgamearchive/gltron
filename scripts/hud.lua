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

function drawHUD(width, height, score, ai, speed_digital, speed_analog, booster, fps)
	-- speedometer
	c_pushMatrix()
	c_translate(HUDConfig.Speed.x, HUDConfig.Speed.y, 0)
	c_drawHUDSurface(eHUD.Speed)
	drawSpeedDigital(speed_digital, HUDConfig.SpeedText, HUDColors.Speed)
	c_drawHUDMask(eHUD.MaskSpeed, 17)
	drawSpeedAnalog(speed_analog, HUDSpeedDial.circle, HUDSpeedDial.angles)
	c_drawHUDMask(eHUD.MaskTurbo, 18)
	drawTurbo(booster, HUDTurbo.rect, HUDTurbo.ranges)
	c_drawHUDMask(-1, -1)
	c_popMatrix()
	
	-- 2d map
	c_pushMatrix()
	c_translate(HUDConfig.MapFrame.x, HUDConfig.MapFrame.y, 0)
	c_drawHUDSurface(eHUD.Map)
	c_translate(HUDConfig.Map.x, HUDConfig.Map.y, 0)
	c_draw2D(
		HUDConfig.Map.w,
		HUDConfig.Map.h
		)
	c_popMatrix()
	
	-- Score
	c_pushMatrix()
	c_translate(HUDConfig.ScoreFrame.x, HUDConfig.ScoreFrame.y, 0)
	c_drawHUDSurface(eHUD.Scores)
	c_translate(HUDConfig.Score.x, HUDConfig.Score.y, 0)
	c_color(HUDColors.Score.r, HUDColors.Score.g, HUDColors.Score.b, HUDColors.Score.a)
	c_drawTextFitIntoRect(format("%d", score),
		HUDConfig.Score.w, HUDConfig.Score.h,
		FontFormat.ScaleFitVertically)
	c_popMatrix()

	-- AI status
	c_pushMatrix()
	c_translate(HUDConfig.AI_Status.x, HUDConfig.AI_Status.y, 0)
	c_color(HUDColors.AI_Status.r, HUDColors.AI_Status.g, HUDColors.AI_Status.b, HUDColors.AI_Status.a)
 	c_drawTextFitIntoRect(
 		"AI computer", -- ai,
 		HUDConfig.AI_Status.w, HUDConfig.AI_Status.h,
 		FontFormat.ScaleFitHorizontally + 
 		FontFormat.ScaleFitVertically)
	c_popMatrix()

	-- Console
	c_pushMatrix()

	c_translate(HUDConfig.Console.x, HUDConfig.Console.y, 0)
	c_drawHUDSurface(eHUD.Console)
	c_popMatrix()
	
	-- FPS
	c_pushMatrix()
	c_translate(HUDConfig.FPS.x, HUDConfig.FPS.y, 0)
	c_drawHUDSurface(eHUD.FPS)
	c_popMatrix()
end

