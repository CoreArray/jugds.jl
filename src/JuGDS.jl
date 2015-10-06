# ===========================================================================
#
# JuGDS.jl: Julia Interface to CoreArray Genomic Data Structure (GDS) Files
#
# Copyright (C) 2015	Xiuwen Zheng
#
# This is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License Version 3 as
# published by the Free Software Foundation.
#
# JuGDS is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with JuGDS.
# If not, see <http://www.gnu.org/licenses/>.

module JuGDS

using Compat

import	Base: ifelse, joinpath, isfile, show, print, println, utf8

export	type_file_gds, type_node_gds, create_gds, open_gds, close_gds,
		root_gdsn, name_gdsn, rename_gdsn, ls_gdsn, index_gdsn, getfolder_gdsn,
		objdesp_gdsn



####  Open and initialize the CoreArray Binary Library  ####

@linux_only	  libfn = "libCoreArray.so"
@osx_only	  libfn = "libCoreArray.dylib"
@windows_only libfn = "libCoreArray.dll"

global libname = joinpath(Pkg.dir(), "JuGDS", "deps", libfn)

if !isfile(libname)
	error("The CoreArray library cannot be found; it may not have been built correctly.\nTry Pkg.build(\"JuGDS\").")
end

const libCoreArray = libname

function __init__()
	ccall((:GDS_Init, libCoreArray), Void, ())
end



####  Type of GDS File and Node	 ####

type type_file_gds
	filename::AbstractString
	id::Int32
	readonly::Bool
end

type type_node_gds
	id::Int32
	ptr::Ptr{Void}
end

# GDS variable information
immutable type_info_gdsn
	name::UTF8String
	fullname::UTF8String
	storage::ASCIIString
	trait::ASCIIString
	gds_type::ASCIIString
	is_array::Bool
	dim::Vector{Int64}
	encoder::ASCIIString
	compression::ASCIIString
	compression_ratio::Float64
	size::Int64
	good::Bool
	hidden::Bool
	message::UTF8String
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


function text_push(pobj::Ptr{Void}, txt::Ptr{UInt8}, len::Csize_t)
	obj = unsafe_pointer_to_objref(pobj)
	push!(obj, utf8(txt, len))
	nothing
end

function text_set(pobj::Ptr{Void}, index::Csize_t, txt::Ptr{UInt8}, len::Csize_t)
	s = unsafe_pointer_to_objref(pobj)
	s[index] = utf8(txt, len)
	nothing
end

function array_push(pobj::Ptr{Void}, val)
	obj = unsafe_pointer_to_objref(pobj)
	push!(obj, val)
	nothing
end


const c_text_push  = cfunction(text_push,  Void, (Ptr{Void}, Ptr{UInt8}, Csize_t))
const c_text_set   = cfunction(text_set,   Void, (Ptr{Void}, Csize_t, Ptr{UInt8}, Csize_t))
const c_int64_push = cfunction(array_push, Void, (Ptr{Void}, Int64))



####  GDS File  ####

# Create a GDS file
function create_gds(filename::AbstractString)
	id = @check ccall((:GDS_File_Create, libCoreArray), Cint, (Cstring,),
		filename)
	return type_file_gds(filename, id, false)
end


# Open an existing GDS file
function open_gds(filename::AbstractString, readonly::Bool=true)
	id = @check ccall((:GDS_File_Open, libCoreArray), Cint, (Cstring,Bool),
		filename, readonly)
	return type_file_gds(filename, id, readonly)
end


# Close the GDS file
function close_gds(file::type_file_gds)
	@check ccall((:GDS_File_Close, libCoreArray), Void, (Cint,),
		file.id)
	file.filename = ""
	file.id = -1
	file.readonly = true
	return nothing
end


# Synchronize the GDS file
function sync_gds(file::type_file_gds)
	@check ccall((:GDS_File_Sync, libCoreArray), Void, (Cint,),
		file.id)
	return nothing
end



####  GDS Node  ####

# Get the root of GDS file
function root_gdsn(file::type_file_gds)
	p = [ C_NULL ]
	id = @check ccall((:GDS_Node_Root, libCoreArray), Cint,
		(Cint, Ptr{Ptr{Void}}), file.id, pointer(p))
	return type_node_gds(id, p[1])
end


# Get the name of GDS node
function name_gdsn(obj::type_node_gds, fullname::Bool=false)
	ss = UTF8String[]
	@check ccall((:GDS_Node_Name, libCoreArray), Void,
		(Cint, Ptr{Void}, Bool, Ptr{Void}, Ptr{Void}),
		obj.id, obj.ptr, fullname, c_text_push, pointer_from_objref(ss))
	return ss[1]
end


# Rename the GDS node
function rename_gdsn(obj::type_node_gds, newname::AbstractString)
	@check ccall((:GDS_Node_Rename, libCoreArray), Void,
		(Cint, Ptr{Void}, Cstring), obj.id, obj.ptr, utf8(newname))
	return obj
end


# Get the name(s) of child node
function ls_gdsn(obj::type_node_gds, has_hidden::Bool=false)
	ss = UTF8String[]
	@check ccall((:GDS_Node_ListName, libCoreArray), Void,
		(Cint, Ptr{Void}, Bool, Ptr{Void}, Ptr{Void}),
		obj.id, obj.ptr, has_hidden, c_text_push, pointer_from_objref(ss))
	return ss
