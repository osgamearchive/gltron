-- very similiar to setupSoundTracks() in audio.lua

function setupArtpacks()
	artpacks = c_loadDirectory(ePath.ART);
	local i,name
	for i,name in artpacks do
		write("found artpack: ", name)
		write("\n")
	end
	for i,name in artpacks do
		if name == settings.current_artpack then
			current_artpack_index = i
			c_setArtPath()
			return
		end
	end
	settings.current_artpack = artpacks[1]
	current_artpack_index = 1
	c_setArtPath()
end

function nextArtpack()
	if current_artpack_index < getn(artpacks) then
		current_artpack_index = current_artpack_index + 1
	else
		current_artpack_index = 1
	end
	tmp.current_artpack = artpacks[ current_artpack_index ]
end

function previousArtpack()
	if current_artpack_index > 1 then
		current_artpack_index = current_artpack_index - 1
	else
		current_artpack_index = getn(artpacks) 
	end
	tmp.current_artpack = artpacks[ current_artpack_index ]
end

-- copy-paste from setup/next/previous Artpack
function setupLevels()
	levels = c_loadDirectory(ePath.LEVEL);
	local i,name
		for i,name in levels do
			write("found level: ", name)
			write("\n")
		end
	for i,name in levels do
		if name == settings.current_level then
					current_level_index = i
					return
		end
	end
	settings.current_level = levels[1]
	current_level_index = 1
end

function nextLevel()
	if current_level_index < getn(levels) then
		current_level_index = current_level_index + 1
	else
		current_level_index = 1
	end
	tmp.current_level = levels[ current_level_index ]
end

function previousLevel()
	if current_level_index > 1 then
		current_level_index = current_level_index - 1
	else
		current_level_index = getn(levels) 
	end
	tmp.current_level = levels[ current_level_index ]
end


