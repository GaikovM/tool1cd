#include <memory>

#include"APIcfBase.h"
#include "V8File.h"

extern Registrator msreg_g;

using namespace System;

V8File::V8File(V8Catalog* _parent, const String& _name, V8File* _previous, int _start_data, int _start_header, int64_t* _time_create, int64_t* _time_modify)
{
	Lock = new TCriticalSection();
	_destructed = false;
	flushed  = false;
	parent   = _parent;
	name     = _name;
	previous = _previous;
	next     = nullptr;
	data     = nullptr;
	start_data        = _start_data;
	start_header      = _start_header;
	_datamodified   = !start_data;
	is_headermodified = !start_header;
	if(previous)
		previous->next = this;
	else
		parent->first_file(this);
	iscatalog = FileIsCatalog::unknown;
	self = nullptr;
	is_opened = false;
	time_create = *_time_create;
	time_modify = *_time_modify;
	_selfzipped = false;
	if(parent) {
		V8Catalog::V8Files& files = parent->v8files();
		files[name.UpperCase()] = this;
	}
}

//---------------------------------------------------------------------------
// получить время создания
void V8File::GetTimeCreate(FILETIME* ft)
{
	V8timeToFileTime(&time_create, ft);
}

//---------------------------------------------------------------------------
// получить время модификации
void V8File::GetTimeModify(FILETIME* ft)
{
	V8timeToFileTime(&time_modify, ft);
}

//---------------------------------------------------------------------------
// установить время создания
void V8File::SetTimeCreate(FILETIME* ft)
{
	FileTimeToV8time(ft, &time_create);
}

//---------------------------------------------------------------------------
// установить время модификации
void V8File::SetTimeModify(FILETIME* ft)
{
	FileTimeToV8time(ft, &time_modify);
}

//---------------------------------------------------------------------------
// сохранить в файл
void V8File::SaveToFile(const boost::filesystem::path &FileName)
{
	FILETIME create, modify;

#ifdef _MSC_VER

		struct _utimbuf ut;

#else

		struct utimbuf ut;

#endif // _MSC_VER

	if (!try_open()){
		return;
	}

	TFileStream fs(FileName, fmCreate);
	Lock->Acquire();
	fs.CopyFrom(data, 0);
	fs.Close();
	Lock->Release();

	GetTimeCreate(&create);
	GetTimeModify(&modify);

	time_t RawtimeCreate = FileTime_to_POSIX(&create);
	struct tm * ptm_create = localtime(&RawtimeCreate);
	ut.actime = mktime(ptm_create);

	time_t RawtimeModified = FileTime_to_POSIX(&create);
	struct tm * ptm_modified = localtime(&RawtimeModified);
	ut.modtime = mktime(ptm_modified);

	#ifdef _MSC_VER

		_utime(FileName.string().c_str(), &ut);

	#else

		utime(FileName.string().c_str(), &ut);

	#endif // _MSC_VER
}

//---------------------------------------------------------------------------
// сохранить в поток
void V8File::SaveToStream(TStream* stream)
{
	Lock->Acquire();
	if (!try_open()) {
		return;
	}
	stream->CopyFrom(data, 0);
	Lock->Release();
}

int64_t V8File::GetFileLength()
{
	int64_t ret = 0l;
	Lock->Acquire();
	if (!try_open()) {
		return ret;
	}
	ret = data->GetSize();
	Lock->Release();
	return ret;
}

//---------------------------------------------------------------------------
// чтение
int64_t V8File::Read(void* Buffer, int Start, int Length)
{
	int64_t ret = 0l;
	Lock->Acquire();
	if (!try_open()){
		return ret;
	}
	data->Seek(Start, soFromBeginning);
	ret = data->Read(Buffer, Length);
	Lock->Release();
	return ret;
}

//---------------------------------------------------------------------------
// чтение
int64_t V8File::Read(std::vector<uint8_t> Buffer, int Start, int Length)
{
	int64_t ret = 0l;
	Lock->Acquire();
	if (!try_open()){
		return ret;
	}
	data->Seek(Start, soFromBeginning);
	ret = data->Read(Buffer, Length);
	Lock->Release();
	return ret;
}

//---------------------------------------------------------------------------
// получить поток
TV8FileStream* V8File::get_stream(bool own)
{
	return new TV8FileStream(this, own);
}

//---------------------------------------------------------------------------
// записать
int64_t V8File::Write(const void* Buffer, int Start, int Length) // дозапись/перезапись частично
{
	int64_t ret = 0l;
	Lock->Acquire();
	if (!try_open()){
		return ret;
	}
	setCurrentTime(&time_modify);
	is_headermodified = true;
	_datamodified   = true;
	data->Seek(Start, soFromBeginning);
	ret = data->Write(Buffer, Length);
	Lock->Release();

	return ret;
}

