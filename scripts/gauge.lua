-- disclaimer: red / yellow / green can be replaced with arbitrary colors

M_PI = 3.14159265358979323846

function angle_MathFromClock360(angle)
	-- convert from [0..360] to [0..2PI]
	angle = angle * 2 * M_PI / 360
	-- move 0 to 'east', and reverse direction
	angle = M_PI / 2 - angle
	return angle
end

function color_interpolate(t, c1, c2)
	local color = { }
	color.r = (1 - t) * c1.r + t * c2.r
	color.g = (1 - t) * c1.g + t * c2.g
	color.b = (1 - t) * c1.b + t * c2.b
	return color
end

function drawCircleSegment(circle, range)
	 c_pushMatrix();
	c_translate(circle.center.x, circle.center.y, 0);
	c_drawCircle(range.arc_start.angle, range.arc_end.angle, 
							 20, circle.inner_radius, circle.radius,
							 range.arc_start.color, range.arc_end.color, 
							 range.arc_start.color, range.arc_end.color)
	c_popMatrix();
end

function drawRectangle(rect, range)
	 c_pushMatrix();
	 -- convert range to pixels
	 local left = rect.right + 
			range.range_end.width * (rect.left - rect.right)
	 local right = rect.right + 
			range.range_start.width * (rect.left - rect.right)

	 c_translate(left, rect.top, 0)
	 c_drawRectangle(right - left, rect.bottom - rect.top,
							range.range_end.color, range.range_end.color,
							range.range_start.color, range.range_start.color)
	c_popMatrix();
end

function drawGauge(speed) -- between 0 and 1
	 color1 = { r = .2, b = .2, g = .2 };
	 color2 = { r = .6, b = .6, g = .6 };
	 c_drawRectangle(248, 150, color1, color2, color2, color1);

	-- circle center in pixel coordinates
	-- circle inner and outer radius in pixels
	-- write("speed: ", speed, "\n");
	circle = {
		center = { x = 180, y = 75 },
		inner_radius = 20,
		radius = 60
	}
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
	-- for each area, draw a circle arc, with optional color
	-- interpolation through the arc
	for index, range in angles do
		if(speed > range.arc_start.speed) then
			if(speed > range.arc_end.speed) then
				drawCircleSegment(circle, range);
			else
				 local t = (speed - range.arc_start.speed) / 
						(range.arc_end.speed - range.arc_start.speed)
				range.arc_end.angle = 
						(1 - t) * range.arc_start.angle + 
					 t * range.arc_end.angle;
				range.arc_end.color = 
						color_interpolate(t, range.arc_start.color, range.arc_end.color);
				drawCircleSegment(circle, range);
			end
		end
	end
end

function drawTurbo(charge) -- between 0 and 1
	 -- write("charge: ", charge, "\n");
	 -- Rectangle in pixel coordinates
	 rect = { top = 5, left = 5, right = 130, bottom = 45 }
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
	 -- for each area, draw a rectangle, with optional color
	 -- interpolation through the arc
	 for index, range in ranges do
			if(charge > range.range_start.charge) then
				 if(charge > range.range_end.charge) then
						drawRectangle(rect, range);
				 else
						local t = (charge - range.range_start.charge) / 
							 (range.range_end.charge - range.range_start.charge)
						range.range_end.width = 
							 (1 - t) * range.range_start.width + 
							 t * range.range_end.width;
						range.range_end.color = 
							 color_interpolate(t, range.range_start.color, range.range_end.color);
						drawRectangle(rect, range);
				 end
			end
	 end
end
