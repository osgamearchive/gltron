level = {

version = 71,

-- the sizes in the level are all in the [0,1] range and can be
-- scaled to the appropriate arena size

scalable = 1,

-- collsision detection takes place against these lines
boundary = {
	{ { x = 0, y = 0 }, { x = 1, y = 0 } },
	{ { x = 1, y = 0 }, { x = 1, y = 1 } },
	{ { x = 1, y = 1 }, { x = 0, y = 1 } },
	{ { x = 0, y = 1 }, { x = 0, y = 0 } }
},

-- spawn points
-- (they don't have to be sorted, they will be randomized anyway)

spawn = {
	{ x = .25, y = .5 },
	{ x = .5, y = .25 },
	{ x = .5, y = .75 },
	{ x = .75, y = .5 }
},

geometry = {
	-- floor geometry is used to generate reflections, etc.
	floor = {
		-- triangles = 0, quads = 1
		type = 1,
		-- uv = 4, normal = 2, positon = 1
		vertexformat = 7, 
		 
		shading = {
			lit = 0, -- no lighting, only diffuse texture is applied
			textures = { diffuse = "floor.png" }
		},
		vertices = {
			{ pos = { x = 0, y = 0, z = 0 },
				uv = { u = 0, v = 0 } },
			{ pos = { x = 1, y = 0, z = 0 },
				uv = { u = 1, v = 0 } },
			{ pos = { x = 1, y = 1, z = 0 },
				uv = { u = 1, v = 1 } },
			{ pos = { x = 0, y = 1, z = 0 },
				uv = { u = 0, v = 1 } }
		},
		indices = { 0, 1, 2, 3 }
	},

	arena = {
		type = "quads",
		shading = {
			lit = 0, -- no lighting, only diffuse texture is applied
			textures = { diffuse = "walls.png" }
		},
		vertices = {
			{ -- quad0
			{ pos = { x = 1, y = 0, z = 0 },
				uv = { u = 0, v = 0 } },
			{ pos = { x = 0, y = 0, z = 0 },
				uv = { u = 0.25, v = 0 } },
			{ pos = { x = 0, y = 0, z = 1 },
				uv = { u = 0.25, v = 1 } },
			{ pos = { x = 1, y = 0, z = 1 },
				uv = { u = 0, v = 1 } }
			},
			{ -- quad1
			{ pos = { x = 1, y = 1, z = 0 },
				uv = { u = 0.25, v = 0 } },
			{ pos = { x = 1, y = 0, z = 0 },
				uv = { u = 0.5, v = 0 } },
			{ pos = { x = 1, y = 0, z = 1 },
				uv = { u = 0.5, v = 1 } },
			{ pos = { x = 1, y = 1, z = 1 },
				uv = { u = 0.25, v = 1 } }
			},
			{ -- quad2
			{ pos = { x = 0, y = 1, z = 0 },
				uv = { u = 0.5, v = 0 } },
			{ pos = { x = 1, y = 1, z = 0 },
				uv = { u = 0.75, v = 0 } },
			{ pos = { x = 1, y = 1, z = 1 },
				uv = { u = 0.75, v = 1 } },
			{ pos = { x = 0, y = 1, z = 1 },
				uv = { u = 0.5, v = 1 } }
			},
			{ -- quad3
			{ pos = { x = 0, y = 0, z = 0 },
				uv = { u = 0.75, v = 0 } },
			{ pos = { x = 0, y = 1, z = 0 },
				uv = { u = 1, v = 0 } },
			{ pos = { x = 0, y = 1, z = 1 },
				uv = { u = 1, v = 1 } },
			{ pos = { x = 0, y = 0, z = 1 },
				uv = { u = 0.75, v = 1 } }
			},
		}
	}
}

}
