// ===========================================================
//
// jugds.cpp: Julia Interface to CoreArray Genomic Data Structure (GDS) Files
//
// Copyright (C) 2017    Xiuwen Zheng
//
// jugds is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License Version 3 as
// published by the Free Software Foundation.
//
// jugds is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with jugds.
// If not, see <http://www.gnu.org/licenses/>.


#define COREARRAY_JULIA_PACKAGE

#include <J_GDS_CPP.h>
#include <string>
#include <set>
#include <map>


namespace jugds
{
	extern PdGDSFile PKG_GDS_Files[];
	extern vector<PdGDSObj> PKG_GDSObj_List;
	extern map<PdGDSObj, int> PKG_GDSObj_Map;
	extern int GetFileIndex(PdGDSFile file, bool throw_error=true);


	/// initialization and finalization
	class COREARRAY_DLL_LOCAL CInitNameObject
	{
	public:

		/// comparison of "const char *"
		struct strCmp {
			bool operator()(const char* s1, const char* s2) const
				{ return strcmp(s1, s2) < 0; }
		};

		/// mapping object
		map<const char*, const char*, strCmp> ClassMap;

		/// initialization
		CInitNameObject()
		{
			// ==============================================================
			// integer

			ClassMap["int8"  ] = TdTraits< C_Int8   >::StreamName();
			ClassMap["uint8" ] = TdTraits< C_UInt8  >::StreamName();
			ClassMap["int16" ] = TdTraits< C_Int16  >::StreamName();
			ClassMap["uint16"] = TdTraits< C_UInt16 >::StreamName();
			ClassMap["int32" ] = TdTraits< C_Int32  >::StreamName();
			ClassMap["uint32"] = TdTraits< C_UInt32 >::StreamName();
			ClassMap["int64" ] = TdTraits< C_Int64  >::StreamName();
			ClassMap["uint64"] = TdTraits< C_UInt64 >::StreamName();
			ClassMap["int24" ] = TdTraits< CoreArray::Int24  >::StreamName();
			ClassMap["uint24"] = TdTraits< CoreArray::UInt24 >::StreamName();

			ClassMap["bit1"] = TdTraits< CoreArray::BIT1 >::StreamName();
			ClassMap["bit2"] = TdTraits< CoreArray::BIT2 >::StreamName();
			ClassMap["bit3"] = TdTraits< CoreArray::BIT3 >::StreamName();
			ClassMap["bit4"] = TdTraits< CoreArray::BIT4 >::StreamName();
			ClassMap["bit5"] = TdTraits< CoreArray::BIT5 >::StreamName();
			ClassMap["bit6"] = TdTraits< CoreArray::BIT6 >::StreamName();
			ClassMap["bit7"] = TdTraits< CoreArray::BIT7 >::StreamName();
			ClassMap["bit8"] = TdTraits< C_UInt8         >::StreamName();

			ClassMap["bit9" ] = TdTraits< CoreArray::BIT9  >::StreamName();
			ClassMap["bit10"] = TdTraits< CoreArray::BIT10 >::StreamName();
			ClassMap["bit11"] = TdTraits< CoreArray::BIT11 >::StreamName();
			ClassMap["bit12"] = TdTraits< CoreArray::BIT12 >::StreamName();
			ClassMap["bit13"] = TdTraits< CoreArray::BIT13 >::StreamName();
			ClassMap["bit14"] = TdTraits< CoreArray::BIT14 >::StreamName();
			ClassMap["bit15"] = TdTraits< CoreArray::BIT15 >::StreamName();
			ClassMap["bit16"] = TdTraits< C_UInt16         >::StreamName();

			ClassMap["bit24"] = TdTraits< CoreArray::UInt24 >::StreamName();
			ClassMap["bit32"] = TdTraits< C_UInt32          >::StreamName();
			ClassMap["bit64"] = TdTraits< C_UInt64          >::StreamName();

			ClassMap["sbit2"] = TdTraits< CoreArray::SBIT2 >::StreamName();
			ClassMap["sbit3"] = TdTraits< CoreArray::SBIT3 >::StreamName();
			ClassMap["sbit4"] = TdTraits< CoreArray::SBIT4 >::StreamName();
			ClassMap["sbit5"] = TdTraits< CoreArray::SBIT5 >::StreamName();
			ClassMap["sbit6"] = TdTraits< CoreArray::SBIT6 >::StreamName();
			ClassMap["sbit7"] = TdTraits< CoreArray::SBIT7 >::StreamName();
			ClassMap["sbit8"] = TdTraits< C_Int8           >::StreamName();

			ClassMap["sbit9" ] = TdTraits< CoreArray::SBIT9  >::StreamName();
			ClassMap["sbit10"] = TdTraits< CoreArray::SBIT10 >::StreamName();
			ClassMap["sbit11"] = TdTraits< CoreArray::SBIT11 >::StreamName();
			ClassMap["sbit12"] = TdTraits< CoreArray::SBIT12 >::StreamName();
			ClassMap["sbit13"] = TdTraits< CoreArray::SBIT13 >::StreamName();
			ClassMap["sbit14"] = TdTraits< CoreArray::SBIT14 >::StreamName();
			ClassMap["sbit15"] = TdTraits< CoreArray::SBIT15 >::StreamName();
			ClassMap["sbit16"] = TdTraits< C_Int16           >::StreamName();

			ClassMap["sbit24"] = TdTraits< CoreArray::Int24 >::StreamName();
			ClassMap["sbit32"] = TdTraits< C_Int32          >::StreamName();
			ClassMap["sbit64"] = TdTraits< C_Int64          >::StreamName();


			// ==============================================================
			// Real number

			ClassMap["float32"] = TdTraits< C_Float32 >::StreamName();
			ClassMap["float64"] = TdTraits< C_Float64 >::StreamName();
			ClassMap["packedreal8"]  = TdTraits< TREAL8  >::StreamName();
			ClassMap["packedreal16"] = TdTraits< TREAL16 >::StreamName();
			ClassMap["packedreal24"] = TdTraits< TREAL24 >::StreamName();
			ClassMap["packedreal32"] = TdTraits< TREAL32 >::StreamName();


			// ==============================================================
			// String

			ClassMap["string"   ] = TdTraits< VARIABLE_LEN<C_UTF8>  >::StreamName();
			ClassMap["string16" ] = TdTraits< VARIABLE_LEN<C_UTF16> >::StreamName();
			ClassMap["string32" ] = TdTraits< VARIABLE_LEN<C_UTF32> >::StreamName();
			ClassMap["cstring"  ] = TdTraits< C_STRING<C_UTF8>  >::StreamName();
			ClassMap["cstring16"] = TdTraits< C_STRING<C_UTF16> >::StreamName();
			ClassMap["cstring32"] = TdTraits< C_STRING<C_UTF32> >::StreamName();
			ClassMap["fstring"  ] = TdTraits< FIXED_LEN<C_UTF8>  >::StreamName();
			ClassMap["fstring16"] = TdTraits< FIXED_LEN<C_UTF16> >::StreamName();
			ClassMap["fstring32"] = TdTraits< FIXED_LEN<C_UTF32> >::StreamName();


			// ==============================================================
			// R storage mode

			ClassMap["char"     ] = TdTraits< C_Int8  >::StreamName();
			ClassMap["raw"      ] = TdTraits< C_Int8  >::StreamName();
			ClassMap["int"      ] = TdTraits< C_Int32 >::StreamName();
			ClassMap["integer"  ] = TdTraits< C_Int32 >::StreamName();
			ClassMap["vl_int"   ] = TdTraits< TVL_Int >::StreamName();
			ClassMap["vl_uint"  ] = TdTraits< TVL_UInt >::StreamName();
			ClassMap["float"    ] = TdTraits< C_Float32 >::StreamName();
			ClassMap["numeric"  ] = TdTraits< C_Float64 >::StreamName();
			ClassMap["double"   ] = TdTraits< C_Float64 >::StreamName();
			ClassMap["character"] = TdTraits< VARIABLE_LEN<C_UTF8> >::StreamName();
			ClassMap["logical"  ] = TdTraits< C_Int32 >::StreamName();
			ClassMap["factor"   ] = TdTraits< C_Int32 >::StreamName();

			ClassMap["list"] = "$FOLDER$";
			ClassMap["folder"] = "$FOLDER$";
		}
	};
	
