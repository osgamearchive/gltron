function setupSoundTrack()
   local i,name
   for i,name in tracks do
      if name == current_track then
	 current_track_index = i
	 return
      end
   end
   current_track = tracks[1]
   current_track_index = 1
end

function nextTrack()
   if current_track_index < getn(tracks) then
      current_track_index = current_track_index + 1
   else
      current_track_index = 1
   end
   current_track = tracks[ current_track_index ]
   c_reloadTrack()
end

function previousTrack()
   if current_track_index > 1 then
      current_track_index = current_track_index - 1
   else
      current_track_index = getn(tracks) 
   end
   current_track = tracks[ current_track_index ]
   c_reloadTrack()
end
