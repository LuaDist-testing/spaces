//
// OpenSSLInitializer.h
//
// $Id: //poco/1.4/Crypto/include/Poco/Crypto/OpenSSLInitializer.h#2 $
//
// Library: Crypto
// Package: CryptoCore
// Module:  OpenSSLInitializer
//
// Definition of the OpenSSLInitializer class.
//
// Copyright (c) 2006-2009, Applied Informatics Software Engineering GmbH.
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


#ifndef Crypto_OpenSSLInitializer_INCLUDED
#define Crypto_OpenSSLInitializer_INCLUDED


#include "Poco/Crypto/Crypto.h"
#include "Poco/Mutex.h"
#include <openssl/opensslconf.h>
#ifdef OPENSSL_FIPS
#include <openssl/fips.h>
#endif


extern "C"
{
	struct CRYPTO_dynlock_value
	{
		Poco::FastMutex _mutex;
	};
}


namespace Poco {
namespace Crypto {


class Crypto_API OpenSSLInitializer
	/// Initalizes the OpenSSL library.
	///
	/// The class ensures the earliest initialization and the
	/// latest shutdown of the OpenSSL library.
{
public:
	OpenSSLInitializer();
		/// Automatically initialize OpenSSL on startup.
		
	~OpenSSLInitializer();
		/// Automatically shut down OpenSSL on exit.
	
	static void initialize();
		/// Initializes the OpenSSL machinery.

	static void uninitialize();
		/// Shuts down the OpenSSL machinery.

	static bool isFIPSEnabled();
		// Returns true if FIPS mode is enabled, false otherwise.

	static void enableFIPSMode(bool enabled);
		// Enable or disable FIPS mode. If FIPS is not available, this method doesn't do anything.

protected:
	enum
	{
		SEEDSIZE = 256
	};
	
	// OpenSSL multithreading support
	static void lock(int mode, int n, const char* file, int line);
	static unsigned long id();
	static struct CRYPTO_dynlock_value* dynlockCreate(const char* file, int line);
	static void dynlock(int mode, struct CRYPTO_dynlock_value* lock, const char* file, int line);
	static void dynlockDestroy(struct CRYPTO_dynlock_value* lock, const char* file, int line);

private:
	static Poco::FastMutex* _mutexes;
	static Poco::FastMutex _mutex;
	static int _rc;
};


//
// inlines
//
inline bool OpenSSLInitializer::isFIPSEnabled()
{
#ifdef OPENSSL_FIPS
	return FIPS_mode() ? true : false;
#else
	return false;
#endif
}


#ifdef OPENSSL_FIPS
inline void OpenSSLInitializer::enableFIPSMode(bool enabled)
{
	FIPS_mode_set(enabled);
}
#else
inline void OpenSSLInitializer::enableFIPSMode(bool /*enabled*/)
{
}
#endif


} } // namespace Poco::Crypto


#endif // Crypto_OpenSSLInitializer_INCLUDED
