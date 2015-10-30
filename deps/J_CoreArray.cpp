// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// J_CoreArray.cpp: Export the C routines of CoreArray
//
// Copyright (C) 2015    Xiuwen Zheng
//
// This file is part of CoreArray.
//
// CoreArray is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License Version 3 as
// published by the Free Software Foundation.
//
// CoreArray is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with CoreArray.
// If not, see <http://www.gnu.org/licenses/>.

/**
 *	\file     J_CoreArray.cpp
 *	\author   Xiuwen Zheng [zhengx@u.washington.edu]
 *	\version  1.0
 *	\date     2015
 *	\brief    Export the C routines of CoreArray
 *	\details
**/

#include <CoreArray.h>

#define COREARRAY_JUGDS_PACKAGE
#include "J_GDS.h"


namespace JuGDS
{
	using namespace std;
	using namespace CoreArray;


	/// error exception for the gdsfmt package
	class COREARRAY_DLL_EXPORT ErrJuGDS: public ErrCoreArray
	{
	public:
		ErrJuGDS() {}
		ErrJuGDS(const char *fmt, ...) { _COREARRAY_ERRMACRO_(fmt); }
		ErrJuGDS(const std::string &msg) { fMessage = msg; }
	};


	/// the maximum number of GDS files
	#define JUGDS_MAX_NUM_GDS_FILES    256

	/// a list of GDS files in the gdsfmt package
	COREARRAY_DLL_LOCAL PdGDSFile JuGDS_GDS_Files[JUGDS_MAX_NUM_GDS_FILES];

	/// get the index in 'JuGDS_GDS_Files' for NULL
	COREARRAY_DLL_LOCAL int GetEmptyFileIndex(bool throw_error=true)
	{
		for (int i=0; i < JUGDS_MAX_NUM_GDS_FILES; i++)
		{
			if (JuGDS_GDS_Files[i] == NULL)
				return i;
		}
		if (throw_error)
		{
			throw ErrJuGDS(
				"You have opened %d GDS files, and no more is allowed!",
				JUGDS_MAX_NUM_GDS_FILES);
		}
		return -1;
	}

	/// get the index in 'JuGDS_GDS_Files' for file
	COREARRAY_DLL_LOCAL int GetFileIndex(PdGDSFile file, bool throw_error=true)
	{
		for (int i=0; i < JUGDS_MAX_NUM_GDS_FILES; i++)
		{
			if (JuGDS_GDS_Files[i] == file)
				return i;
		}
		if (throw_error)
		{
			throw ErrJuGDS(
				"The GDS file has been closed, or invalid.");
		}
		return -1;
	}


	/// a list of GDS objects
	COREARRAY_DLL_LOCAL vector<PdGDSObj> JuGDS_GDSObj_List;

	/// mapping from GDS objects to indices
	COREARRAY_DLL_LOCAL map<PdGDSObj, int> JuGDS_GDSObj_Map;


	/// initialization and finalization
	class COREARRAY_DLL_LOCAL CInitObject
	{
	public:
		/// initialization
		CInitObject()
		{
			memset(JuGDS_GDS_Files, 0, sizeof(JuGDS_GDS_Files));
			JuGDS_GDSObj_List.reserve(1024);
		}

		/// finalization
		~CInitObject()
		{
			JuGDS_GDSObj_List.clear();
			JuGDS_GDSObj_Map.clear();

			for (int i=0; i < JUGDS_MAX_NUM_GDS_FILES; i++)
			{
				PdGDSFile file = JuGDS_GDS_Files[i];
				if (file != NULL)
				{
					try {
						JuGDS_GDS_Files[i] = NULL;
						delete file;
					}
					catch (...) { }
				}
			}
		}
	};

	static CInitObject InitObject;
}

using namespace std;
using namespace CoreArray;
using namespace JuGDS;



// ===========================================================================
// ===========================================================================
// Functions with try and catch, usually called by JuGDS.jl
// ===========================================================================
// ===========================================================================