//---------------------------------------------------------------------------
// записать
int64_t V8File::Write(std::vector<uint8_t> Buffer, int Start, int Length) // дозапись/перезапись частично
{
	int64_t ret = 0l;
	Lock->Acquire();
	if (!try_open()){
		return ret;
	}
	setCurrentTime(&time_modify);
	is_headermodified = true;
	_datamodified   = true;
	data->Seek(Start, soFromBeginning);
	ret = data->Write(Buffer, Length);
	Lock->Release();

	return ret;
}

//---------------------------------------------------------------------------
// записать
int64_t V8File::Write(const void* Buffer, int Length) // перезапись целиком
{
	int64_t ret = 0l;
	Lock->Acquire();
	if (!try_open()) {
		return ret;
	}
	setCurrentTime(&time_modify);
	is_headermodified = true;
	_datamodified = true;
	if (data->GetSize() > Length) data->SetSize(Length);
	data->Seek(0, soFromBeginning);
	ret = data->Write(Buffer, Length);
	Lock->Release();

	return ret;
}

//---------------------------------------------------------------------------
// записать
int64_t V8File::Write(TStream* Stream, int Start, int Length) // дозапись/перезапись частично
{
	int64_t ret = 0l;
	Lock->Acquire();
	if (!try_open()) {
		return ret;
	}
	setCurrentTime(&time_modify);
	is_headermodified = true;
	_datamodified   = true;
	data->Seek(Start, soFromBeginning);
	ret = data->CopyFrom(Stream, Length);
	Lock->Release();

	return ret;
}

//---------------------------------------------------------------------------
// записать
int64_t V8File::Write(TStream* Stream) // перезапись целиком
{
	int64_t ret = 0l;
	Lock->Acquire();
	if (!try_open()) {
		return ret;
	}
	setCurrentTime(&time_modify);
	is_headermodified = true;
	_datamodified   = true;
	if (data->GetSize() > Stream->GetSize()) data->SetSize(Stream->GetSize());
	data->Seek(0, soFromBeginning);
	ret = data->CopyFrom(Stream, 0);
	Lock->Release();

	return ret;
}


//---------------------------------------------------------------------------
// возвращает имя
String V8File::GetFileName()
{
	return name;
}

//---------------------------------------------------------------------------
// возвращает полное имя
String V8File::GetFullName()
{
	if(parent != nullptr) {
		String fulln = parent->get_full_name();
		if(!fulln.IsEmpty())
		{
			fulln += "\\";
			fulln += name;
			return fulln;
		}
	}
	return name;
}

//---------------------------------------------------------------------------
// устанавливает имя
void V8File::SetFileName(const String& _name)
{
	name = _name;
	is_headermodified = true;
}

//---------------------------------------------------------------------------
// определение "каталога"
bool V8File::IsCatalog()
{
	int64_t _filelen;
	uint32_t _startempty = (uint32_t)(-1);
	char _t[BLOCK_HEADER_LEN];

	Lock->Acquire();
	if(iscatalog == FileIsCatalog::unknown){
		// эмпирический метод?
		if (!try_open())
		{
			Lock->Release();
			return false;
		}
		_filelen = data->GetSize();
		if(_filelen == CATALOG_HEADER_LEN)
		{
			data->Seek(0, soFromBeginning);
			data->Read(_t, CATALOG_HEADER_LEN);
			if(memcmp(_t, _EMPTY_CATALOG_TEMPLATE, CATALOG_HEADER_LEN) != 0)
			{
				iscatalog = FileIsCatalog::no;
				Lock->Release();
				return false;
			}
			else
			{
				iscatalog = FileIsCatalog::yes;
				Lock->Release();
				return true;
			}
		}

		data->Seek(0, soFromBeginning);
		data->Read(&_startempty, 4);
		if(_startempty != LAST_BLOCK){
			if(_startempty + 31 >= _filelen){
				iscatalog = FileIsCatalog::no;
				Lock->Release();
				return false;
			}
			data->Seek(_startempty, soFromBeginning);
			data->Read(_t, 31);
			if(_t[0] != 0xd || _t[1] != 0xa || _t[10] != 0x20 || _t[19] != 0x20 || _t[28] != 0x20 || _t[29] != 0xd || _t[30] != 0xa){
				iscatalog = FileIsCatalog::no;
				Lock->Release();
				return false;
			}
		}
		if(_filelen < (BLOCK_HEADER_LEN - 1 + CATALOG_HEADER_LEN) ){
			iscatalog = FileIsCatalog::no;
			Lock->Release();
			return false;
		}
		data->Seek(CATALOG_HEADER_LEN, soFromBeginning);
		data->Read(_t, 31);
		if(_t[0] != 0xd || _t[1] != 0xa || _t[10] != 0x20 || _t[19] != 0x20 || _t[28] != 0x20 || _t[29] != 0xd || _t[30] != 0xa){
			iscatalog = FileIsCatalog::no;
			Lock->Release();
			return false;
		}
		iscatalog = FileIsCatalog::yes;
		Lock->Release();
		return true;
	}
	Lock->Release();
	return iscatalog == FileIsCatalog::yes;
}

