module JuGDS

using Compat

import	Base: joinpath, isfile, show, print, println

export	TypeFileGDS, TypeNodeGDS, create_gds, open_gds, close_gds,
		root_gdsn, name_gdsn, rename_gdsn, ls_gdsn, index_gdsn



####  Find and Open the CoreArray Binary Library  ####

@linux_only   libfn = "libCoreArray.so"
@osx_only     libfn = "libCoreArray.dylib"
@windows_only libfn = "libCoreArray.dll"

global libname = joinpath(Pkg.dir(), "JuGDS", "deps", libfn)

if !isfile(libname)
	error("The CoreArray library cannot be found; it may not have been built correctly.\n Try include(\"build.jl\") from within the deps directory.")
end

const libCoreArray = libname


####  Type of GDS File and Node  ####

type TypeFileGDS
	filename::AbstractString
	id::Int32
	readonly::Bool
end

type TypeNodeGDS
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

function create_gds(filename::AbstractString)
	id = @check ccall((:GDS_File_Create, libCoreArray), Cint, (Ptr{UInt8},),
		filename)
	return TypeFileGDS(filename, id, false)
end


function open_gds(filename::AbstractString, readonly::Bool=true)
	id = @check ccall((:GDS_File_Open, libCoreArray), Cint, (Ptr{UInt8},Bool),
		filename, readonly)
	return TypeFileGDS(filename, id, readonly)
end


function close_gds(file::TypeFileGDS)
	@check ccall((:GDS_File_Close, libCoreArray), Void, (Cint,),
		file.id)
	file.filename = ""
	file.id = -1
	file.readonly = true
	return nothing
end



####  GDS Node  ####

function root_gdsn(file::TypeFileGDS)
	p = [ C_NULL ]
	id = @check ccall((:GDS_Node_Root, libCoreArray), Cint,
		(Cint, Ptr{Ptr{Void}}), file.id, pointer(p))
	return TypeNodeGDS(id, p[1])
end


function name_gdsn(obj::TypeNodeGDS, fullname::Bool=false)
	rv = fill("", 0)
	@check ccall((:GDS_Node_Name, libCoreArray), Void,
		(Cint, Ptr{Void}, Bool, Ptr{Void}, Ptr{Void}),
		obj.id, obj.ptr, fullname, c_text_push, pointer_from_objref(rv))
	return rv[1]
end


function rename_gdsn(obj::TypeNodeGDS, newname::AbstractString)
	@check ccall((:GDS_Node_Rename, libCoreArray), Void,
		(Cint, Ptr{Void}, Ptr{UInt8}), obj.id, obj.ptr, newname)
	return obj
end


function ls_gdsn(obj::TypeNodeGDS, has_hidden::Bool=false)
	rv = fill("", 0)
	@check ccall((:GDS_Node_ListName, libCoreArray), Void,
		(Cint, Ptr{Void}, Bool, Ptr{Void}, Ptr{Void}),
		obj.id, obj.ptr, has_hidden, c_text_push, pointer_from_objref(rv))
	return rv
end


function index_gdsn(obj::TypeNodeGDS, path::AbstractString, silent::Bool=false)
	p = [ C_NULL ]
	id = @check ccall((:GDS_Node_Index, libCoreArray), Cint,
		(Cint, Ptr{Void}, Ptr{UInt8}, Bool, Ptr{Ptr{Void}}),
		obj.id, obj.ptr, path, silent, pointer(p))
	return TypeNodeGDS(id, p[1])
end

function index_gdsn(file::TypeFileGDS, path::AbstractString, silent::Bool=false)
	return index_gdsn(root_gdsn(file), path, silent)
end



####  Dispaly  ####

function show(io::IO, file::TypeFileGDS)
	print_with_color(:bold, "File:")
	println(" ", file.filename)
	show(io, ls_gdsn(root_gdsn(file)))
end

function show(io::IO, obj::TypeNodeGDS)
	println(io, name_gdsn(obj, true))
end

end