end


# Get a specified GDS node with path
function index_gdsn(obj::type_node_gds, path::AbstractString, silent::Bool=false)
	p = [ C_NULL ]
	id = @check ccall((:GDS_Node_Index, libCoreArray), Cint,
		(Cint, Ptr{Void}, Cstring, Bool, Ptr{Ptr{Void}}),
		obj.id, obj.ptr, utf8(path), silent, pointer(p))
	if p[1] != C_NULL
		return type_node_gds(id, p[1])
	else
		return nothing
	end
end

function index_gdsn(file::type_file_gds, path::AbstractString, silent::Bool=false)
	return index_gdsn(root_gdsn(file), path, silent)
end


# Get the folder node which contains the specified node
function getfolder_gdsn(obj::type_node_gds)
	p = [ C_NULL ]
	id = @check ccall((:GDS_GetFolder, libCoreArray), Cint,
		(Cint, Ptr{Void}, Ptr{Ptr{Void}}), obj.id, obj.ptr, pointer(p))
	if p[1] != C_NULL
		return type_node_gds(id, p[1])
	else
		return nothing
	end
end


# Get the descritpion of a specified node
function objdesp_gdsn(obj::type_node_gds)
	ss = UTF8String[]
	dm = Int64[]
	cpratio = Float64[1]
	size = Int64[1]

	flag = @check ccall((:GDS_NodeObjDesp, libCoreArray), Cint,
		(Cint, Ptr{Void}, Ptr{Float64}, Ptr{Int64}, Ptr{Void}, Ptr{Void},
			Ptr{Void}, Ptr{Void}),
		obj.id, obj.ptr, pointer(cpratio), pointer(size),
			c_text_push, pointer_from_objref(ss),
			c_int64_push, pointer_from_objref(dm))

	return type_info_gdsn(ss[1], ss[2], ss[3], ss[4], ss[5],
		(flag & 0x01) != 0, dm, ss[6], ss[7], cpratio[1], size[1],
		(flag & 0x02) != 0, (flag & 0x04) != 0, ss[8])
end





####  GDS Attributes  ####









####  Display  ####

function size_fmt(size::Int64)
	if size >= 1000^4
		return @sprintf("%.1f TB", size/(1000.0^4))
	elseif size >= 1000^3
		return @sprintf("%.1f GB", size/(1000.0^3))
	elseif size >= 1000^2
		return @sprintf("%.1f MB", size/(1000.0^2))
	elseif size >= 1000
		return @sprintf("%.1f KB", size/1000.0)
	elseif size > 1
		return @sprintf("%d bytes", size)
	else
		return @sprintf("%d byte", size)
	end
end


function show(io::IO, file::type_file_gds)
	size = @check ccall((:GDS_FileSize, libCoreArray), Clonglong, (Cint,),
		file.id)
	print_with_color(:bold, io, "File:")
	print_with_color(:black, io, " ", file.filename)
	print_with_color(:white, io, " (", size_fmt(size), ")")
	println(io)
	show(io, root_gdsn(file))
end


function show(io::IO, obj::type_node_gds)

	function enum(obj::type_node_gds, space::ASCIIString, level::Int,
		expand::Bool, fullname::Bool)

		d = objdesp_gdsn(obj)

		lText = ""; rText = ""
		if d.gds_type == "Label"
			lText = " "; rText = " "
		elseif d.gds_type == "VFolder"
			lText = ifelse(d.good, "[ -->", "[ -X-")
			rText = ifelse(d.good, "]", "]")
		elseif d.gds_type == "Folder"
			lText = "["; rText = "]"
		elseif d.gds_type == "Unknown"
			lText = "	-X-"; rText = ""
		else
			lText = "{"; rText = "}"
		end

		s = lText * " " * d.trait
		if d.gds_type == "Logical"
			s = s * ",logical"
		elseif d.gds_type == "Factor"
			s = s * ",factor"
		end
		# } else if ("R.class" %in% names(at))
		# {
		#	if (n$trait != "")
		#		s <- paste(s, BLURRED(","), sep="")
		#	if (!is.null(at$R.class))
		#	{
		#		s <- paste(s,
		#			BLURRED(paste(at$R.class, sep="", collapse=",")), sep="")
		#	}
		#}

		# show the dimension
		if length(d.dim) > 0
			s = s * " " * join(d.dim, "x")
		end

		# show compression
		if d.encoder != ""
			s = s * " " * d.encoder
			if isfinite(d.compression_ratio)
				s = s * @sprintf("(%0.2f%%)", 100*d.compression_ratio)
			end
		end

		if d.size >= 0
			s = s * ", " * size_fmt(d.size)
		end

		s = s * " " * rText
		# if (length(at) > 0L)
		#	s <- paste(s, rText, "*")
		#else
		#	s <- paste(s, " ", rText, sep="")

		print_with_color(:black, io, space, "+ ",
			ifelse(fullname, d.fullname, d.name), "   ")
		print_with_color(:white, io, s)
		println(io)
		for nm = ls_gdsn(obj)
			enum(index_gdsn(obj, nm), ifelse(level==1, "|--", "|  ") * space,
				level+1, expand, false)
		end

		return nothing
	end

	print_with_color(:bold, io, "")
	enum(obj, "", 1, true, true)
end

end