//---------------------------------------------------------------------------
// получение "каталога"
V8Catalog* V8File::GetCatalog(){
	V8Catalog* ret;

	Lock->Acquire();
	if(IsCatalog())
	{
		if(!self)
		{
			self = new V8Catalog(this);
		}
		ret = self;
	}
	else ret = nullptr;
	Lock->Release();
	return ret;
}

//---------------------------------------------------------------------------
// получение родительского контейнера
V8Catalog* V8File::GetParentCatalog()
{
	return parent;
}

//---------------------------------------------------------------------------
// удалить файл
void V8File::DeleteFile()
{
	Lock->Acquire();
	if(parent)
	{
		parent->Acquire();
		if(next)
		{
			next->Lock->Acquire();
			next->previous = previous;
			next->Lock->Release();
		}
		else parent->last_file(previous);
		if(previous) {
			previous->Lock->Acquire();
			previous->next = next;
			previous->Lock->Release();
		}
		else {
			parent->first_file(next);
		}

		parent->fatmodified(true);
		parent->free_block(start_data);
		parent->free_block(start_header);

		V8Catalog::V8Files& files = parent->v8files();
		files.erase(name.UpperCase());

		parent->Release();
		parent = nullptr;
	}
	delete data;
	data = nullptr;
	if(self)
	{
		self->erase_data(); // TODO: разобраться, зачем обнуляется указатель
		delete self;
		self = nullptr;
	}
	iscatalog = FileIsCatalog::no;
	next = nullptr;
	previous = nullptr;
	is_opened = false;
	start_data = 0;
	start_header = 0;
	_datamodified = false;
	is_headermodified = false;
}

//---------------------------------------------------------------------------
// получить следующий
V8File* V8File::GetNext()
{
	return next;
}

//---------------------------------------------------------------------------
// открыть файл
bool V8File::Open(){
	if(!parent) return false;
	Lock->Acquire();
	if(is_opened)
	{
		Lock->Release();
		return true;
	}
	data = parent->read_datablock(start_data);
	is_opened = true;
	Lock->Release();
	return true;
}

//---------------------------------------------------------------------------
// закрыть файл
void V8File::Close(){
	int _t = 0;

	if(!parent) return;
	Lock->Acquire();
	if(!is_opened) return;

	if( self && !self->is_destructed() ) {
		delete self;
	}
	self = nullptr;

	if(!parent->data_empty()) {
		if(_datamodified || is_headermodified)
		{
			parent->Acquire();
			if(_datamodified)
			{
				start_data = parent->write_datablock(data, start_data, _selfzipped);
			}
			if(is_headermodified)
			{
				TMemoryStream* hs = new TMemoryStream();
				hs->Write(&time_create, 8);
				hs->Write(&time_modify, 8);
				hs->Write(&_t, 4);
				#ifndef _DELPHI_STRING_UNICODE // FIXME: определится используем WCHART или char
				int ws = name.WideCharBufSize();
				char* tb = new char[ws];
				name.WideChar((WCHART*)tb, ws);
				hs->Write((char*)tb, ws);
				delete[] tb;
				#else
				hs->Write(name.c_str(), name.Length() * 2);
				#endif
				hs->Write(&_t, 4);

				start_header = parent->write_block(hs, start_header, false);
				delete hs;
			}
			parent->Release();
		}
	}
	delete data;
	data = nullptr;
	iscatalog = FileIsCatalog::unknown;
	is_opened = false;
	_datamodified = false;
	is_headermodified = false;
	Lock->Release();
}