extern "C"
{

/// the flag of error
COREARRAY_DLL_LOCAL bool has_error;
/// the error message
COREARRAY_DLL_LOCAL string error_info;

/// try block for CoreArray library
#define CORE_TRY    \
	has_error = false; \
	try {

/// catch block for CoreArray library
#define CORE_CATCH    \
	} \
	catch (exception &E) { \
		has_error = true; error_info = E.what(); \
	} \
	catch (const char *E) { \
		has_error = true; error_info = E; \
	} \
	catch (...) { \
		has_error = true; error_info = ERR_UNKNOWN; \
	}


/// text interactive functions
#define TEXT_PUSH  \
	void (*push_text)(void *julia, const char *txt, size_t txt_len)

#define TEXT_SET   \
	void (*set_text)(void *julia, ssize_t index, const char *txt, size_t txt_len)

/// integer interactive functions
#define INT64_PUSH  \
	void (*push_int64)(void *julia, C_Int64 val)





// ===========================================================================
// Error Messages

static const char *ERR_NOT_FOLDER = "The GDS node is not a folder!";
static const char *ERR_UNKNOWN = "Unknown error!";


// ===========================================================================
// Functions for Error

COREARRAY_DLL_EXPORT const char *GDS_Error()
{
	const char *rv = (has_error) ? error_info.c_str() : NULL;
	has_error = false;
	return rv;
}


// ===========================================================================
// Initialization

/// Initialize
COREARRAY_DLL_EXPORT void GDS_Init()
{
	CORE_TRY
		// to register CoreArray classes and objects
		RegisterClass();
	CORE_CATCH
}



// ===========================================================================
// Functions for GDS File

COREARRAY_INLINE static PdGDSFile GetGDSFile(int gds_id)
{
	static const char *ERR_GDS_File = "Invalid GDS file (ID: %d).";
	// check
	if ((gds_id < 0) || (gds_id >= JUGDS_MAX_NUM_GDS_FILES))
		throw ErrJuGDS(ERR_GDS_File, gds_id);
	PdGDSFile rv = JuGDS_GDS_Files[gds_id];
	if (rv == NULL)
		throw ErrJuGDS(ERR_GDS_File, gds_id);
	return rv;
}


/// Create a GDS file
COREARRAY_DLL_EXPORT int GDS_File_Create(const char *FileName)
{
	int gds_id = -1;
	CORE_TRY
		int i = GetEmptyFileIndex();
		PdGDSFile file = NULL;
		try {
			file = new CdGDSFile;
			file->SaveAsFile(FileName);
			JuGDS_GDS_Files[i] = file;
			gds_id = i;
		}
		catch (exception &E) {
			if (file) delete file;
			throw;
		}
		catch (const char *E) {
			if (file) delete file;
			throw;
		}
		catch (...) {
			if (file) delete file;
			throw;
		}
	CORE_CATCH
	return gds_id;
}


/// Open an existing GDS file
COREARRAY_DLL_EXPORT int GDS_File_Open(const char *FileName, C_BOOL ReadOnly)
{
	int gds_id = -1;
	CORE_TRY
		int id = GetEmptyFileIndex();
		PdGDSFile file = NULL;
		try {
			file = new CdGDSFile;
			file->LoadFileFork(FileName, ReadOnly!=0);
			JuGDS_GDS_Files[id] = file;
			gds_id = id;
		}
		catch (exception &E) {
			string Msg = E.what();
			if ((file!=NULL) && !file->Log().List().empty())
			{
				Msg.append(sLineBreak);
				Msg.append("Log:");
				for (size_t i=0; i < file->Log().List().size(); i++)
				{
					Msg.append(sLineBreak);
					Msg.append(RawText(file->Log().List()[i].Msg));
				}
			}
			if (file) delete file;
			throw ErrJuGDS(Msg);
		}
		catch (const char *E) {
			string Msg = E;
			if ((file!=NULL) && !file->Log().List().empty())
			{
				Msg.append(sLineBreak);
				Msg.append("Log:");
				for (size_t i=0; i < file->Log().List().size(); i++)
				{
					Msg.append(sLineBreak);
					Msg.append(RawText(file->Log().List()[i].Msg));
				}
			}
			if (file) delete file;
			throw ErrJuGDS(Msg);
		}
		catch (...) {
			if (file) delete file;
			throw;
		}
	CORE_CATCH
	return gds_id;
}


/// Close a GDS file
COREARRAY_DLL_EXPORT void GDS_File_Close(int gds_id)
{
	CORE_TRY
		PdGDSFile File = NULL;
		if ((0 <= gds_id) && (gds_id < JUGDS_MAX_NUM_GDS_FILES))
			File = JuGDS_GDS_Files[gds_id];
		if (File)
		{
			JuGDS_GDS_Files[gds_id] = NULL;

			// delete GDS objects in JuGDS_GDSObj_List and JuGDS_GDSObj_Map
			vector<PdGDSObj>::iterator p = JuGDS_GDSObj_List.begin();
			for (; p != JuGDS_GDSObj_List.end(); p++)
			{
				if (*p != NULL)
				{
					// for a virtual folder
					PdGDSObj Obj = *p;
					PdGDSFolder Folder = Obj->Folder();
					while (Folder != NULL)
					{
						Obj = Folder;
						Folder = Obj->Folder();
					}
					// Obj is the root, and then get the GDS file
					if (Obj->GDSFile() == File)
					{
						JuGDS_GDSObj_Map.erase(*p);
						*p = NULL;
					}
				}
			}
			if (File) delete File;
		} else
			throw ErrJuGDS("Invalid GDS file (ID: %d).", gds_id);
	CORE_CATCH
}


/// Synchronize the GDS file
COREARRAY_DLL_EXPORT void GDS_File_Sync(int gds_id)
{
	CORE_TRY
		GetGDSFile(gds_id)->SyncFile();
	CORE_CATCH
}


/// Get the file size and check the file handler
COREARRAY_DLL_EXPORT C_Int64 GDS_FileSize(int gds_id)
{
	C_Int64 rv;
	CORE_TRY
		rv = GetGDSFile(gds_id)->GetFileSize();
	CORE_CATCH
	return rv;
}


/// Get the number of fragments
COREARRAY_DLL_EXPORT int GDS_NumOfFragment(int gds_id)
{
	int rv;
	CORE_TRY
		rv = GetGDSFile(gds_id)->GetNumOfFragment();
	CORE_CATCH
	return rv;
}


/// Clean up fragments of a GDS file
COREARRAY_DLL_EXPORT void GDS_TidyUp(int gds_id)
{
	CORE_TRY
		GetGDSFile(gds_id)->TidyUp(false);
	CORE_CATCH
}



// ===========================================================================
// Functions for File Structure

/// Check the validity of GDS node
COREARRAY_INLINE static int StoreGDSObj(PdGDSObj Obj)
{
	static const char *ERR_GDSOBJ  = "Internal error in 'StoreGDSObj'.";
	static const char *ERR_GDSOBJ2 = "Invalid GDS object (=NULL).";

	if (Obj != NULL)
	{
		int idx = -1;
		map<PdGDSObj, int>::iterator it = JuGDS_GDSObj_Map.find(Obj);
		if (it != JuGDS_GDSObj_Map.end())
		{
			idx = it->second;
			if ((idx < 0) || (idx >= (int)JuGDS_GDSObj_List.size()))
				throw ErrJuGDS(ERR_GDSOBJ);
			if (JuGDS_GDSObj_List[idx] != Obj)
				throw ErrJuGDS(ERR_GDSOBJ);
		} else {
			vector<PdGDSObj>::iterator it =
				find(JuGDS_GDSObj_List.begin(), JuGDS_GDSObj_List.end(),
				(PdGDSObj)NULL);
			if (it != JuGDS_GDSObj_List.end())
			{
				idx = it - JuGDS_GDSObj_List.begin();
				*it = Obj;
			} else {
				idx = JuGDS_GDSObj_List.size();
				JuGDS_GDSObj_List.push_back(Obj);
			}
			JuGDS_GDSObj_Map[Obj] = idx;
		}
		return idx;
	} else
		throw ErrJuGDS(ERR_GDSOBJ2);
}

/// Check the validity of R SEXP
COREARRAY_INLINE static void CheckGDSObj(int idx, PdGDSObj Obj, bool ReadOnly)
{
	static const char *ERR_GDS_OBJ  =
		"Invalid GDS node object!";
	static const char *ERR_GDS_OBJ2 =
		"Invalid GDS node object (it was closed or deleted).";

	// check
	if ((idx < 0) || (idx >= (int)JuGDS_GDSObj_List.size()))
		throw ErrJuGDS(ERR_GDS_OBJ);
	if (JuGDS_GDSObj_List[idx] != Obj)
		throw ErrJuGDS(ERR_GDS_OBJ2);

	CdGDSFile *file = Obj->GDSFile();
	if (file->ReadOnly() && !ReadOnly)
		throw ErrJuGDS("The GDS file is read-only.");
}


/// Get the root of a GDS file
COREARRAY_DLL_EXPORT int GDS_Node_Root(int gds_id, PdGDSObj *PObj)
{
	int id = -1;
	CORE_TRY
		PdGDSObj Obj = &(GetGDSFile(gds_id)->Root());
		id = StoreGDSObj(Obj);
		*PObj = Obj;
	CORE_CATCH
	return id;
}


/// Get the name of a GDS node
COREARRAY_DLL_EXPORT void GDS_Node_Name(int node_id, PdGDSObj node,
	C_BOOL full, TEXT_PUSH, void *julia)
{
	CORE_TRY
		CheckGDSObj(node_id, node, true);
		string s = (full == 0) ?
			RawText(node->Name()) : RawText(node->FullName());
		(*push_text)(julia, &s[0], s.size());
	CORE_CATCH
}


/// Rename a GDS node
COREARRAY_DLL_EXPORT void GDS_Node_Rename(int node_id, PdGDSObj node,
	const char *newname)
{
	CORE_TRY
		CheckGDSObj(node_id, node, false);
		node->SetName(UTF16Text(newname));
	CORE_CATCH
}


/// Get a name list of child node(s)
COREARRAY_DLL_EXPORT void GDS_Node_ListName(int node_id, PdGDSObj node,
	C_BOOL has_hidden, TEXT_PUSH, void *julia)
{
	CORE_TRY
		CheckGDSObj(node_id, node, true);
		CdGDSAbsFolder *Dir = dynamic_cast<CdGDSAbsFolder*>(node);
		if (Dir)
		{
			for (int i=0; i < Dir->NodeCount(); i++)
			{
				CdGDSObj *Obj = Dir->ObjItemEx(i);
				if (Obj)
				{
					if (has_hidden)
					{
						string s = RawText(Obj->Name());
						(*push_text)(julia, &s[0], s.size());
					} else {
						if (!Obj->Attribute().HasName(ASC16("R.invisible")))
						{
							string s = RawText(Obj->Name());
							(*push_text)(julia, &s[0], s.size());
						}
					}
				}
			}
		}
	CORE_CATCH
}


/// Get the node according to the path
COREARRAY_DLL_EXPORT int GDS_Node_Index(int node_id, PdGDSObj node,
	const char *path, C_BOOL silent, PdGDSObj *PObj)
{
	int id = -1;
	*PObj = NULL;
	CORE_TRY

		CheckGDSObj(node_id, node, true);
		if (!dynamic_cast<CdGDSAbsFolder*>(node))
		{
			string pn = RawText(node->FullName());
			if (pn.empty()) pn = "$ROOT$";
			throw ErrGDSFile("'%s' is not a folder.", pn.c_str());
		}

		CdGDSAbsFolder &Dir = *((CdGDSAbsFolder*)node);
		node = Dir.PathEx(UTF16Text(path));
		if (!node && !silent)
			throw ErrJuGDS("No such GDS node \"%s\"!", path);

		id = StoreGDSObj(node);
		*PObj = node;

	CORE_CATCH
	return id;
}


/// Get the folder node which contains the specified node
COREARRAY_DLL_EXPORT int GDS_GetFolder(int node_id, PdGDSObj node,
	PdGDSObj *PObj)
{
	int id = -1;
	CORE_TRY
		CheckGDSObj(node_id, node, true);
		*PObj = node = node->Folder();
		if (node)
			id = StoreGDSObj(node);
	CORE_CATCH
	return id;
}


/// Delete a node
COREARRAY_DLL_EXPORT void GDS_DeleteNode(int node_id, PdGDSObj node,
	C_BOOL Force)
{
	CORE_TRY
		CheckGDSObj(node_id, node, true);

		vector<C_BOOL> DeleteArray;
		if (dynamic_cast<CdGDSAbsFolder*>(node))
		{
			DeleteArray.resize(JuGDS_GDSObj_List.size(), false);
			size_t idx = 0;
			vector<PdGDSObj>::iterator p = JuGDS_GDSObj_List.begin();
			for (; p != JuGDS_GDSObj_List.end(); p++)
			{
				if (*p != NULL)
				{
					if (static_cast<CdGDSAbsFolder*>(node)->HasChild(*p, true))
						DeleteArray[idx] = true;
				}
				idx ++;
			}
		}

		if (node->Folder())
			node->Folder()->DeleteObj(node, Force);
		else
			throw ErrJuGDS("Can not delete the root.");

		// delete GDS objects in GDSFMT_GDSObj_List and GDSFMT_GDSObj_Map
		vector<PdGDSObj>::iterator p = JuGDS_GDSObj_List.begin();
		for (; p != JuGDS_GDSObj_List.end(); p++)
			if (*p == node) *p = NULL;
		JuGDS_GDSObj_Map.erase(node);

		if (!DeleteArray.empty())
		{
			size_t idx = 0;
			vector<C_BOOL>::iterator p = DeleteArray.begin();
			for (; p != DeleteArray.end(); p++)
			{
				if (*p)
				{
					PdGDSObj &Obj = JuGDS_GDSObj_List[idx];
					JuGDS_GDSObj_Map.erase(Obj);
					Obj = NULL;
				}
				idx ++;
			}
		}

	CORE_CATCH
}


/// Get the description of a GDS node
COREARRAY_DLL_EXPORT int GDS_NodeObjDesp(int node_id, PdGDSObj node,
	double *CompressionRatio, C_Int64 *Size,
	TEXT_PUSH, void *txt, INT64_PUSH, void *intarray)
{
	int flag = 0;
	CORE_TRY

		CheckGDSObj(node_id, node, true);
		UTF8String s;

		#define PUSH_STRING(str)  \
			{ s = UTF8Text(str); (*push_text)(txt, &s[0], s.size()); }
		#define PUSH_INT(i)  \
			(*push_int64)(intarray, i);

		// 1: name
		PUSH_STRING(node->Name());
		// 2: full name
		PUSH_STRING(node->FullName());
		// 3: storage, the stream name of data field, such like "dInt32"
		PUSH_STRING(node->dName());

		// 4: trait, the description of data field, such like "Int32"
		s = UTF8Text(node->dTraitName());
		if (dynamic_cast<CdGDSVirtualFolder*>(node))
			s = UTF8Text(((CdGDSVirtualFolder*)node)->LinkFileName());
		PUSH_STRING(s);

		// 5: type (a factor)
		static const char *FactorText[] = {
			"Label", "Folder", "VFolder", "Raw", "Integer", "Factor",
			"Logical", "Real", "String", "Unknown" };
		int TypeInt = 10; // unknown
		if (dynamic_cast<CdGDSLabel*>(node))
			TypeInt = 1;
		else if (dynamic_cast<CdGDSFolder*>(node))
			TypeInt = 2;
		else if (dynamic_cast<CdGDSVirtualFolder*>(node))
			TypeInt = 3;
		else if (dynamic_cast<CdGDSStreamContainer*>(node))
			TypeInt = 4;
		else if (dynamic_cast<CdContainer*>(node))
		{
			CdContainer* nn = static_cast<CdContainer*>(node);
			C_SVType sv = nn->SVType();
			if (COREARRAY_SV_INTEGER(sv))
			{
				if (GDS_R_Is_Factor(node))
					TypeInt = 6;
				else if (GDS_R_Is_Logical(node))
					TypeInt = 7;
				else
					TypeInt = 5;
			} else if (COREARRAY_SV_FLOAT(sv))
				TypeInt = 8;
			else if (COREARRAY_SV_STRING(sv))
				TypeInt = 9;
		}
		PUSH_STRING(FactorText[TypeInt - 1]);

		// 6: is.array
		flag = dynamic_cast<CdAbstractArray*>(node) ? 0x01 : 0x00;

		// 7: dim, the dimension of data field
		// 8: encoder, the compression method: "", "ZIP"
		// 9: compress, the compression method: "", "ZIP.max"
		// 10: CompressionRatio, data compression ratio, "NaN" indicates no compression
		string encoder, coder;
		*CompressionRatio = NaN;
		if (dynamic_cast<CdAbstractArray*>(node))
		{
			CdAbstractArray *_Obj = (CdAbstractArray*)node;

			for (int i=_Obj->DimCnt()-1; i >=0; i--)
				PUSH_INT(_Obj->GetDLen(i));

			if (_Obj->PipeInfo())
			{
				encoder = _Obj->PipeInfo()->Coder();
				coder   = _Obj->PipeInfo()->CoderParam();
				if (_Obj->PipeInfo()->StreamTotalIn() > 0)
				{
					*CompressionRatio =
						(double)_Obj->PipeInfo()->StreamTotalOut() /
						_Obj->PipeInfo()->StreamTotalIn();
				}
			}
		} else if (dynamic_cast<CdGDSStreamContainer*>(node))
		{
			CdGDSStreamContainer *_Obj = (CdGDSStreamContainer*)node;
			if (_Obj->PipeInfo())
			{
				encoder = _Obj->PipeInfo()->Coder();
				coder   = _Obj->PipeInfo()->CoderParam();
				if (_Obj->PipeInfo()->StreamTotalIn() > 0)
				{
					*CompressionRatio =
						(double)_Obj->PipeInfo()->StreamTotalOut() /
						_Obj->PipeInfo()->StreamTotalIn();
				}
				PUSH_INT(_Obj->PipeInfo()->StreamTotalIn());
			} else
				PUSH_INT(_Obj->GetSize());
		}
		PUSH_STRING(encoder);
		PUSH_STRING(coder);

		// 11: size
		*Size = -1;
		if (dynamic_cast<CdContainer*>(node))
		{
			CdContainer *p = static_cast<CdContainer*>(node);
			p->Synchronize();
			*Size = p->GDSStreamSize();
		} else if (dynamic_cast<CdGDSStreamContainer*>(node))
		{
			CdGDSStreamContainer *_Obj = (CdGDSStreamContainer*)node;
			if (_Obj->PipeInfo())
				*Size = _Obj->PipeInfo()->StreamTotalIn();
			else
				*Size = _Obj->GetSize();
		}

		// 12: good
		if (dynamic_cast<CdGDSVirtualFolder*>(node))
		{
			CdGDSVirtualFolder *v = (CdGDSVirtualFolder*)node;
			flag |= (v->IsLoaded(true) ? 0x02 : 0x00);
		} else if (!dynamic_cast<CdGDSUnknown*>(node))
		{
			flag |= 0x02;
		}

		// 13: hidden
		if (node->GetHidden()) flag |= 0x04;

		// 14: message
		if (dynamic_cast<CdGDSVirtualFolder*>(node))
		{
			CdGDSVirtualFolder *v = (CdGDSVirtualFolder*)node;
			v->IsLoaded(true);
			PUSH_STRING(v->ErrMsg());
		} else
			PUSH_STRING("");

/*			// 14: param
			tmp = R_NilValue;
			if (dynamic_cast<CdPackedReal8*>(Obj) ||
				dynamic_cast<CdPackedReal16*>(Obj) ||
				dynamic_cast<CdPackedReal32*>(Obj))
			{
				PROTECT(tmp = NEW_LIST(2));
				SEXP nm = PROTECT(NEW_STRING(2));
				nProtected += 2;
				SET_STRING_ELT(nm, 0, mkChar("offset"));
				SET_STRING_ELT(nm, 1, mkChar("scale"));
				SET_NAMES(tmp, nm);

				if (dynamic_cast<CdPackedReal8*>(Obj))
				{
					CdPackedReal8 *v = static_cast<CdPackedReal8*>(Obj);
					SET_ELEMENT(tmp, 0, ScalarReal(v->Offset()));
					SET_ELEMENT(tmp, 1, ScalarReal(v->Scale()));
				} else if (dynamic_cast<CdPackedReal16*>(Obj))
				{
					CdPackedReal16 *v = static_cast<CdPackedReal16*>(Obj);
					SET_ELEMENT(tmp, 0, ScalarReal(v->Offset()));
					SET_ELEMENT(tmp, 1, ScalarReal(v->Scale()));
				} else {
					CdPackedReal32 *v = static_cast<CdPackedReal32*>(Obj);
					SET_ELEMENT(tmp, 0, ScalarReal(v->Offset()));
					SET_ELEMENT(tmp, 1, ScalarReal(v->Scale()));
				}
			} else if (dynamic_cast<CdFStr8*>(Obj) ||
				dynamic_cast<CdFStr16*>(Obj) ||
				dynamic_cast<CdFStr32*>(Obj))
			{
				PROTECT(tmp = NEW_LIST(1));
				SEXP nm = PROTECT(NEW_STRING(1));
				nProtected += 2;
				SET_STRING_ELT(nm, 0, mkChar("maxlen"));
				SET_NAMES(tmp, nm);

				if (dynamic_cast<CdFStr8*>(Obj))
				{
					SET_ELEMENT(tmp, 0, ScalarInteger(
						dynamic_cast<CdFStr8*>(Obj)->MaxLength()));
				} else if (dynamic_cast<CdFStr16*>(Obj))
				{
					SET_ELEMENT(tmp, 0, ScalarInteger(
						dynamic_cast<CdFStr16*>(Obj)->MaxLength()));
				} else {
					SET_ELEMENT(tmp, 0, ScalarInteger(
						dynamic_cast<CdFStr32*>(Obj)->MaxLength()));
				}
			}
			SET_ELEMENT(rv_ans, 13, tmp);
*/

		#undef PUSH_STRING
		#undef PUSH_INT

	CORE_CATCH
	return flag;
}


/// Delete an attribute
COREARRAY_DLL_EXPORT void GDS_DeleteAttr(int node_id, PdGDSObj node,
	const char *name)
{
	CORE_TRY
		CheckGDSObj(node_id, node, true);
		node->Attribute().Delete(UTF16Text(name));
	CORE_CATCH
}





// ===========================================================================
// ===========================================================================
// Functions without try and catch
// ===========================================================================
// ===========================================================================

extern "C"
{
/// return true, if Obj is a logical object in R
COREARRAY_DLL_EXPORT C_BOOL GDS_R_Is_Logical(PdGDSObj Obj)
{
	return Obj->Attribute().HasName(ASC16("R.logical"));
}

/// return true, if Obj is a factor variable
COREARRAY_DLL_EXPORT C_BOOL GDS_R_Is_Factor(PdGDSObj Obj)
{
	if (Obj->Attribute().HasName(ASC16("R.class")) &&
		Obj->Attribute().HasName(ASC16("R.levels")))
	{
		return (RawText(Obj->Attribute()[ASC16("R.class")].GetStr8())
			== "factor");
	} else
		return false;
}

}


} // extern "C"
