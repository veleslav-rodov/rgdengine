/** @file exceptions.h  
*  Basic error reporting mechanisms.
*
*  3 major ways to report errors:
*  1) throw_<X>
*  2) asserting error codes
*  3) usual error codes (does not need a support here)
*  
*  @author sleepydrago
*  @date june 2005
*/
#pragma once

#include "base/log.h"
#include "base/logHelper.h"

namespace base
{
	/** wraps all throw statements to make life/debug/log easier
	*/
	template <class X>
		void throw_(const X& ex)
	{
		///@todo log what and when
		assert(false); /// for debug - we just wanna break - if not comment it out
		throw ex;
	}
	/** base class for project's exceptions.
	*
	*  @note file&line info is for debug builds only and are not expected to be placed manually
	*  @warning normal way to provide file&line info is to use BASE_EXCEPTION_ARGS macro
	*  eg 
	*    class MyEx:public CBaseException{ 
	*    public: 
	*        MyEx(const std::string&msg):BASE_EXCEPTION_ARGS1(msg) {} 
	*    };
	*/
	class CBaseException : public std::exception
	{
	public:
		CBaseException(const std::string& strCode, unsigned code=0, const char* f="", int l=0)
			: std::exception(""), m_strWhat(strCode), 
			m_Code(code), m_File(f), m_Line(l)
		{
			base::lerr << "CBaseException::CBaseException() " << m_strWhat << base::endl;
		}
		const char* what(){return m_strWhat.c_str();}

	protected:
		std::string m_strWhat;
		unsigned m_Code;
		const char* m_File;
		const int   m_Line;
	};
}
#define BASE_EXCEPTION_ARGS1(what) CBaseException((what), 0, __FILE__, __LINE__)
#define BASE_EXCEPTION_ARGS2(what, code) CBaseException((what), (code), __FILE__, __LINE__)
//
//class IgnoreError {};
///** asserting error codes. 
//*
//*  Recommended usage: when ignored retcode is a BUG
//*  The only way to not check for value of ErrorCode is to cast it to the IgnoreError
//*  warning to all casters - you will be easily found by text search :)
//*  Typical usage - just replace type name "int" in return value of a method with ErrorCode<int> 
//*    run and assert in destructor of ErrorCode will find all ignorant guys.
//*/
//template< typename T >
//class ErrorCode
//{
//public:
//	ErrorCode(const T& code) : code_(code), read_(false) {}
//	ErrorCode(const ErrorCode& rhs) : code_(rhs.code_), read_(rhs.read_)
//	{
//		rhs.read_ = true;
//	}
//
//	~ErrorCode() { assert(read_); }
//
//	operator T() const { read_ = true; return code_; }
//	operator IgnoreError() const { read_ = true; return IgnoreError(); }
//
//private:
//	mutable bool read_;
//	T code_;
//
//	ErrorCode& operator =(const ErrorCode&);
//};