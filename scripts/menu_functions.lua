GetMenuValue = {}
MenuAction = {}

GetMenuValue[ MenuC.type.menu ] = function ( menu )
   -- return "(menu)"
   return ""
end

MenuAction[ MenuC.type.menu ] = function ( menu )
   Menu.current = menu 
   Menu.active = 1
end

GetMenuValue[ MenuC.type.list ] = function ( menu )
   local value
   if Menu[menu].read then value = Menu[menu].read() end
   if not value then value = "undef" end

   -- script_print(menu .. " --> " .. value )
   local i
   for i=1,getn(Menu[menu].values) do
      if Menu[menu].values[i] == value then return Menu[menu].labels[i] end
   end
   return "unknown (" .. value .. ")"
end

MenuAction[ MenuC.type.list ] = function ( menu )
   local value
   if Menu[menu].read then 
      value = Menu[menu].read()
   else
      value = "undef"
   end

   local nValues = getn(Menu[menu].values)
   local i
   for i=1,nValues do
      if Menu[menu].values[i] == value then
	 if i < nValues then
	    value = Menu[menu].values[i + 1]
	 else
	    value = Menu[menu].values[1]
	 end
	 return Menu[menu].store( value ) 
      end
   end
   return Menu[menu].store( Menu[menu].values[1] )
end

MenuAction[ MenuC.type.action ] = function ( menu )
   Menu[menu].action()
end

GetMenuValue[ MenuC.type.slider ] = function ( menu )
   return Menu[menu].read()
end

MenuAction[ MenuC.type.slider ] = function( menu )
   if Menu[menu].action then
      Menu[menu].action()
   end
end

MenuAction[ MenuC.type.key ] = function ( menu )
   local player = Menu[menu].player
   local event = Menu[menu].event
   configure_player = player
   configure_event = event
   c_configureKeyboard()
end

GetMenuValue[ MenuC.type.key ] = function ( menu )
   local player = Menu[menu].player
   local event = Menu[menu].event
   return c_getKeyName( settings.keys[ player ][ event ] )
end

GetMenuValue[ MenuC.type.action ] = function ( menu )
   -- return "(action)"
   return ""
end

GetMenuValueWidth = function ( menu )
   return strlen( GetMenuValue[ Menu[menu].type ]( menu ) )
end

GetMenuValueString = function ( menu )
   -- write(format("GetMenuValueString: '%s'\n", menu));
   return GetMenuValue[ Menu[menu].type ]( menu )
end

