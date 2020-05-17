function shell_escape(str)
    str = replace(str, "'" => "'\''")
    return "'$str'"
end


jl_cflags = "-std=gnu99" * " -I" *
	shell_escape(abspath(Sys.BINDIR, Base.INCLUDEDIR, "julia"))
if Sys.isunix()
	jl_cflags *= " -fPIC"
end


jl_cxxflags = "-I" *
	shell_escape(abspath(Sys.BINDIR, Base.INCLUDEDIR, "julia"))
if Sys.isunix()
	jl_cxxflags *= " -fPIC"
end


lib_dir = abspath(Sys.BINDIR, Base.LIBDIR)
jl_ldflags = "-L$(shell_escape(lib_dir))"
if Sys.iswindows()
	jl_ldflags *= " -Wl,--stack,8388608"
elseif !Sys.isapple()
	jl_ldflags *= " -Wl,--export-dynamic"
end


jl_ldlibs = ""
lib_nm = (ccall(:jl_is_debugbuild, Cint, ()) != 0) ?  "julia-debug" : "julia"
if Sys.isunix()
	jl_ldlibs *= "-Wl,-rpath,$(shell_escape(lib_dir))"
	if !Sys.isapple()
		private_lib_dir = abspath(Sys.BINDIR, Base.PRIVATE_LIBDIR)
		jl_ldlibs *= " -Wl,-rpath,$(shell_escape(private_lib_dir))"
	end
	jl_ldlibs *= " -l$lib_nm"
else
	jl_ldlibs *= "-l$lib_nm -lopenlibm"
end


# set environment variables
ENV["JL_CFLAGS"]   = jl_cflags;
ENV["JL_CXXFLAGS"] = jl_cxxflags;
ENV["JL_LDFLAGS"]  = jl_ldflags;
ENV["JL_LDLIBS"]   = jl_ldlibs;


# run
run(`make -f MakefileXZ`)
run(`make`)
