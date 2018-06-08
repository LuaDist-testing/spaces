//
// Compress.h
//
// $Id: //poco/1.4/Zip/include/Poco/Zip/Compress.h#2 $
//
// Library: Zip
// Package: Zip
// Module:  Compress
//
// Definition of the Compress class.
//
// Copyright (c) 2007, Applied Informatics Software Engineering GmbH.
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


#ifndef Zip_Compress_INCLUDED
#define Zip_Compress_INCLUDED


#include "Poco/Zip/Zip.h"
#include "Poco/Zip/ZipArchive.h"
#include "Poco/FIFOEvent.h"
#include <istream>
#include <ostream>


namespace Poco {
namespace Zip {


class Zip_API Compress
	/// Compresses a directory or files as zip.
{
public:
	Poco::FIFOEvent<const ZipLocalFileHeader> EDone;

	Compress(std::ostream& out, bool seekableOut);
		/// seekableOut determines how we write the zip, setting it to true is recommended for local files (smaller zip file),
		/// if you are compressing directly to a network, you MUST set it to false

	~Compress();

	void addFile(std::istream& input, const Poco::DateTime& lastModifiedAt, const Poco::Path& fileName, ZipCommon::CompressionMethod cm = ZipCommon::CM_DEFLATE, ZipCommon::CompressionLevel cl = ZipCommon::CL_MAXIMUM);
		/// Adds a single file to the Zip File. fileName must not be a directory name.

	void addFile(const Poco::Path& file, const Poco::Path& fileName, ZipCommon::CompressionMethod cm = ZipCommon::CM_DEFLATE, ZipCommon::CompressionLevel cl = ZipCommon::CL_MAXIMUM);
		/// Adds a single file to the Zip File. fileName must not be a directory name. The file must exist physically!

	void addDirectory(const Poco::Path& entryName, const Poco::DateTime& lastModifiedAt);
		/// Adds a directory entry excluding all children to the Zip file, entryName must not be empty.

	void addRecursive(const Poco::Path& entry, ZipCommon::CompressionLevel cl = ZipCommon::CL_MAXIMUM, bool excludeRoot = true, const Poco::Path& name = Poco::Path());
		/// Adds a directory entry recursively to the zip file, set excludeRoot to false to exclude the parent directory.
		/// If excludeRoot is true you can specify an empty name to add the files as relative files

	void addRecursive(const Poco::Path& entry, ZipCommon::CompressionMethod cm, ZipCommon::CompressionLevel cl = ZipCommon::CL_MAXIMUM, bool excludeRoot = true, const Poco::Path& name = Poco::Path());
		/// Adds a directory entry recursively to the zip file, set excludeRoot to false to exclude the parent directory.
		/// If excludeRoot is true you can specify an empty name to add the files as relative files

	void setZipComment(const std::string& comment);
		/// Sets the Zip file comment.

	const std::string& getZipComment() const;
		/// Returns the Zip file comment.
		
	ZipArchive close();
		/// Finalizes the ZipArchive, closes it.

private:
	enum
	{
		COMPRESS_CHUNK_SIZE = 8192
	};

	Compress(const Compress&);
	Compress& operator=(const Compress&);

	void addEntry(std::istream& input, const Poco::DateTime& lastModifiedAt, const Poco::Path& fileName, ZipCommon::CompressionMethod cm = ZipCommon::CM_DEFLATE, ZipCommon::CompressionLevel cl = ZipCommon::CL_MAXIMUM);
		/// Either adds a file or a single directory entry (excluding subchildren) to the Zip file. the compression level will be ignored
		/// for directories.

	void addFileRaw(std::istream& in, const ZipLocalFileHeader& hdr, const Poco::Path& fileName);
		/// copys an already compressed ZipEntry from in

private:
	std::ostream&              _out;
	bool                       _seekableOut;
	ZipArchive::FileHeaders    _files;
	ZipArchive::FileInfos      _infos;
	ZipArchive::DirectoryInfos _dirs;
	Poco::UInt32               _offset;
    std::string                _comment;

	friend class Keep;
	friend class Rename;
};


//
// inlines
//
inline void Compress::setZipComment(const std::string& comment)
{
	_comment = comment;
}


inline const std::string& Compress::getZipComment() const
{
	return _comment;
}


} } // namespace Poco::Zip


#endif // Zip_Compress_INCLUDED
