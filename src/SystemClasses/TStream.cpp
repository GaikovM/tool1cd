#include "TStream.hpp"
#include "Exception.hpp"
#include <cstring>

namespace System {

namespace Classes {

TStream::TStream()
	: m_position(0), m_size(0)
{
}


int64_t TStream::GetSize() const
{
	return m_size;
}

void TStream::SetSize(int64_t NewSize)
{
	// TODO: реализовать TStream::SetSize
	m_size = NewSize;
}

int64_t TStream::GetPosition() const
{
	return m_position;
}

void TStream::SetPosition(int64_t NewPosition)
{
	m_position = NewPosition;
}


int64_t TStream::Read(void *Buffer, int64_t Count)
{
	return 0;
}

int64_t TStream::Seek(const int64_t offset, const TSeekOrigin Origin)
{
	if (Origin == soFromBeginning) {

		if (offset > GetSize()) {
			throw Exception("Wrong offset");
		}
		m_position = offset;

	} else if (Origin == soFromEnd) {

		m_position = GetSize() - offset;

		if (GetSize() < offset) {
			throw Exception("Wrong offset");
		}

	} else {

		if (m_position + offset < 0 || m_position + offset > GetSize()) {
			throw Exception("Wrong offset");
		}
		m_position += offset;
	}
	return m_position;
}

int64_t TStream::Write(const void *Buffer, int64_t Count)
{
	return 0;
}

int64_t TStream::ReadBuffer(void *Buffer, int64_t Count)
{
	auto data_read = Read(Buffer, Count);
	return data_read;
}

int64_t TStream::Read(std::vector<uint8_t> &Buffer, int64_t Count)
{
	auto CountToRead = (Count <= 0) ? GetSize() : Count;
	if (Buffer.size() < CountToRead) {
		Buffer.resize(CountToRead);
	}
	return Read(Buffer.data(), CountToRead);
}

int64_t TStream::CopyFrom(TStream *Source, const int64_t Count)
{
	if (Count == 0) {
		Source->Seek(0, soFromBeginning);
		auto data_size = Source->GetSize();
		if (data_size != 0) {
			return CopyFrom(Source, data_size);
		}
		return 0;
	}
	std::vector<uint8_t> _data;
	auto resultCount = Source->Read(_data, Count);
	return Write(_data, resultCount);
}

int64_t TStream::WriteBuffer(const void *Buffer, const int64_t Count)
{
	auto result = Write(Buffer, Count);
	if (result != Count) {
		// Throw EWriteError
	}
	return result;
}

int64_t TStream::Write(const std::vector<uint8_t> &Buffer, const int64_t Count)
{
	if (Count == 0) {
		auto data_size = Buffer.size();
		if (data_size != 0) {
			return Write(Buffer.data(), data_size);
		}
		return 0;
	}
	return Write(Buffer.data(), Count);
}

void TStream::Close()
{
}

void TStream::WriteString(const std::string &string)
{
	Write(string.c_str(), string.size());
}

TStream::~TStream()
{
}


THandleStream::~THandleStream()
{
}

TWrapperStream::TWrapperStream()
{
}

TWrapperStream::TWrapperStream(const std::shared_ptr<std::iostream> &stream)
	: _stream(stream)
{
	init_size();
}

TWrapperStream::TWrapperStream(std::shared_ptr<std::iostream> &&stream)
	: _stream(stream)
{
	init_size();
}

void TWrapperStream::reset(std::iostream *stream)
{
	_stream.reset(stream);
	init_size();
	m_position = 0;
}

void TWrapperStream::reopen(const std::shared_ptr<std::iostream> &stream)
{
	_stream = stream;
	init_size();
}

void TWrapperStream::init_size()
{
	_stream->seekg(0, std::ios_base::end);
	m_size = _stream->tellg();
	_stream->seekg(0, std::ios_base::beg);
	if (m_size < 0) {
		m_size = 0;
	}
}


int64_t TWrapperStream::Read(void *Buffer, int64_t Count)
{
	_stream->clear();
	_stream->seekg(GetPosition(), std::ios_base::beg);
	_stream->read((char*)Buffer, Count);
	if (_stream->bad()) {
		String err(std::strerror(errno));
		std::cerr << err << std::endl;
		throw Exception(err);
	}
	auto data_read = _stream->gcount();
	m_position += data_read;
	return data_read;
}

int64_t TWrapperStream::Write(const void *Buffer, int64_t Count)
{
	_stream->seekp(GetPosition(), std::ios_base::beg);
	_stream->write((char*)Buffer, Count);
	_stream->flush();
	if (_stream->bad()) {
		String err(std::strerror(errno));
		std::cerr << err << std::endl;
		throw Exception(err);
	}

	m_position += Count;
	if (m_position > m_size) {
		m_size = m_position;
	}

	return Count;
}

TWrapperStream::~TWrapperStream()
{
	_stream.reset();
}

} // Classes

} // System

