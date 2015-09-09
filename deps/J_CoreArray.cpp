// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// J_CoreArray.cpp: Export the C routines of CoreArray allowing C++ exceptions
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
 *	\brief    Export the C routines of CoreArray allowing C++ exceptions
 *	\details
**/

#include "CoreArray/CoreArray.h"
#include <string>
#include <vector>

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


/// 
#define TEXT_PUSH  \
	void (*push_text)(void *julia, const char *txt, size_t txt_len)

#define TEXT_SET   \
	void (*set_text)(void *julia, ssize_t index, const char *txt, size_t txt_len)


// ===========================================================================
// Error Messages

static const char *ERR_NOT_FOLDER = "The GDS node is not a folder!";
static const char *ERR_UNKNOWN = "Unknown error!";


// ===========================================================================
// Functions for Error

COREARRAY_DLL_EXPORT const char *GDS_Error()
{
	return (has_error) ? error_info.c_str() : NULL;
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
		// to register CoreArray classes and objects
		RegisterClass();

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
		// to register CoreArray classes and objects
		RegisterClass();

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


///
COREARRAY_DLL_EXPORT void GDS_File_Sync(PdGDSFile File)
{
	File->SyncFile();
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
		} else
			throw ErrJuGDS(ERR_NOT_FOLDER);
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

} // extern "C"
