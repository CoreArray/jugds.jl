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
		sync_gds, cleanup_gds,
		root_gdsn, name_gdsn, rename_gdsn, ls_gdsn, index_gdsn, getfolder_gdsn,
		delete_gdsn, objdesp_gdsn,
		put_attr_gdsn, get_attr_gdsn, delete_attr_gdsn



####  Open and initialize the CoreArray Binary Library  ####

@linux_only	  libfn = "libCoreArray.so"
@osx_only	  libfn = "libCoreArray.dylib"
@windows_only libfn = "libCoreArray.dll"

global libname = joinpath(Pkg.dir(), "JuGDS", "deps", libfn)

if !isfile(libname)
	error("The CoreArray library cannot be found; it may not have been built correctly.\nTry Pkg.build(\"JuGDS\").")
end

const LibCoreArray = libname

function __init__()
	ccall((:GDS_Init, LibCoreArray), Void, ())
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

function error_check()
	s = ccall((:GDS_Error, LibCoreArray), Ptr{UInt8}, ())
	if s != C_NULL
		error(bytestring(s))
	end
	nothing
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
	id = ccall((:GDS_File_Create, LibCoreArray), Cint, (Cstring,), filename)
	error_check()
	return type_file_gds(filename, id, false)
end


# Open an existing GDS file
function open_gds(filename::AbstractString, readonly::Bool=true)
	id = ccall((:GDS_File_Open, LibCoreArray), Cint, (Cstring,Bool),
		filename, readonly)
	error_check()
	return type_file_gds(filename, id, readonly)
end


# Close the GDS file
function close_gds(file::type_file_gds)
	ccall((:GDS_File_Close, LibCoreArray), Void, (Cint,), file.id)
	error_check()
	file.filename = ""
	file.id = -1
	file.readonly = true
	return nothing
end


# Synchronize the GDS file
function sync_gds(file::type_file_gds)
	ccall((:GDS_File_Sync, LibCoreArray), Void, (Cint,), file.id)
	error_check()
	return nothing
end


# Clean up fragments of a GDS file
function cleanup_gds(filename::AbstractString, verbose::Bool=true)
	f = open_gds(filename, false)
	try
		if verbose
			size = ccall((:GDS_FileSize, LibCoreArray), Clonglong, (Cint,), f.id)
			error_check()
			num  = ccall((:GDS_NumOfFragment, LibCoreArray), Cint, (Cint,), f.id)
			error_check()
			println("Clean up the fragments of GDS file:")
			println("\topen the file \"$filename\" (size: $size)")
			println("\t# of fragments in total: $num")
			fn = filename * ".tmp"
			println("\tsave it to \"$fn\"")
		end

		ccall((:GDS_TidyUp, LibCoreArray), Void, (Cint,), f.id)
		error_check()

		if verbose
			size = ccall((:GDS_FileSize, LibCoreArray), Clonglong, (Cint,), f.id)
			error_check()
			num  = ccall((:GDS_NumOfFragment, LibCoreArray), Cint, (Cint,), f.id)
			error_check()
			println("\trename \"$fn\" (size: $size)")
			println("\t# of fragments in total: $num")
		end
	finally
		close_gds(f)
	end
	return nothing
end



####  GDS Node  ####

# Get the root of GDS file
function root_gdsn(file::type_file_gds)
	p = Ref{Ptr{Void}}(C_NULL)
	id = ccall((:GDS_Node_Root, LibCoreArray), Cint, (Cint, Ref{Ptr{Void}}),
		file.id, p)
	error_check()
	return type_node_gds(id, p[])
end


# Get the name of GDS node
function name_gdsn(obj::type_node_gds, fullname::Bool=false)
	ss = UTF8String[]
	ccall((:GDS_Node_Name, LibCoreArray), Void,
		(Cint, Ptr{Void}, Bool, Ptr{Void}, Ptr{Void}),
		obj.id, obj.ptr, fullname, c_text_push, pointer_from_objref(ss))
	error_check()
	return ss[1]
end


# Rename the GDS node
function rename_gdsn(obj::type_node_gds, newname::AbstractString)
	ccall((:GDS_Node_Rename, LibCoreArray), Void,
		(Cint, Ptr{Void}, Cstring), obj.id, obj.ptr, utf8(newname))
	error_check()
	return obj
end


