-- very similiar to setupSoundTracks() in audio.lua

function setupArtpacks()
   local i,name
   for i,name in artpacks do
      if name == settings.current_artpack then
	 current_artpack_index = i
	 return
      end
   end
   settings.current_artpack = artpacks[1]
   current_artpack_index = 1
end

function nextArtpack()
   if current_artpack_index < getn(artpacks) then
      current_artpack_index = current_artpack_index + 1
   else
      current_artpack_index = 1
   end
   settings.current_artpack = artpacks[ current_artpack_index ]
   c_reloadArtpack()
end

function previousArtpack()
   if current_artpack_index > 1 then
      current_artpack_index = current_artpack_index - 1
   else
      current_artpack_index = getn(artpacks) 
   end
   settings.current_artpack = artpacks[ current_artpack_index ]
   c_reloadArtpack()
end



