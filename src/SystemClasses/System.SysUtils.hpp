#ifndef SYSTEM_SYSUTILS_HPP
#define SYSTEM_SYSUTILS_HPP

#include <vector>

#include "System.IOUtils.hpp"
#include "String.hpp"

namespace System {

namespace SysUtils {

String StringReplace(const String &S, const String &OldPattern, const String &NewPattern, TReplaceFlags Flags);

class TStringBuilder
{
public:
	explicit TStringBuilder();

	explicit TStringBuilder(const String &initial);

	TStringBuilder *Replace(const String &substring, const String &replace);

	String ToString() const;

	void Clear();

	void Append(const String &s);

	void Append(char c);

	String value;
};

class TMultiReadExclusiveWriteSynchronizer
{
public:
	void BeginWrite();
	void EndWrite();

	void BeginRead();
	void EndRead();
};

class TEncoding
{

public:

virtual std::vector<uint8_t> GetPreamble() = 0;
virtual String toUtf8(const std::vector<uint8_t> &Buffer, int offset = 0) const = 0;
virtual std::vector<uint8_t> fromUtf8(const String &str) = 0;

static int GetBufferEncoding(const std::vector<uint8_t> &Buffer, TEncoding* &AEncoding);
static std::vector<uint8_t> Convert(TEncoding * const Source, TEncoding * const Destination, const std::vector<uint8_t> &Bytes, int StartIndex, int Count);

//! двухбайтная кодировка WCHART
static TEncoding *Unicode;

static TEncoding *UTF8;

};

int StrToInt(const String &s);


struct TSearchRec {
	int     Time;
	int64_t Size;
	int     Attr;
	String  Name;
	int     ExcludeAttr;
};

String ExtractFileExt(const String &filename);

} // SysUtils

namespace Sysutils = SysUtils;

} // System

using namespace System::SysUtils;

#endif