# Get the name(s) of child node
function ls_gdsn(obj::type_node_gds, has_hidden::Bool=false)
	ss = UTF8String[]
	ccall((:GDS_Node_ListName, LibCoreArray), Void,
		(Cint, Ptr{Void}, Bool, Ptr{Void}, Ptr{Void}),
		obj.id, obj.ptr, has_hidden, c_text_push, pointer_from_objref(ss))
	error_check()
	return ss
end


# Get a specified GDS node with path
function index_gdsn(obj::type_node_gds, path::AbstractString, silent::Bool=false)
	p = Ref{Ptr{Void}}(C_NULL)
	id = ccall((:GDS_Node_Index, LibCoreArray), Cint,
		(Cint, Ptr{Void}, Cstring, Bool, Ref{Ptr{Void}}),
		obj.id, obj.ptr, utf8(path), silent, p)
	error_check()
	if p[] != C_NULL
		return type_node_gds(id, p[])
	else
		return nothing
	end
end

function index_gdsn(file::type_file_gds, path::AbstractString, silent::Bool=false)
	return index_gdsn(root_gdsn(file), path, silent)
end


# Get the folder node which contains the specified node
function getfolder_gdsn(obj::type_node_gds)
	p = Ref{Ptr{Void}}(C_NULL)
	id = ccall((:GDS_GetFolder, LibCoreArray), Cint,
		(Cint, Ptr{Void}, Ref{Ptr{Void}}), obj.id, obj.ptr, p)
	error_check()
	if p[] != C_NULL
		return type_node_gds(id, p[])
	else
		return nothing
	end
end


# Delete a specified node
function delete_gdsn(obj::type_node_gds, force::Bool=false)
	ccall((:GDS_DeleteNode, LibCoreArray), Void,
		(Cint, Ptr{Void}, Bool), obj.id, obj.ptr, force)
	error_check()
	return nothing
end


# Get the descritpion of a specified node
function objdesp_gdsn(obj::type_node_gds)
	ss = UTF8String[]
	dm = Int64[]
	cpratio = Ref{Float64}(NaN)
	size = Ref{Int64}(-1)

	flag = ccall((:GDS_NodeObjDesp, LibCoreArray), Cint,
		(Cint, Ptr{Void}, Ref{Float64}, Ref{Int64}, Ptr{Void}, Ptr{Void},
			Ptr{Void}, Ptr{Void}),
		obj.id, obj.ptr, cpratio, size, c_text_push, pointer_from_objref(ss),
			c_int64_push, pointer_from_objref(dm))
	error_check()

	return type_info_gdsn(ss[1], ss[2], ss[3], ss[4], ss[5],
		(flag & 0x01) != 0, dm, ss[6], ss[7], cpratio[], size[],
		(flag & 0x02) != 0, (flag & 0x04) != 0, ss[8])
end



####  GDS Attributes  ####

# Add an attribute to a GDS node
function put_attr_gdsn(obj::type_node_gds, name::AbstractString, val)
	return nothing
end


# Get the attributes of a GDS node
function get_attr_gdsn(obj::type_node_gds)
	return nothing
end

# Remove an attribute from a GDS node
function delete_attr_gdsn(obj::type_node_gds, name::AbstractString)
	ccall((:GDS_DeleteAttr, LibCoreArray), Void,
		(Cint, Ptr{Void}, Cstring), obj.id, obj.ptr, utf8(name))
	error_check()
	return nothing
end






####  Data Operations  ####






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

function enum_node(io::IO, obj::type_node_gds, space::ASCIIString, level::Int,
	all::Bool, expand::Bool, fullname::Bool)

	d = objdesp_gdsn(obj)
	if !all
		if d.hidden
			return nothing
		end
	end

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

	if expand
		for nm = ls_gdsn(obj)
			enum_node(io, index_gdsn(obj, nm),
				ifelse(level==1, "|--", "|  ") * space,
				level+1, all, expand, false)
		end
	end

	return nothing
end


function show(io::IO, file::type_file_gds, all=false)
	size = ccall((:GDS_FileSize, LibCoreArray), Clonglong, (Cint,), file.id)
	error_check()
	print_with_color(:bold, io, "File:")
	print_with_color(:black, io, " ", file.filename)
	print_with_color(:white, io, " (", size_fmt(size), ")")
	println(io)
	show(io, root_gdsn(file), all)
end


function show(io::IO, obj::type_node_gds, all=false, expand=true)
	print_with_color(:bold, io, "")
	enum_node(io, obj, "", 1, all, expand, true)
end

end
