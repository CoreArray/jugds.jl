module JuGDS

using Compat

import	Base: ifelse, joinpath, isfile, show, print, println

export	type_file_gds, type_node_gds, create_gds, open_gds, close_gds,
		root_gdsn, name_gdsn, rename_gdsn, ls_gdsn, index_gdsn



####  Open and initialize the CoreArray Binary Library  ####

@linux_only   libfn = "libCoreArray.so"
@osx_only     libfn = "libCoreArray.dylib"
@windows_only libfn = "libCoreArray.dll"

global libname = joinpath(Pkg.dir(), "JuGDS", "deps", libfn)

if !isfile(libname)
	error("The CoreArray library cannot be found; it may not have been built correctly.\nTry Pkg.build(\"JuGDS\").")
end

const libCoreArray = libname

function __init__()
	ccall((:GDS_Init, libCoreArray), Void, ())
end



####  Type of GDS File and Node  ####

type type_file_gds
	filename::String
	id::Int32
	readonly::Bool
end

type type_node_gds
	id::Int32
	ptr::Ptr{Void}
end



####  Internal Functions  ####

macro check(ex)
	return quote
		local val = $ex
		local s = ccall((:GDS_Error, libCoreArray), Ptr{UInt8}, ())
		if s != C_NULL
			error(bytestring(s))
		end
		val
	end
end


function text_push(pobj::Ptr{Void}, txt::Ptr{Uint8}, len::Csize_t)
	obj = unsafe_pointer_to_objref(pobj)
	push!(obj, bytestring(txt, len))
	nothing
end

function text_set(pobj::Ptr{Void}, index::Cssize_t, txt::Ptr{Uint8}, len::Csize_t)
	s = unsafe_pointer_to_objref(pobj)
	s[index+1] = bytestring(txt, len)
	nothing
end


const c_text_push = cfunction(text_push, Void, (Ptr{Void}, Ptr{Uint8}, Csize_t))
const c_text_set  = cfunction(text_set,  Void, (Ptr{Void}, Cssize_t, Ptr{Uint8}, Csize_t))



####  GDS File  ####

# create a GDS file
function create_gds(filename::String)
	id = @check ccall((:GDS_File_Create, libCoreArray), Cint, (Ptr{UInt8},),
		filename)
	return type_file_gds(filename, id, false)
end


# open an existing GDS file
function open_gds(filename::String, readonly::Bool=true)
	id = @check ccall((:GDS_File_Open, libCoreArray), Cint, (Ptr{UInt8},Bool),
		filename, readonly)
	return type_file_gds(filename, id, readonly)
end


# close the GDS file
function close_gds(file::type_file_gds)
	@check ccall((:GDS_File_Close, libCoreArray), Void, (Cint,),
		file.id)
	file.filename = ""
	file.id = -1
	file.readonly = true
	return nothing
end


# synchronize the GDS file
function sync_gds(file::type_file_gds)
	@check ccall((:GDS_File_Sync, libCoreArray), Void, (Cint,),
		file.id)
	return nothing
end



####  GDS Node  ####

# get the root of GDS file
function root_gdsn(file::type_file_gds)
	p = [ C_NULL ]
	id = @check ccall((:GDS_Node_Root, libCoreArray), Cint,
		(Cint, Ptr{Ptr{Void}}), file.id, pointer(p))
	return type_node_gds(id, p[1])
end


# get the name of GDS node
function name_gdsn(obj::type_node_gds, fullname::Bool=false)
	rv = fill("", 0)
	@check ccall((:GDS_Node_Name, libCoreArray), Void,
		(Cint, Ptr{Void}, Bool, Ptr{Void}, Ptr{Void}),
		obj.id, obj.ptr, fullname, c_text_push, pointer_from_objref(rv))
	return rv[1]
end


# rename the GDS node
function rename_gdsn(obj::type_node_gds, newname::String)
	@check ccall((:GDS_Node_Rename, libCoreArray), Void,
		(Cint, Ptr{Void}, Ptr{UInt8}), obj.id, obj.ptr, newname)
	return obj
end


# get the name(s) of child node
function ls_gdsn(obj::type_node_gds, has_hidden::Bool=false)
	rv = fill("", 0)
	@check ccall((:GDS_Node_ListName, libCoreArray), Void,
		(Cint, Ptr{Void}, Bool, Ptr{Void}, Ptr{Void}),
		obj.id, obj.ptr, has_hidden, c_text_push, pointer_from_objref(rv))
	return rv
end


# get a specified GDS node with path
function index_gdsn(obj::type_node_gds, path::String, silent::Bool=false)
	p = [ C_NULL ]
	id = @check ccall((:GDS_Node_Index, libCoreArray), Cint,
		(Cint, Ptr{Void}, Ptr{UInt8}, Bool, Ptr{Ptr{Void}}),
		obj.id, obj.ptr, path, silent, pointer(p))
	if p[1] != C_NULL
		return type_node_gds(id, p[1])
	else
		return nothing
	end
end

function index_gdsn(file::type_file_gds, path::String, silent::Bool=false)
	return index_gdsn(root_gdsn(file), path, silent)
end


# GDS variable information
immutable type_info_gdsn
end






####  Dispaly  ####

function show(io::IO, file::type_file_gds)
	print_with_color(:bold, "File:")
	println(" ", file.filename)
	show(io, root_gdsn(file))
end


function show(io::IO, obj::type_node_gds)

	function enum(obj::type_node_gds, space::String, level::Int,
			expand::Bool, fullname::Bool)
		println(io, space, "+ ", name_gdsn(obj, fullname))
		for nm = ls_gdsn(obj)
			enum(index_gdsn(obj, nm), ifelse(level==1, "|--", "|  ") * space,
				level+1, expand, false)
		end
	end

	print_with_color(:bold, "")
	enum(obj, "", 1, true, true)
end

end
