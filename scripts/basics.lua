function script_print(...)
   write("[script] ")
   for i=1,arg.n do
      write(arg[i])
   end
   write("\n")
end

