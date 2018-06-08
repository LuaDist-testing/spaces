//
// ODBCException.h
//
// $Id: //poco/1.4/Data/ODBC/include/Poco/Data/ODBC/ODBCException.h#1 $
//
// Library: ODBC
// Package: ODBC
// Module:  ODBCException
//
// Definition of ODBCException.
//
// Copyright (c) 2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#ifndef Data_ODBC_ODBCException_INCLUDED
#define Data_ODBC_ODBCException_INCLUDED


#include "Poco/Data/ODBC/ODBC.h"
#include "Poco/Data/ODBC/Utility.h"
#include "Poco/Data/ODBC/Diagnostics.h"
#include "Poco/Data/ODBC/Error.h"
#include "Poco/Data/DataException.h"
#include "Poco/Format.h"


namespace Poco {
namespace Data {
namespace ODBC {


POCO_DECLARE_EXCEPTION(ODBC_API, ODBCException, Poco::Data::DataException)
POCO_DECLARE_EXCEPTION(ODBC_API, InsufficientStorageException, ODBCException)
POCO_DECLARE_EXCEPTION(ODBC_API, UnknownDataLengthException, ODBCException)
POCO_DECLARE_EXCEPTION(ODBC_API, DataTruncatedException, ODBCException)


template <class H, SQLSMALLINT handleType>
class HandleException: public ODBCException
{
public:
	HandleException(const H& handle): _error(handle)
		/// Creates HandleException
	{
		message(_error.toString());
	}

	HandleException(const H& handle, const std::string& msg): 
		ODBCException(msg), 
		_error(handle)
		/// Creates HandleException
	{
		extendedMessage(_error.toString());
	}							

	HandleException(const H& handle, const std::string& msg, const std::string& arg): 
		ODBCException(msg, arg), 
		_error(handle)
		/// Creates HandleException
	{
	}

	HandleException(const H& handle, const std::string& msg, const Poco::Exception& exc): 
		ODBCException(msg, exc),
		_error(handle)
		/// Creates HandleException
	{
	}

	HandleException(const HandleException& exc): 
		ODBCException(exc),
		_error(exc._error)
		/// Creates HandleException
	{
	}

	~HandleException() throw()
		/// Destroys HandleException
	{
	}

	HandleException& operator = (const HandleException& exc)
		/// Assignment operator
	{
		HandleException::operator = (exc);
		return *this;
	}

	const char* name() const throw()
		/// Returns the name of the exception
	{
		return "ODBC handle exception";
	}

	const char* className() const throw()
		/// Returns the HandleException class name.
	{
		return typeid(*this).name();
	}

	Poco::Exception* clone() const
		/// Clones the HandleException
	{
		return new HandleException(*this);
	}

	void rethrow() const
		/// Re-throws the HandleException.
	{
		throw *this;
	}

	const Diagnostics<H, handleType>& diagnostics() const
		/// Returns error diagnostics.
	{
		return _error.diagnostics();
	}

	std::string toString() const
		/// Returns the formatted error diagnostics for the handle.
	{
		return Poco::format("ODBC Error: %s\n===================\n%s\n",
			std::string(what()),
			_error.toString());
	}

	static std::string errorString(const H& handle)
		/// Returns the error diagnostics string for the handle.
	{
		return Error<H, handleType>(handle).toString();
	}

private:
	Error<H, handleType> _error;
};


typedef HandleException<SQLHENV, SQL_HANDLE_ENV>   EnvironmentException;
typedef HandleException<SQLHDBC, SQL_HANDLE_DBC>   ConnectionException;
typedef HandleException<SQLHSTMT, SQL_HANDLE_STMT> StatementException;
typedef HandleException<SQLHDESC, SQL_HANDLE_DESC> DescriptorException;


} } } // namespace Poco::Data::ODBC


#endif
