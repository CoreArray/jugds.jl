// ===========================================================
//
// J_GDS_CPP.h: C interface to jugds dynamic library + CPP classes
//
// Copyright (C) 2017    Xiuwen Zheng
//
// This file is part of jugds.
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


#ifndef _HEADER_JULIA_CPP_
#define _HEADER_JULIA_CPP_

#include <J_GDS.h>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <climits>


namespace CoreArray
{
	// ==================================================================
	// ==================================================================

	/// try block for CoreArray library
	#define CORE_TRY    \
		try {

	/// try block for CoreArray library
	#define COREARRAY_TRY    \
		bool has_error = false; \
		CORE_TRY


	/// catch block for CoreArray library
	#define CORE_CATCH(cmd)    \
		} \
		catch (std::exception &E) { \
			GDS_SetError(E.what()); cmd; \
		} \
		catch (const char *E) { \
			GDS_SetError(E); cmd; \
		} \
		catch (...) { \
			GDS_SetError("unknown error!"); cmd; \
		}

	/// catch block for CoreArray library
	#define COREARRAY_CATCH    \
		CORE_CATCH(has_error = true); \
		if (has_error) jl_error(GDS_GetError());

	/// catch block for CoreArray library
	#define COREARRAY_CATCH_RETNULL    \
		CORE_CATCH(has_error = true); \
		if (has_error) jl_error(GDS_GetError()); \
		return NULL;

	/// catch block for CoreArray library
	#define COREARRAY_CATCH_RET    \
		CORE_CATCH(has_error = true); \
		if (has_error) jl_error(GDS_GetError()); \
		return -1;



	// ==================================================================
	// ==================================================================

	// [[ ********
	#ifndef COREARRAY_JULIA_PACKAGE

	/// Error Macro
	#define _COREARRAY_ERRMACRO_(x) { \
		va_list args; va_start(args, x); \
		Init(x, args); \
		va_end(args); \
	}

	/// The root class of exception for CoreArray library
	class ErrCoreArray: public std::exception
	{
	public:
		ErrCoreArray() {}
		ErrCoreArray(const char *fmt, ...) { _COREARRAY_ERRMACRO_(fmt); }
		ErrCoreArray(const std::string &msg) { fMessage = msg; }

		virtual const char *what() const throw()
			{ return fMessage.c_str(); }
		virtual ~ErrCoreArray() throw() {};

	protected:
		std::string fMessage;

		void Init(const char *fmt, va_list arglist)
		{
			char buf[1024];
			vsnprintf(buf, sizeof(buf), fmt, arglist);
			fMessage = buf;
		}
	};

	#else  // COREARRAY_JULIA_PACKAGE

	/// error exception for the jugds package
	class COREARRAY_DLL_EXPORT ErrGDSFmt: public ErrCoreArray
	{
	public:
		ErrGDSFmt() {}
		ErrGDSFmt(const char *fmt, ...) { _COREARRAY_ERRMACRO_(fmt); }
		ErrGDSFmt(const std::string &msg) { fMessage = msg; }
	};

	#endif  // COREARRAY_JULIA_PACKAGE
	// ]] ********

}

#endif /* _HEADER_JULIA_CPP_ */