	static CInitNameObject Init;
}


using namespace std;
using namespace CoreArray;
using namespace jugds;


extern "C"
{

// error messages
static const char *ERR_NOT_FOLDER =
	"The GDS node is not a folder!";
static const char *ERR_READ_ONLY =
	"Read-only and please call 'compression.gdsn(node, \"\")' before writing.";
static const char *ERR_NO_DATA =
	"There is no data field.";


// ----------------------------------------------------------------------------
// Internal functions
// ----------------------------------------------------------------------------

/// file size to a string
static string fmt_size(double b)
{
	static const double TB = 1024.0*1024*1024*1024;
	static const double GB = 1024.0*1024*1024;
	static const double MB = 1024.0*1024;
	static const double KB = 1024.0;

	char s[256];
	if (b >= TB)
		FmtText(s, sizeof(s), "%.1fT", b/TB);
	else if (b >= GB)
		FmtText(s, sizeof(s), "%.1fG", b/GB);
	else if (b >= MB)
		FmtText(s, sizeof(s), "%.1fM", b/MB);
	else if (b >= KB)
		FmtText(s, sizeof(s), "%.1fK", b/KB);
	else
		FmtText(s, sizeof(s), "%gB", b);

	return string(s);
}


/// convert "(CdGDSObj*)  -->  PyObject*"
static void set_obj(CdGDSObj *Obj, int &outidx)
{
	static const char *ERR_OBJLIST = "Internal error in GDS node list.";

	if (!Obj)
		throw ErrGDSFmt("Invalid GDS object [NULL].");

	map<PdGDSObj, int>::iterator it = PKG_GDSObj_Map.find(Obj);
	if (it != PKG_GDSObj_Map.end())
	{
		outidx = it->second;
		if ((outidx < 0) || (outidx >= (int)PKG_GDSObj_Map.size()))
			throw ErrGDSFmt(ERR_OBJLIST);
		if (PKG_GDSObj_List[outidx] != Obj)
			throw ErrGDSFmt(ERR_OBJLIST);
	} else {
		vector<PdGDSObj>::iterator it =
			find(PKG_GDSObj_List.begin(), PKG_GDSObj_List.end(),
			(PdGDSObj)NULL);
		if (it != PKG_GDSObj_List.end())
		{
			outidx = it - PKG_GDSObj_List.begin();
			*it = Obj;
		} else {
			outidx = PKG_GDSObj_List.size();
			PKG_GDSObj_List.push_back(Obj);
		}
		PKG_GDSObj_Map[Obj] = outidx;
	}
}

static CdGDSObj* get_obj(int idx, void *ptr_int)
{
	static const char *ERR_GDS_OBJ  =
		"Invalid GDS node object!";
	static const char *ERR_GDS_OBJ2 =
		"Invalid GDS node object (it was closed or deleted).";

	CdGDSObj *ptr = (CdGDSObj *)ptr_int;
	// check
	if ((idx < 0) || (idx >= (int)PKG_GDSObj_List.size()))
		throw ErrGDSFmt(ERR_GDS_OBJ);
	if (ptr == NULL)
		throw ErrGDSFmt(ERR_GDS_OBJ);
	if (PKG_GDSObj_List[idx] != ptr)
		throw ErrGDSFmt(ERR_GDS_OBJ2);

	return ptr;
}



// ----------------------------------------------------------------------------
// File Operations
// ----------------------------------------------------------------------------

/// Create a GDS file
JL_DLLEXPORT int gdsCreateGDS(const char *fn, C_BOOL allow_dup)
{
	int file_id = -1;
	COREARRAY_TRY
		if (!allow_dup)
		{
			UTF8String FName = UTF8Text(fn);
			for (int i=0; i < PKG_MAX_NUM_GDS_FILES; i++)
			{
				if (PKG_GDS_Files[i])
				{
					if (PKG_GDS_Files[i]->FileName() == FName)
					{
						throw ErrGDSFmt(
							"The file '%s' has been created or opened.", fn);
					}
				}
			}
		}
		CdGDSFile *file = GDS_File_Create(fn);
		file_id = GetFileIndex(file);
	COREARRAY_CATCH
	return file_id;
}


/// Open an existing GDS file
JL_DLLEXPORT int gdsOpenGDS(const char *fn, C_BOOL readonly, C_BOOL allow_dup)
{
	int file_id = -1;
	COREARRAY_TRY
		if (!allow_dup)
		{
			UTF8String FName = UTF8Text(fn);
			for (int i=0; i < PKG_MAX_NUM_GDS_FILES; i++)
			{
				if (PKG_GDS_Files[i])
				{
					if (PKG_GDS_Files[i]->FileName() == FName)
					{
						throw ErrGDSFmt(
							"The file '%s' has been created or opened.", fn);
					}
				}
			}
		}
		CdGDSFile *file = GDS_File_Open(fn, readonly, true);
		file_id = GetFileIndex(file);
	COREARRAY_CATCH
	return file_id;
}


/// Close the GDS file
JL_DLLEXPORT void gdsCloseGDS(int file_id)
{
	COREARRAY_TRY
		if (file_id >= 0)
			GDS_File_Close(GDS_ID2File(file_id));
	COREARRAY_CATCH
}


/// Synchronize the GDS file
JL_DLLEXPORT void gdsSyncGDS(int file_id)
{
	COREARRAY_TRY
		GDS_ID2File(file_id)->SyncFile();
	COREARRAY_CATCH
}


/// Get the file size and check the file handler
JL_DLLEXPORT long long gdsFileSize(int file_id)
{
	long long sz;
	COREARRAY_TRY
		GDS_ID2File(file_id)->GetFileSize();
	COREARRAY_CATCH
	return sz;
}


/// Clean up fragments of a GDS file
JL_DLLEXPORT void gdsTidyUp(const char *fn, C_BOOL verbose)
{
	COREARRAY_TRY
		CdGDSFile file(fn, CdGDSFile::dmOpenReadWrite);
		C_Int64 old_s = file.GetFileSize();
		if (verbose)
		{
			printf("Clean up the fragments of GDS file:\n");
			printf("    open the file '%s' (%s)\n", fn, fmt_size(old_s).c_str());
			printf("    # of fragments: %d\n", file.GetNumOfFragment());
			printf("    save to '%s.tmp'\n", fn);
			fflush(stdout);
		}
		file.TidyUp(false);
		if (verbose)
		{
			C_Int64 new_s = file.GetFileSize();
			printf("    rename '%s.tmp' (%s, reduced: %s)\n", fn,
				fmt_size(new_s).c_str(), fmt_size(old_s-new_s).c_str());
			printf("    # of fragments: %d\n", file.GetNumOfFragment());
			fflush(stdout);
		}
	COREARRAY_CATCH
}


/// Get the root of a GDS file
JL_DLLEXPORT int gdsRoot(int file_id, PdGDSObj *PObj)
{
	int id = -1;
	COREARRAY_TRY
		CdGDSObj *Obj = &GDS_ID2File(file_id)->Root();
		set_obj(Obj, id);
		*PObj = Obj;
	COREARRAY_CATCH
	return id;
}



// ----------------------------------------------------------------------------
// File Structure Operations
// ----------------------------------------------------------------------------

/// Enumerate the names of its child nodes
JL_DLLEXPORT jl_array_t* gdsnListName(int node_id, PdGDSObj node,
	C_BOOL has_hidden)
{
	jl_array_t *rv_ans = NULL;
	COREARRAY_TRY

		CdGDSObj *Obj = get_obj(node_id, node);
		CdGDSAbsFolder *Dir = dynamic_cast<CdGDSAbsFolder*>(Obj);
		if (Dir)
		{
			vector<string> List;
			for (int i=0; i < Dir->NodeCount(); i++)
			{
				CdGDSObj *Obj = Dir->ObjItemEx(i);
				if (Obj)
				{
					if (has_hidden)
					{
						List.push_back(RawText(Obj->Name()));
					} else {
						if (!Obj->GetHidden() &&
							!Obj->Attribute().HasName(ASC16("R.invisible")))
						{
							List.push_back(RawText(Obj->Name()));
						}
					}
				}
			}

			jl_value_t *array_type = jl_apply_array_type(jl_string_type, 1);
			rv_ans = jl_alloc_array_1d(array_type, List.size());
			void **data = (void**)jl_array_data(rv_ans);
			for (size_t i=0; i < List.size(); i++)
			{
				jl_value_t *s = jl_pchar_to_string(List[i].c_str(), List[i].size());
				data[i] = s;
				jl_gc_wb(rv_ans, s);
			}
		} else {
			throw ErrGDSObj("It is not a folder.");
		}

	COREARRAY_CATCH
	return rv_ans;
}


/// Get the GDS node with a given path
JL_DLLEXPORT int gdsnIndex(int node_id, PdGDSObj node, const char *path,
	C_BOOL silent, PdGDSObj *PObj)
{
	int idx;
	COREARRAY_TRY
		CdGDSObj *Obj = get_obj(node_id, node);
		CdGDSAbsFolder *Dir = dynamic_cast<CdGDSAbsFolder*>(Obj);
		if (Dir)
		{
			Obj = Dir->PathEx(UTF16Text(path));
			if (!Obj && !silent)
				throw ErrGDSObj("No such GDS node \"%s\"!", path);
			if (Obj)
				set_obj(Obj, idx);
			else
				idx = -1;
			*PObj = Obj;
		} else {
			throw ErrGDSObj("It is not a folder.");
		}
	COREARRAY_CATCH
	return idx;
}


/// Get the name of a GDS node
JL_DLLEXPORT jl_value_t* gdsnName(int node_id, PdGDSObj node, C_BOOL full)
{
	jl_value_t *rv_ans = NULL;
	COREARRAY_TRY
		CdGDSObj *Obj = get_obj(node_id, node);
		string nm;
		if (full)
			nm = RawText(Obj->FullName());
		else
			nm = RawText(Obj->Name());
		rv_ans = jl_pchar_to_string(nm.c_str(), nm.size());
	COREARRAY_CATCH
	return rv_ans;
}


/// Get the name of a GDS node
JL_DLLEXPORT void gdsnRename(int node_id, PdGDSObj node, const char *newname)
{
	COREARRAY_TRY
		CdGDSObj *Obj = get_obj(node_id, node);
		Obj->SetName(UTF16Text(newname));
	COREARRAY_CATCH
}


/// Get the description of a GDS node
JL_DLLEXPORT jl_array_t* gdsnDesp(int node_id, PdGDSObj node,
	jl_array_t *dim, double *cratio, C_Int64 *size, C_BOOL *good, C_BOOL *hidden)
{
	jl_array_t *rv_ans = NULL;
	COREARRAY_TRY

		CdGDSObj *Obj = get_obj(node_id, node);

		string nm  = RawText(Obj->Name());
		string nm2 = RawText(Obj->FullName());
		string ste = Obj->dName();

		string tra = Obj->dTraitName();
		if (dynamic_cast<CdGDSVirtualFolder*>(Obj))
			tra = RawText(((CdGDSVirtualFolder*)Obj)->LinkFileName());

		string type = "Unknown";
		if (dynamic_cast<CdGDSLabel*>(Obj))
			type = "Label";
		else if (dynamic_cast<CdGDSFolder*>(Obj))
			type = "Folder";
		else if (dynamic_cast<CdGDSVirtualFolder*>(Obj))
			type = "VFolder";
		else if (dynamic_cast<CdGDSStreamContainer*>(Obj))
			type = "Raw";
		else if (dynamic_cast<CdContainer*>(Obj))
		{
			CdContainer* nn = static_cast<CdContainer*>(Obj);
			C_SVType sv = nn->SVType();
			if (COREARRAY_SV_INTEGER(sv))
			{
				if (GDS_Is_RFactor(Obj))
					type = "Factor";
				else if (GDS_Is_RLogical(Obj))
					type = "Logical";
				else
					type = "Integer";
			} else if (COREARRAY_SV_FLOAT(sv))
				type = "Real";
			else if (COREARRAY_SV_STRING(sv))
				type = "String";
		}

		// dim, the dimension of data field
		string encoder, compress;
		if (dynamic_cast<CdAbstractArray*>(Obj))
		{
			CdAbstractArray *_Obj = (CdAbstractArray*)Obj;
			int n = _Obj->DimCnt();
			// work around the mis data in julia (TODO)
			jl_value_t *v = jl_box_int64(0);
			JL_GC_PUSH1(&v);
			for (int i=0; i < n; i++) jl_array_ptr_1d_push(dim, v);
			JL_GC_POP();
			C_Int64 *p = (C_Int64*)jl_array_data(dim);
			for (int i=0; i < n; i++)
				p[i] = _Obj->GetDLen(n-i-1);

			if (_Obj->PipeInfo())
			{
				encoder = _Obj->PipeInfo()->Coder();
				compress = _Obj->PipeInfo()->CoderParam();
				if (_Obj->PipeInfo()->StreamTotalIn() > 0)
				{
					*cratio = (double)_Obj->PipeInfo()->StreamTotalOut() /
						_Obj->PipeInfo()->StreamTotalIn();
				}
			}
		}

		// object size
		if (dynamic_cast<CdContainer*>(Obj))
		{
			CdContainer* p = static_cast<CdContainer*>(Obj);
			p->Synchronize();
			*size = p->GDSStreamSize();
		} else if (dynamic_cast<CdGDSStreamContainer*>(Obj))
		{
			CdGDSStreamContainer *_Obj = (CdGDSStreamContainer*)Obj;
			if (_Obj->PipeInfo())
				*size = _Obj->PipeInfo()->StreamTotalIn();
			else
				*size = _Obj->GetSize();
		}

		// good
		*good = 1;
		if (dynamic_cast<CdGDSVirtualFolder*>(Obj))
		{
			CdGDSVirtualFolder *v = (CdGDSVirtualFolder*)Obj;
			*good = v->IsLoaded(true) ? 1 : 0;
		} else if (dynamic_cast<CdGDSUnknown*>(Obj))
		{
			*good = 0;
		}
		

		// hidden
		*hidden = Obj->GetHidden() ||
			Obj->Attribute().HasName(ASC16("R.invisible"));

		// message
		string msg;
		if (dynamic_cast<CdGDSVirtualFolder*>(Obj))
		{
			CdGDSVirtualFolder *v = (CdGDSVirtualFolder*)Obj;
			v->IsLoaded(true);
			msg = v->ErrMsg().c_str();
		}

		rv_ans = jl_alloc_array_1d(jl_apply_array_type(jl_string_type, 1), 8);
		jl_value_t *s;
		void **p = (void**)jl_array_data(rv_ans);
		p[0] = s = jl_pchar_to_string(nm.c_str(), nm.size());
		jl_gc_wb(rv_ans, s);
		p[1] = s = jl_pchar_to_string(nm2.c_str(), nm2.size());
		jl_gc_wb(rv_ans, s);
		p[2] = s = jl_pchar_to_string(ste.c_str(), ste.size());
		jl_gc_wb(rv_ans, s);
		p[3] = s = jl_pchar_to_string(tra.c_str(), tra.size());
		jl_gc_wb(rv_ans, s);
		p[4] = s = jl_pchar_to_string(type.c_str(), type.size());
		jl_gc_wb(rv_ans, s);
		p[5] = s = jl_pchar_to_string(encoder.c_str(), encoder.size());
		jl_gc_wb(rv_ans, s);
		p[6] = s = jl_pchar_to_string(compress.c_str(), compress.size());
		jl_gc_wb(rv_ans, s);
		p[7] = s = jl_pchar_to_string(msg.c_str(), msg.size());
		jl_gc_wb(rv_ans, s);

	COREARRAY_CATCH
	return rv_ans;
}



// ----------------------------------------------------------------------------
// Data Operations
// ----------------------------------------------------------------------------

/// Read data from a GDS node
JL_DLLEXPORT jl_array_t* gdsnRead(int node_id, PdGDSObj node,
	jl_array_t *start, jl_array_t *count, const char *cvt)
{
	// check the argument 'cvt'
	C_SVType sv;
	if (strcmp(cvt, "") == 0)
		sv = svCustom;
	else if (strcmp(cvt, "int8") == 0)
		sv = svInt8;
	else if (strcmp(cvt, "uint8") == 0)
		sv = svUInt8;
	else if (strcmp(cvt, "int16") == 0)
		sv = svInt16;
	else if (strcmp(cvt, "uint16") == 0)
		sv = svUInt16;
	else if (strcmp(cvt, "int32") == 0)
		sv = svInt32;
	else if (strcmp(cvt, "uint32") == 0)
		sv = svUInt32;
	else if (strcmp(cvt, "int64") == 0)
		sv = svInt64;
	else if (strcmp(cvt, "uint64") == 0)
		sv = svUInt64;
	else if (strcmp(cvt, "float32") == 0)
		sv = svFloat32;
	else if (strcmp(cvt, "float64") == 0)
		sv = svFloat64;
	else if (strcmp(cvt, "utf8") == 0)
		sv = svStrUTF8;
	else if (strcmp(cvt, "utf16") == 0)
		sv = svStrUTF16;
	else
		jl_error("Invalid 'cvt'.");

	// check the argument 'start'
	CdAbstractArray::TArrayDim dm_st;
	int dm_st_n = jl_array_len(start);
	{
		C_Int64 *p = (C_Int64*)jl_array_data(start);
		for (int i=0; i < dm_st_n; i++) dm_st[i] = p[i];
	}

	// check the argument 'count'
	CdAbstractArray::TArrayDim dm_cnt;
	int dm_cnt_n = jl_array_len(count);
	{
		C_Int64 *p = (C_Int64*)jl_array_data(count);
		for (int i=0; i < dm_cnt_n; i++) dm_cnt[i] = p[i];
	}

	if ((dm_st_n==0 && dm_cnt_n>0) || (dm_st_n>0 && dm_cnt_n==0))
		jl_error("'start' and 'count' should be both None.");


	COREARRAY_TRY

		CdGDSObj *obj = get_obj(node_id, node);
		CdAbstractArray *Obj = dynamic_cast<CdAbstractArray*>(obj);
		if (Obj == NULL)
			throw ErrGDSFmt(ERR_NO_DATA);

		C_Int32 *pDS=NULL, *pDL=NULL;
		if (dm_st_n>0 && dm_cnt_n>0)
		{
			int Len = Obj->DimCnt();
			CdAbstractArray::TArrayDim DCnt;
			Obj->GetDim(DCnt);

			if (dm_st_n != Len)
				throw ErrGDSFmt("The length of 'start' is invalid.");
			for (int i=0; i < Len; i++)
			{
				if ((dm_st[i] < 0) || (dm_st[i] >= DCnt[i]))
					throw ErrGDSFmt("'start' is invalid.");
			}
			pDS = dm_st;

			if (dm_cnt_n != Len)
				throw ErrGDSFmt("The length of 'count' is invalid.");
			for (int i=0; i < Len; i++)
			{
				int &v = dm_cnt[i];
				if (v == -1)
					v = DCnt[i] - dm_st[i];
				if ((v <= 0) || ((dm_st[i]+v) >= DCnt[i]))
					throw ErrGDSFmt("'count' is invalid.");
			}
			pDL = dm_cnt;
		}

		return GDS_JArray_Read(Obj, pDS, pDL, NULL, sv);

	COREARRAY_CATCH
	return NULL;
}


/*
// ----------------------------------------------------------------------------
// Attribute Operations
// ----------------------------------------------------------------------------

JL_DLLEXPORT PyObject* any2obj(CdAny &Obj)
{
	if (Obj.IsInt())
	{
		return PyInt_FromLong(Obj.GetInt32());
	} else if (Obj.IsFloat())
	{
		return PyFloat_FromDouble(Obj.GetFloat64());
	} else if (Obj.IsString())
	{
		const UTF8String &s = Obj.GetStr8();
		return PYSTR_SET2(s.c_str(), s.size());
	} else if (Obj.IsBool())
	{
		return PyBool_FromLong(Obj.GetBool() ? 1 : 0);
	} else if (Obj.IsArray())
	{
		const size_t n = Obj.GetArrayLength();
		CdAny *p = Obj.GetArray();
		PyObject *rv_ans = PyList_New(n);
		for (size_t i=0; i < n; i++)
			PyList_SetItem(rv_ans, i, any2obj(*p++));
		return rv_ans;
	} else
		Py_RETURN_NONE;
}

/// Get the attribute(s) of a GDS node
JL_DLLEXPORT PyObject* gdsnGetAttr(PyObject *self, PyObject *args)
{
	int nidx;
	Py_ssize_t ptr_int;
	if (!PyArg_ParseTuple(args, "in", &nidx, &ptr_int))
		return NULL;

	COREARRAY_TRY
		CdGDSObj *Obj = get_obj(nidx, ptr_int);
		if (Obj->Attribute().Count() > 0)
		{
			const size_t n = Obj->Attribute().Count();
			PyObject *rv_ans = PyDict_New();
			for (size_t i=0; i < n; i++)
			{
				PyObject *x = any2obj(Obj->Attribute()[i]);
				PyDict_SetItemString(rv_ans,
					RawText(Obj->Attribute().Names(i)).c_str(), x);
				Py_DECREF(x);
			}
			return rv_ans;
		}
	COREARRAY_CATCH_NONE
}

*/


// ----------------------------------------------------------------------------
// Register routines
// ----------------------------------------------------------------------------

extern void *GDS_C_API;

/// Initialize
JL_DLLEXPORT void* GDS_Init()
{
	// to register CoreArray classes and objects
	RegisterClass();
	return GDS_C_API;
}

}