//---------------------------------------------------------------------------
// записать и закрыть
int64_t V8File::WriteAndClose(TStream* Stream, int Length)
{
	int32_t _4bzero = 0;

	Lock->Acquire();
	if (!try_open())
	{
		Lock->Release();
		return 0;
	}

	if (!parent)
	{
		Lock->Release();
		return 0;
	}

	if (self) delete self;
	self = nullptr;

	delete data;
	data = nullptr;

	if (!parent->data_empty()) {
		int name_size = name.WideCharBufSize();
		WCHART *wname = new WCHART[name_size];
		name.WideChar(wname, name.Length());

		parent->Acquire();
		start_data = parent->write_datablock(Stream, start_data, _selfzipped, Length);
		TMemoryStream hs;
		hs.Write(&time_create, 8);
		hs.Write(&time_modify, 8);
		hs.Write(&_4bzero, 4);
		hs.Write(wname, name.Length() * sizeof(WCHART));
		hs.Write(&_4bzero, 4);
		start_header = parent->write_block(&hs, start_header, false);
		parent->Release();
		delete[]wname;
	}
	iscatalog = FileIsCatalog::unknown;
	is_opened = false;
	_datamodified = false;
	is_headermodified = false;
	Lock->Release();

	if (Length == -1) return Stream->GetSize();
	return Length;
}

//---------------------------------------------------------------------------
// деструктор
V8File::~V8File()
{
	std::set<TV8FileStream*>::iterator istreams;

	Lock->Acquire();
	_destructed = true;
	for(istreams = _streams.begin(); istreams != _streams.end(); ++istreams) {
		delete *istreams;
	}
	_streams.clear();

	Close();

	if(parent)
	{
		if(next)
		{
			next->Lock->Acquire();
			next->previous = previous;
			next->Lock->Release();
		}
		else
		{
			parent->Acquire();
			parent->last_file(previous);
			parent->Release();
		}
		if(previous)
		{
			previous->Lock->Acquire();
			previous->next = next;
			previous->Lock->Release();
		}
		else
		{
			parent->Acquire();
			parent->first_file(next);
			parent->Release();
		}
	}
	delete Lock;
}

//---------------------------------------------------------------------------
// сброс
void V8File::Flush()
{
	int _t = 0;

	Lock->Acquire();
	if(flushed)
	{
		Lock->Release();
		return;
	}
	if(!parent)
	{
		Lock->Release();
		return;
	}
	if(!is_opened)
	{
		Lock->Release();
		return;
	}

	flushed = true;
	if(self) self->Flush();

	if( !parent->data_empty() )	{
		if(_datamodified || is_headermodified)
		{
			parent->Acquire();
			if(_datamodified)
			{
				start_data = parent->write_datablock(data, start_data, _selfzipped);
				_datamodified = false;
			}
			if(is_headermodified)
			{
				TMemoryStream* hs = new TMemoryStream();
				hs->Write(&time_create, 8);
				hs->Write(&time_modify, 8);
				hs->Write(&_t, 4);
				#ifndef _DELPHI_STRING_UNICODE
				int ws = name.WideCharBufSize();
				char* tb = new char[ws];
				name.WideChar((WCHART*)tb, ws);
				hs->Write((char*)tb, ws);
				delete[] tb;
				#else
				hs->Write(name.c_str(), name.Length() * 2);
				#endif
				hs->Write(&_t, 4);

				start_header = parent->write_block(hs, start_header, false);
				delete hs;
				is_headermodified = false;
			}
			parent->Release();
		}
	}
	flushed = false;
	Lock->Release();
}

tree* V8File::get_tree()
{
	std::vector<uint8_t> bytes;
	std::unique_ptr<TBytesStream> bytes_stream( new TBytesStream(bytes) );
	SaveToStream(bytes_stream.get());

	TEncoding *enc = nullptr;
	int32_t offset = TEncoding::GetBufferEncoding(bytes_stream->GetBytes(), enc);
	if(offset == 0 || enc == nullptr) {
		msreg_g.AddError("Ошибка определения кодировки файла контейнера",
			"Файл",  GetFullName());
		return nullptr;
	}

	String text = enc->toUtf8(bytes_stream->GetBytes(), offset);

	tree* rt = parse_1Ctext(text, GetFullName());

	return rt;
}

TCriticalSection* V8File::get_lock() {
	return Lock;
}

TStream* V8File::get_data() {
	return data;
}

int V8File::get_start_data() const {
	return start_data;
}

int V8File::get_start_header() const {
	return start_header;
}

bool V8File::is_datamodified() const {
	return _datamodified;
}

void V8File::datamodified(const bool value) {
	_datamodified = value;
}

bool V8File::is_destructed() const {
	return _destructed;
}

void V8File::destructed(const bool value) {
	_destructed = value;
}

bool V8File::is_self_zipped() const {
	return _selfzipped;
}

void V8File::self_zipped(const bool value) {
	_selfzipped = value;
}

V8File::TV8FileStreams& V8File::streams() {
	return _streams;
}
