GetMenuValue = {}
MenuAction = {}

GetMenuValue[ MenuC.type.menu ] = function ( menu )
   return "(menu)"
end
MenuAction[ MenuC.type.menu ] = function ( menu )
   Menu.current = menu 
   Menu.active = 1
end

GetMenuValue[ MenuC.type.list ] = function ( menu )
   local value
   if Menu[menu].read then 
      value = Menu[menu].read()
   else
      value = "undef"
   end
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

GetMenuValue[ MenuC.type.key ] = function ( menu )
   -- TODO not implemented
   return "unknown key"
end

GetMenuValue[ MenuC.type.action ] = function ( menu )
   return "(action)"
end

GetMenuValueWidth = function ( menu )
   return strlen( GetMenuValue[ Menu[menu].type ]( menu ) )
end

GetMenuValueString = function ( menu )
   return GetMenuValue[ Menu[menu].type ]( menu )
end
