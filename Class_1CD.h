//---------------------------------------------------------------------------

#ifndef Class_1CDH
#define Class_1CDH

#include "vector"

#include "MessageRegistration.h"
#include "APIcfBase.h"
//#ifndef getcfname
//#include "ICU.h"
//#endif
#include "db_ver.h"
#include "Parse_tree.h"

//---------------------------------------------------------------------------

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

//String __fastcall GUIDas1C(const unsigned char* fr);
//String __fastcall GUIDasMS(const unsigned char* fr);

class T_1CD;
class table;

class ConfigStorageTableConfig;
class ConfigStorageTableConfigSave;

//unsigned int __fastcall reverse_byte_order(unsigned int value);

#pragma pack(push)
#pragma pack(1)

// ��������� ������ �������� ����������
struct v8con
{
	char sig[8]; // ��������� SIG_CON
	char ver1;
	char ver2;
	char ver3;
	char ver4;
	unsigned int length;
	int firstblock;
	unsigned int pagesize;

	String __fastcall getver(){
		String ss = (int)ver1;
		ss += ".";
		ss += (int)ver2;
		ss += ".";
		ss += (int)ver3;
		ss += ".";
		ss += (int)ver4;
		return ss;
	}
};

// ��������� �������� ���������� ������ 1 ������ �� 8.0 �� 8.2.14
struct objtab
{
	int numblocks;
	unsigned int blocks[1023];
};

// ��������� �������� ���������� ������ 1 ������ �� 8.3.8
struct objtab838
{
	unsigned int blocks[1]; // �������� ���������� ������ ������� �� ������� �������� (pagesize)
};

struct _version_rec
{
	unsigned int version_1; // ������ ����������������
	unsigned int version_2; // ������ ���������
};

struct _version
{
	unsigned int version_1; // ������ ����������������
	unsigned int version_2; // ������ ���������
	unsigned int version_3; // ������ ��������� 2
};

// ��������� ������������ �������� ����� ������ ��� ����� ��������� �������
struct v8ob
{
	char sig[8]; // ��������� SIG_OBJ
	unsigned int len; // ����� �����
	_version version;
	unsigned int blocks[1018];
};

// ��������� ������������ �������� ����� ������ ������� � ������ 8.3.8
struct v838ob_data
{
	unsigned char sig[2]; // ��������� 0x1C 0xFD (1C File Data?)
	short int fatlevel; // ������� ������� ���������� (0x0000 - � ������� blocks ������ ������� � �������, 0x0001 - � ������� blocks ������ ������� � ��������� ���������� ������� ������, � ������� ���, � ���� �������, ��������� ������ ������� � �������)
	_version version;
	uint64_t len; // ����� �����
	unsigned int blocks[1]; // �������� ����� ������� ������� �� ������� �������� � ����� pagesize/4-6 (�� ��� 1018 ��� 4� �� 16378 ��� 64�)
};

// ��������� ������������ �������� ����� ��������� ������� ������� � ������ 8.3.8
struct v838ob_free
{
	unsigned char sig[2]; // ��������� 0x1C 0xFF (1C File Free?)
	short int fatlevel; // 0x0000 ����! �� ����� ... ������� ������� ���������� (0x0000 - � ������� blocks ������ ������� � �������, 0x0001 - � ������� blocks ������ ������� � ��������� ���������� ������� ������, � ������� ���, � ���� �������, ��������� ������ ������� � �������)
	unsigned int version; // ??? ����������������...
	unsigned int blocks[1]; // �������� ����� ������� ������� �� ������� �������� � ����� pagesize/4-6 (�� ��� 1018 ��� 4� �� 16378 ��� 64�)
};


#pragma pack(pop)

// ���� ���������� ������
enum v8objtype
{
	v8ot_unknown = 0, // ��� ����������
	v8ot_data80 = 1, // ���� ������ ������� 8.0 (�� 8.2.14 ������������)
	v8ot_free80 = 2, // ���� ��������� ������� ������� 8.0 (�� 8.2.14 ������������)
	v8ot_data838 = 3, // ���� ������ ������� 8.3.8
	v8ot_free838 = 4 // ���� ��������� ������� ������� 8.3.8
};

class v8object
{
friend table;
friend T_1CD;
private:
	MessageRegistrator* err;
	T_1CD* base;

	uint64_t len; // ����� �������. ��� ���� ������� ��������� ������� - ���������� ��������� ������
	_version version; // ������� ������ �������
	_version_rec version_rec; // ������� ������ ������
	bool new_version_recorded; // �������, ��� ����� ������ ������� ��������
//	unsigned int version_restr; // ������ ����������������
//	unsigned int version_edit; // ������ ���������
	v8objtype type; // ��� � ������ �����
	int fatlevel; // ���������� ������������� ������� � ������� ����������
	unsigned int numblocks; // ���-�� ������� � �������� ������� ���������� �������
	unsigned int real_numblocks; // �������� ���-�� ������� � �������� ������� (������ ��� ������ ��������� �������, ����� ���� ������ numblocks)
	unsigned int* blocks; // ������� ������� �������� ������� ���������� ������� (�.�. ������ 0)
	unsigned int block; // ����� ����� �������
	char* data; // ������, �������������� ��������, NULL ���� �� ��������� ��� len = 0

	static v8object* first;
	static v8object* last;
	v8object* next;
	v8object* prev;
	unsigned int lastdataget; // ����� (Windows time, � �������������) ���������� ��������� � ������ ������� (data)
	bool lockinmemory;

	void __fastcall set_len(uint64_t _len); // ��������� ����� ����� �������

	void __fastcall init();
	void __fastcall init(T_1CD* _base, int blockNum);

public:
	__fastcall v8object(T_1CD* _base, int blockNum); // ����������� ������������� �������
	__fastcall v8object(T_1CD* _base); // ����������� ������ (��� �� �������������) �������
	__fastcall ~v8object();
	char* __fastcall getdata(); // ������ ����� ������� �������, ������������ ����������� ��������. ����� ����������� �������
	char* __fastcall getdata(void* buf, uint64_t _start, uint64_t _length); // ������ ������� �������, ������������ ����������� ������. ����� �� ����������� �������
	bool __fastcall setdata(void* buf, uint64_t _start, uint64_t _length); // ������ ������� �������, ������������ ����������� ������.
	bool __fastcall setdata(void* buf, uint64_t _length); // ������ ������� �������, ������������ ����������� ������.
	bool __fastcall setdata(TStream* stream); // ���������� ����� ������� � ������, ������������ ����������� ������.
	bool __fastcall setdata(TStream* stream, uint64_t _start, uint64_t _length); // ������ ����� ������ � ������, ������������ ����������� ������.
	uint64_t __fastcall getlen();
	//void __fastcall savetofile();
	void __fastcall savetofile(String filename);
	void __fastcall set_lockinmemory(bool _lock);
	static void __fastcall garbage();
	uint64_t __fastcall get_fileoffset(uint64_t offset); // �������� ���������� �������� � ����� �� �������� � �������
	void __fastcall set_block_as_free(unsigned int block_number); // �������� ���� ��� ���������
	unsigned int __fastcall get_free_block(); // �������� ����� ���������� ����� (� �������� ��� �������)
	void __fastcall get_version_rec_and_increase(_version* ver); // �������� ������ ��������� ������ � ����������� ����������� ������ �������
	void __fastcall get_version(_version* ver); // �������� ����������� ������ �������
	void __fastcall write_new_version(); // ���������� ����� ������ �������
	static v8object* __fastcall get_first();
	static v8object* __fastcall get_last();
	v8object* __fastcall get_next();
	unsigned int __fastcall get_block_number();
	TStream* readBlob(TStream* _str, unsigned int _startblock, unsigned int _length = MAXUINT, bool rewrite = true);
};

#pragma pack(push)
#pragma pack(1)

struct root_80
{
	char lang[8];
	int numblocks;
	unsigned int blocks[1];
};

struct root_81
{
	char lang[32];
	int numblocks;
	unsigned int blocks[1];
};

#pragma pack(pop)

union root
{
	root_80 root80;
	root_81 root81;
};

enum type_fields
{
	tf_binary, // B // ����� = length
	tf_bool, // L // ����� = 1
	tf_numeric, // N // ����� = (length + 2) / 2
	tf_char, // NC // ����� = length * 2
	tf_varchar, // NVC // ����� = length * 2 + 2
	tf_version, // RV // 16, 8 ������ �������� � 8 ������ ����������� ? ������ ������ int(���������) + int(����������������)
	tf_string, // NT // 8 (unicode text)
	tf_text, // T // 8 (ascii text)
	tf_image, // I // 8 (image = bynary data)
	tf_datetime, // DT //7
	tf_version8, // 8, ������� ���� ��� recordlock == false � ���������� ���� ���� tf_version
	tf_varbinary // VB // ����� = length + 2
};

// ����� �������������� bynary16 � GUID
//
// �������� ��������
// 00112233445566778899aabbccddeeff
//
// 1� style
// ccddeeff-aabb-8899-0011-223344556677
//
// MS style
// 33221100-5544-7766-8899-aabbccddeeff
//


class field
{
friend table;
friend T_1CD;
public:
	static bool showGUID;
private:
	MessageRegistrator* err;
	String name;
	type_fields type;
	bool null_exists;
	int length;
	int precision;
	bool case_sensitive;

	table* parent;
	int len; // ����� ���� � ������
	int offset; // �������� ���� � ������
	static bool showGUIDasMS; // �������, ��� GUID ���� ��������������� �� ����� MS (����� �� ����� 1�)
	static char buf[];
	static char null_index[];
	static bool null_index_initialized;

public:
	__fastcall field(table* _parent);

	int __fastcall getlen(); // ���������� ����� ���� � ������
	String __fastcall getname();
	String __fastcall get_presentation(const char* rec, bool EmptyNull = false, wchar_t Delimiter = 0, bool ignore_showGUID = false, bool detailed = false);
	String __fastcall get_XML_presentation(char* rec, bool ignore_showGUID = false);
	bool __fastcall get_bynary_value(char* buf, bool null, String& value);
	type_fields __fastcall gettype();
	table* __fastcall getparent();
	bool __fastcall getnull_exists();
	int __fastcall getlength();
	int __fastcall getprecision();
	bool __fastcall getcase_sensitive();
	int __fastcall getoffset();
	String __fastcall get_presentation_type();
	bool __fastcall save_blob_to_file(char* rec, String filename, bool unpack);
#ifndef PublicRelease
	unsigned int __fastcall getSortKey(const char* rec, unsigned char* SortKey, int maxlen);
#endif //#ifdef PublicRelease
};

struct index_record
{
	field* field;
	int len;
};

#pragma pack(push)
#pragma pack(1)


// ��������� ����� ������ �������������� ������� ��������-�����
struct unpack_index_record
{
	unsigned int _record_number; // ����� (������) ������ � ������� �������
	unsigned char _index[1]; // �������� ������� ������. �������� ����� �������� ������������ ����� length ������ index
};

// ��������� ��������� ��������-����� ��������
struct branch_page_header{
	unsigned short int flags; // offset 0
	unsigned short int number_indexes; // offset 2
	unsigned int prev_page; // offset 4 // ��� 8.3.8 - ��� ����� �������� (�������� �������� = prev_page * pagesize), �� 8.3.8 - ��� �������� ��������
	unsigned int next_page; // offset 8 // ��� 8.3.8 - ��� ����� �������� (�������� �������� = next_page * pagesize), �� 8.3.8 - ��� �������� ��������
};

// ��������� ��������� ��������-����� ��������
struct leaf_page_header{
	short int flags; // offset 0
	unsigned short int number_indexes; // offset 2
	unsigned int prev_page; // offset 4 // ��� 8.3.8 - ��� ����� �������� (�������� �������� = prev_page * pagesize), �� 8.3.8 - ��� �������� ��������
	unsigned int next_page; // offset 8 // ��� 8.3.8 - ��� ����� �������� (�������� �������� = next_page * pagesize), �� 8.3.8 - ��� �������� ��������
	unsigned short int freebytes; // offset 12
	unsigned int numrecmask; // offset 14
	unsigned short int leftmask; // offset 18
	unsigned short int rightmask; // offset 20
	unsigned short int numrecbits; // offset 22
	unsigned short int leftbits; // offset 24
	unsigned short int rightbits; // offset 26
	unsigned short int recbytes; // offset 28
};

// ��������������� ��������� ��� �������� �������� �� ��������-�����
struct _pack_index_record
{
	unsigned int numrec;
	unsigned int left;
	unsigned int right;
};

#pragma pack(pop)

// �������� ������� ������ � ��������� ������� �������
const short int indexpage_is_root = 1; // ������������� ���� ��������, ��� �������� �������� ��������
const short int indexpage_is_leaf = 2; // ������������� ���� ��������, ��� �������� �������� ������, ����� ������

class index
{
friend table;
private:
	MessageRegistrator* err;
	table* tbase;
	db_ver version; // ������ ����
	unsigned int pagesize; // ������ ����� ������� (�� ������ 8.2.14 ������ 0x1000 (4K), ������� � ������ 8.3.8 �� 0x1000 (4K) �� 0x10000 (64K))

	String name;
	bool is_primary;
	int num_records; // ���������� ����� � �������
	index_record* records;

	uint64_t start; // �������� � ����� �������� ����� �������� �������
	uint64_t rootblock; // �������� � ����� �������� ��������� ����� �������
	unsigned int length; // ����� � ������ ����� ������������� ������ �������
	DynamicArray<unsigned int> recordsindex; // ������������ ������ �������� ������� �� ������ (������ �� ������ ������)
	bool recordsindex_complete; // ������� ������������ recordsindex
	void __fastcall create_recordsindex();

#ifndef PublicRelease
	void __fastcall dump_recursive(v8object* file_index, TFileStream* f, int level, uint64_t curblock);
	void __fastcall delete_index(const char* rec, const unsigned int phys_numrec); // �������� ������� ������ �� ����� index
	void __fastcall delete_index_record(const char* index_buf, const unsigned int phys_numrec); // �������� ������ ������� �� ����� index
	void __fastcall delete_index_record(const char* index_buf, const unsigned int phys_numrec, uint64_t block, bool& is_last_record, bool& page_is_empty, char* new_last_index_buf, unsigned int& new_last_phys_num); // ����������� �������� ������ ������� �� ����� ����� index
	void __fastcall write_index(const unsigned int phys_numrecord, const char* rec); // ������ ������� ������
	void __fastcall write_index_record(const unsigned int phys_numrecord, const char* index_buf); // ������ �������
	void __fastcall write_index_record(const unsigned int phys_numrecord, const char* index_buf, uint64_t block, int& result, char* new_last_index_buf, unsigned int& new_last_phys_num, char* new_last_index_buf2, unsigned int& new_last_phys_num2, uint64_t& new_last_block2); // ����������� ������ �������
#endif //#ifdef PublicRelease

public:
	__fastcall index(table* _base);
	__fastcall ~index();

	String __fastcall getname();
	bool __fastcall get_is_primary();
	int __fastcall get_num_records(); // �������� ���������� ����� � �������
	index_record* __fastcall get_records();

	unsigned int __fastcall get_numrecords(); // �������� ���������� �������, ������������������ ��������
	unsigned int __fastcall get_numrec(unsigned int num_record); // �������� ���������� ������ ������ �� ����������� �������

#ifndef PublicRelease
	void __fastcall dump(String filename);
	void __fastcall calcRecordIndex(const char* rec, char* indexBuf); // ��������� ������ ������ rec � ��������� � indexBuf. ����� ������ indexBuf ������ ���� �� ������ length
#endif //#ifdef PublicRelease

	unsigned int __fastcall get_rootblock();
	unsigned int __fastcall get_length();

	// ������������� ���� ��������-���� ��������
	// ���������� ������ �������� unpack_index_record. ���������� ��������� ������� ������������ � number_indexes
	char* __fastcall unpack_leafpage(uint64_t page_offset, unsigned int& number_indexes);

	// ������������� ���� ��������-���� ��������
	// ���������� ������ �������� unpack_index_record. ���������� ��������� ������� ������������ � number_indexes
	char* __fastcall unpack_leafpage(char* page, unsigned int& number_indexes);

	// ����������� ���� ��������-���� ��������.
	// ���������� ������, ���� �������� �����������, � ����, ���� �������� ����������.
	bool __fastcall pack_leafpage(char* unpack_index, unsigned int number_indexes, char* page_buf);

};

enum table_info
{
	ti_description,
	ti_fields,
	ti_indexes,
	ti_physical_view,
	ti_logical_view
};

// ���� ���������� �������
enum changed_rec_type
{
	crt_not_changed,
	crt_changed,
	crt_insert,
	crt_delete
};

// ��������� �������� ������ �������
class changed_rec
{
public:
	// ��������
	table* parent;

	// ���������� ����� ������ (��� ����������� ������� ��������� ���������� � phys_numrecords)
	unsigned int numrec;

	// ��� ��������� ������ (��������, ���������, �������)
	changed_rec_type changed_type;

	// ��������� ���������� ������ � ������ ���������� �������
	changed_rec* next;

	// ������ ��������� ��������� ���� (�� ������ ����� �� ������ ����, ����� num_fields ����)
	char* fields;

	// ���������� ������. ��� ����� ����� tf_text (TEXT), tf_string (MEMO) � tf_image (BLOB), ���� ��������������� ������� � fields ����������,
	// �������� ��������� �� TStream � ���������� ���� (��� NULL)
	char* rec;

	__fastcall changed_rec(table* _parent, changed_rec_type crt, unsigned int phys_numrecord);
	__fastcall ~changed_rec();
	void __fastcall clear();
};

// ��������� ������ ����� � ����� file_blob
struct blob_block{
	unsigned int nextblock;
	short int length;
	char data[250];
};

// ��������� root ����� ��������/������� ������
struct export_import_table_root
{
	bool has_data;
	bool has_blob;
	bool has_index;
	bool has_descr;
	int data_version_1; // ������ ����������������
	int data_version_2; // ������ ���������
	int blob_version_1; // ������ ����������������
	int blob_version_2; // ������ ���������
	int index_version_1; // ������ ����������������
	int index_version_2; // ������ ���������
	int descr_version_1; // ������ ����������������
	int descr_version_2; // ������ ���������
};

class table{
friend field;
friend class index;
friend changed_rec;
friend T_1CD;
private:
	MessageRegistrator* err;
	T_1CD* base;

	v8object* descr_table; // ������ � ��������� ��������� ������� (������ ��� ������ � 8.0 �� 8.2.14)
	String description;
	String name;
	int num_fields;
	int num_fields2; // ���������� ��������� � ������� fields
	field** fields;
	int num_indexes;
	class index** indexes;
	bool recordlock;
	v8object* file_data;
	v8object* file_blob;
	v8object* file_index;
	int recordlen; // ����� ������ (� ������)
	bool issystem; // ������� ��������� ������� (��� ������� �� ���������� � �������������)
	int lockinmemory; // ������� ���������� � ������

	void __fastcall deletefields();
	void __fastcall deleteindexes();

	changed_rec* ch_rec; // ������ ���������� ������ � ������ ���������� �������
	unsigned int added_numrecords; // ���������� ����������� ������� � ������ ��������������

	unsigned int phys_numrecords; // ���������� ���������� ������� (������ � ����������)
	unsigned int log_numrecords; // ���������� ���������� ������� (������ �� ���������)

	void __fastcall create_file_data(); // �������� ����� file_data
	void __fastcall create_file_blob(); // �������� ����� file_blob
	void __fastcall create_file_index(); // �������� ����� file_index
	void __fastcall refresh_descr_table(); // �������� � ������ ����� �������� �������
#ifndef PublicRelease

	bool edit; // �������, ��� ������� ��������� � ������ ��������������

	void __fastcall delete_data_record(unsigned int phys_numrecord); // �������� ������ �� ����� data
	void __fastcall delete_blob_record(unsigned int blob_numrecord); // �������� ������ �� ����� blob
	void __fastcall delete_index_record(unsigned int phys_numrecord); // �������� ���� �������� ������ �� ����� index
	void __fastcall delete_index_record(unsigned int phys_numrecord, char* rec); // �������� ���� �������� ������ �� ����� index
	void __fastcall write_data_record(unsigned int phys_numrecord, char* rec); // ������ ����� ������ � ���� data
	unsigned int __fastcall write_blob_record(char* blob_record, unsigned int blob_len); // ���������� ����� ������ � ���� blob, ���������� ������ ����� ������
	unsigned int __fastcall write_blob_record(TStream* bstr); //  // ���������� ����� ������ � ���� blob, ���������� ������ ����� ������
	void __fastcall write_index_record(const unsigned int phys_numrecord, const char* rec); // ������ �������� ������ � ���� index
#endif //#ifdef PublicRelease

	bool bad; // ������� ����� �������

public:
	//--> ��������� ������������� ���������� ������� �������
	//DynamicArray<unsigned int> recordsindex; // ������������ ������ �������� ������� �� ������ (������ �� ������ ������)
	unsigned int* recordsindex; // ������ �������� ������� �� ������ (������ �� ������ ������)
	bool recordsindex_complete; // ������� ������������ recordsindex
	unsigned int numrecords_review; // ���������� ������������� ������� ����� � ������ �� ������
	unsigned int numrecords_found; // ���������� ��������� �������� ������� (������� ������ recordsindex)
	//<-- ��������� ������������� ���������� ������� �������
	void __fastcall fillrecordsindex(); // ��������� recordsindex �� �����������


	__fastcall table();
	__fastcall table(T_1CD* _base, int block_descr);
	__fastcall table(T_1CD* _base, String _descr, int block_descr = 0);
	__fastcall ~table();
	void __fastcall init(int block_descr = 0);

	String __fastcall getname();
	String __fastcall getdescription();
	int __fastcall get_numfields();
	int __fastcall get_numindexes();
	field* __fastcall getfield(int numfield);
	class index* __fastcall getindex(int numindex);
	bool __fastcall get_issystem();
	int __fastcall get_recordlen();
	bool __fastcall get_recordlock();

	unsigned int __fastcall get_phys_numrecords(); // ���������� ���������� ������� � ������� �����, ������ � ����������
	unsigned int __fastcall get_log_numrecords(); // ���������� ���������� ������� � ������� �����, ��� ���������
	void __fastcall set_log_numrecords(unsigned int _log_numrecords); //
	unsigned int __fastcall get_added_numrecords();

	char* __fastcall getrecord(unsigned int phys_numrecord, char* buf); // ���������� ��������� �� ������, ����� ����������� ���������� ���������
	TStream* readBlob(TStream* _str, unsigned int _startblock, unsigned int _length, bool rewrite = true);
	unsigned int readBlob(void* _buf, unsigned int _startblock, unsigned int _length);
	void __fastcall set_lockinmemory(bool _lock);
	//bool __fastcall export_to_xml(String filename, index* curindex, bool blob_to_file, bool unpack);
	bool __fastcall export_to_xml(String filename, bool blob_to_file, bool unpack);

	v8object* __fastcall get_file_data();
	v8object* __fastcall get_file_blob();
	v8object* __fastcall get_file_index();

	__int64 __fastcall get_fileoffset(unsigned int phys_numrecord); // �������� ���������� �������� � ����� ������ �� ������

	char* __fastcall get_edit_record(unsigned int phys_numrecord, char* buf); // ���������� ��������� �� ������, ����� ����������� ���������� ���������
	bool __fastcall get_edit();

	unsigned int __fastcall get_phys_numrec(int ARow, class index* cur_index); // �������� ���������� ������ ������ �� ������ ������ �� ���������� �������
	String __fastcall get_file_name_for_field(int num_field, char* rec, unsigned int numrec = 0); // �������� ��� ����� ��-��������� ����������� ���� ���������� ������
	String __fastcall get_file_name_for_record(char* rec); // �������� ��� ����� ��-��������� ���������� ������
	T_1CD* __fastcall getbase(){return base;};

#ifndef PublicRelease
	void __fastcall begin_edit(); // ��������� ������� � ����� ��������������
	void __fastcall cancel_edit(); // ��������� ������� � ����� ��������� � �������� ��� ���������
	void __fastcall end_edit(); // ��������� ������� � ����� ��������� � ��������� ��� ���������
	changed_rec_type __fastcall get_rec_type(unsigned int phys_numrecord);
	changed_rec_type __fastcall get_rec_type(unsigned int phys_numrecord, int numfield);
	void __fastcall set_edit_value(unsigned int phys_numrecord, int numfield, bool null, String value, TStream* st = NULL);
	void __fastcall restore_edit_value(unsigned int phys_numrecord, int numfield);
	void __fastcall set_rec_type(unsigned int phys_numrecord, changed_rec_type crt);

	void __fastcall export_table(String path);
	void __fastcall import_table(String path);
	void __fastcall import_table2(String path);

	void __fastcall delete_record(unsigned int phys_numrecord); // �������� ������
	void __fastcall insert_record(char* rec); // ���������� ������
	void __fastcall update_record(unsigned int phys_numrecord, char* rec, char* changed_fields); // ��������� ������
	char* __fastcall get_record_template_test();
#endif //#ifdef PublicRelease

};

//---------------------------------------------------------------------------
// ��������� ������ ����� �������-���������� ������
struct table_blob_file
{
	unsigned int blob_start;
	unsigned int blob_length;
};

//---------------------------------------------------------------------------
// ��������� ������ ������� ���������� ������
struct table_rec
{
	String name;
	table_blob_file addr;
	int partno;
	FILETIME ft_create;
	FILETIME ft_modify;
};

//---------------------------------------------------------------------------
// ��������� ����� ������� ���������� ������
struct table_file
{
	table* t;
	String name; // ���, ��� ��� �������� � �������
	unsigned int maxpartno;
	table_blob_file* addr;
	FILETIME ft_create;
	FILETIME ft_modify;

	__fastcall table_file(table* _t, const String& _name, unsigned int _maxpartno);
	__fastcall ~table_file();
};

//---------------------------------------------------------------------------
// ����� ������� ���������� ������ (CONFIG, CONFIGSAVE, PARAMS, FILES, CONFICAS, CONFICASSAVE)
class TableFiles
{
private:
	table* tab;
	std::map<String,table_file*> allfiles;
	char* rec;
	bool ready;

	bool __fastcall test_table();
public:
	__fastcall TableFiles(table* t);
	virtual __fastcall ~TableFiles();
	bool __fastcall getready(){return ready;};
	table_file* __fastcall getfile(const String& name);
	table* __fastcall gettable(){return tab;};

	// __property std::map<String,table_file*> files = {read = allfiles};
};

//---------------------------------------------------------------------------
// ����� ������� ���������� ������ (CONFIG, CONFIGSAVE, PARAMS, FILES, CONFICAS, CONFICASSAVE)
class TableFileStream : public TStream
{
private:
	__int64 curoffset;
	table_file* tablefile;
	TStream** streams;
public:
	__fastcall TableFileStream(table_file* tf);
	virtual __fastcall ~TableFileStream();

	virtual int __fastcall Read(void *Buffer, int Count);
	virtual int __fastcall Read(System::DynamicArray<System::Byte> Buffer, int Offset, int Count);
	virtual int __fastcall Write(const void *Buffer, int Count){throw(Exception(L"Write read-only stream"));};
	virtual int __fastcall Write(const System::DynamicArray<System::Byte> Buffer, int Offset, int Count){throw(Exception(L"Write read-only stream"));};
	virtual int __fastcall Seek(int Offset, System::Word Origin);
	virtual __int64 __fastcall Seek(const __int64 Offset, TSeekOrigin Origin);
};


// ����� ������������� ����� � ������
// ������ � ������� ������������ ���� - ���, � �������� �������� ����� ����������
// ��������� � ������� - � ����� ��������� ����������
class memblock
{
friend T_1CD;
private:
	char* buf; // ��������� �� ���� � ������
	static unsigned int pagesize; // ������ ����� ������� (�� ������ 8.2.14 ������ 0x1000 (4K), ������� � ������ 8.3.8 �� 0x1000 (4K) �� 0x10000 (64K))
	unsigned int numblock;
	memblock* next;
	memblock* prev;
	TFileStream* file; // ����, �������� ����������� ����
	bool is_changed; // �������, ��� ���� ������� (������� ������)

	static memblock* first;
	static memblock* last;
	static unsigned int maxcount; // ������������ ���������� ������������ ������
	static unsigned int numblocks; // ���������� �������� ��������� � ������� memblocks (����� ���������� ������ � ����� *.1CD)
	static unsigned int array_numblocks; // ���������� ��������� � ������� memblocks (������ ��� ����� ���������� ������ � ����� *.1CD)
	static unsigned int delta; // ��� ���������� ������� memblocks
	static memblock** memblocks; // ��������� �� ������ ���������� memblock (���������� ����� ���������� ������ � ����� *.1CD)

	unsigned int lastdataget; // ����� (Windows time, � �������������) ���������� ��������� � ������ ������� (data)
	char* __fastcall getblock(bool for_write); // �������� ���� ��� ������ ��� ��� ������
	__fastcall memblock(TFileStream* fs, unsigned int _numblock, bool for_write, bool read);
	__fastcall ~memblock();
	static void __fastcall add_block();
	void __fastcall write();

public:
	static unsigned int count; // ������� ���������� ������������ ������

	static void __fastcall garbage();
	static char* __fastcall getblock(TFileStream* fs, unsigned int _numblock);
	static char* __fastcall getblock_for_write(TFileStream* fs, unsigned int _numblock, bool read);
	static void __fastcall create_memblocks(unsigned int _numblocks);
	static void __fastcall delete_memblocks();
	static unsigned int __fastcall get_numblocks();
	static void __fastcall flush();
};

// ����� ������������ ����������
class SupplierConfig
{
public:
	table_file* file;
	String name; // ��� ������������ ����������
	String supplier; // ������� ������������ ����������
	String version; // ������ ������������ ����������
};

// ��������� ����� ������ ������� ����������� �������� ��������� 8.3 (�� ����� *.ind)

#pragma pack(push)
#pragma pack(1)
struct _datahash
{
	char datahash[20]; // ��� �����
	__int64 offset; // �������� ����� � ����� *.pck
};
#pragma pack(pop)

// ��������� ��� ����� ���� ������ *.ind � *.pck � �������� data\pack\ ��������� 8.3
struct _packdata
{
	TFileStream* pack; // �������� �� ������ ���� *.pck
	unsigned int count; // ���-�� ������� (��������) � ����� *.pck
	_datahash* datahashes; // ������
};

// ���� �������
enum pagetype
{
	pt_lost, // ���������� �������� (�� ��������� �� � ������ �������)
	pt_root, // �������� �������� (�������� 0)
	pt_freeroot, // �������� �������� ������� ��������� ������ (�������� 1)
	pt_freealloc, // �������� ���������� ������� ��������� ������
	pt_free, // ��������� ��������
	pt_rootfileroot, // �������� �������� ��������� ����� (�������� 2)
	pt_rootfilealloc, // �������� ���������� ��������� �����
	pt_rootfile, // �������� ������ ��������� �����
	pt_descrroot, // �������� �������� ����� descr �������
	pt_descralloc, // �������� ���������� ����� descr �������
	pt_descr, // �������� ������ ����� descr �������
	pt_dataroot, // �������� �������� ����� data �������
	pt_dataalloc, // �������� ���������� ����� data �������
	pt_data, // �������� ������ ����� data �������
	pt_indexroot, // �������� �������� ����� index �������
	pt_indexalloc, // �������� ���������� ����� index �������
	pt_index, // �������� ������ ����� index �������
	pt_blobroot, // �������� �������� ����� blob �������
	pt_bloballoc, // �������� ���������� ����� blob �������
	pt_blob // �������� ������ ����� blob �������
};

// ��������� �������������� ��������
struct pagemaprec
{
	int tab; // ������ � T_1CD::tables, -1 - �������� �� ��������� � ��������
	pagetype type; // ��� ��������
	unsigned int number; // ����� �������� � ����� ����
	pagemaprec(){tab = -1; type = pt_lost; number = 0;};
};


// ����� ���� 1CD
class T_1CD
{
friend v8object;
friend table;
friend class index;
friend field;
public:
	static bool recoveryMode;
private:
	MessageRegistrator* err;
	String filename;
	TFileStream* fs;

	db_ver version; // ������ ����
	unsigned int pagesize; // ������ ����� ������� (�� ������ 8.2.14 ������ 0x1000 (4K), ������� � ������ 8.3.8 �� 0x1000 (4K) �� 0x10000 (64K))
	unsigned int length; // ����� ���� � ������
	v8object* free_blocks; // ��������� �����
	v8object* root_object; // �������� ������
	int num_tables; // ���������� ������
	table** tables; // ������� ����
	bool readonly;
//#ifndef PublicRelease
//	ICU* icu;
//#endif //#ifdef PublicRelease
	pagemaprec* pagemap; // ������ ������ length

	TableFiles* _files_config;
	TableFiles* _files_configsave;
	TableFiles* _files_params;
	TableFiles* _files_files;
	TableFiles* _files_configcas;
	TableFiles* _files_configcassave;

	TableFiles* get_files_config();
	TableFiles* get_files_configsave();
	TableFiles* get_files_params();
	TableFiles* get_files_files();
	TableFiles* get_files_configcas();
	TableFiles* get_files_configcassave();

	void __fastcall init();
	bool __fastcall getblock(void* buf, unsigned int block_number, int blocklen = -1); // ����� ����������� ���������� ���������
	char*  __fastcall getblock(unsigned int block_number); // ����� �� ����������� ���������� ������� (����������� memblock)
	char*  __fastcall getblock_for_write(unsigned int block_number, bool read); // ����� �� ����������� ���������� ������� (����������� memblock)
	void __fastcall set_block_as_free(unsigned int block_number); // �������� ���� ��� ���������
	unsigned int __fastcall get_free_block(); // �������� ����� ���������� ����� (� �������� ��� �������)

	void __fastcall add_supplier_config(table_file* file);

	bool __fastcall recursive_test_stream_format(table* t, unsigned int nrec);
	bool __fastcall recursive_test_stream_format2(table* t, unsigned int nrec); // ��� DBSCHEMA
	bool __fastcall recursive_test_stream_format(TStream* str, String path, bool maybezipped2 = false);
	bool __fastcall recursive_test_stream_format(v8catalog* cat, String path);

	void __fastcall pagemapfill();
	String __fastcall pagemaprec_presentation(pagemaprec& pmr);
public:
	char* locale; // ��� ����� ����
	bool is_infobase; // ������� �������������� ����
	bool is_depot; // ������� ��������� ������������

	// ������� �������������� ����
	table* table_config;
	table* table_configsave;
	table* table_params;
	table* table_files;
	table* table_dbschema;
	table* table_configcas;
	table* table_configcassave;
	table* table__extensionsinfo;

//	__property TableFiles* files_config = {read = get_files_config};
//	__property TableFiles* files_configsave = {read = get_files_configsave};
//	__property TableFiles* files_params = {read = get_files_params};
//	__property TableFiles* files_files = {read = get_files_files};
//	__property TableFiles* files_configcas = {read = get_files_configcas};
//	__property TableFiles* files_configcassave = {read = get_files_configcassave};

	// ������� - ��������� ������
	ConfigStorageTableConfig* cs_config;
	ConfigStorageTableConfigSave* cs_configsave;

	// ������� ��������� ������������
	table* table_depot;
	table* table_users;
	table* table_objects;
	table* table_versions;
	table* table_labels;
	table* table_history;
	table* table_lastestversions;
	table* table_externals;
	table* table_selfrefs;
	table* table_outrefs;

	String ver;

	std::vector<SupplierConfig> supplier_configs; // ������������ ����������
	bool supplier_configs_defined; // �������, ��� ��� ���������� ����� ������������ ����������

	__fastcall T_1CD(String _filename, MessageRegistrator* _err = NULL, bool monopoly = true);
	__fastcall T_1CD();
	__fastcall ~T_1CD();
	bool __fastcall is_open();
	int __fastcall get_numtables();
	table* __fastcall gettable(int numtable);
	db_ver __fastcall getversion();

	bool __fastcall save_config(String filename);
	bool __fastcall save_configsave(String filename);
	void __fastcall find_supplier_configs();
	bool __fastcall save_supplier_configs(unsigned int numcon, const String& filename);
	bool __fastcall save_depot_config(const String& _filename, int ver = 0);
	bool __fastcall save_part_depot_config(const String& _filename, int ver_begin, int ver_end);
	int __fastcall get_ver_depot_config(int ver); // ��������� ������ ������ ������������ (0 - ���������, -1 - ������������� � �.�.)
	bool __fastcall save_config_ext(const String& _filename, const TGUID& uid, const String& hashname);
	bool __fastcall save_config_ext_db(const String& _filename, const String& hashname);

	field* get_field(table* tab, String fieldname);
	class index* get_index(table* tab, String indexname);

	bool __fastcall get_readonly();
	void __fastcall set_readonly(bool ro);
	void __fastcall flush();

	bool __fastcall test_stream_format();
	bool __fastcall test_list_of_tables(); // �������� ������ ������ (�� DBNames)
#ifndef PublicRelease
	void __fastcall find_lost_objects();
	void __fastcall find_and_save_lost_objects();
	bool __fastcall create_table(String path); // �������� ������� �� ������ ������� ������
	bool __fastcall delete_table(table* tab);
	bool __fastcall delete_object(v8object* ob);
	bool __fastcall replaceTREF(String mapfile); // ������ �������� ����� ...TREF �� ���� �������� ����
	void __fastcall find_and_create_lost_tables();
	void __fastcall restore_DATA_allocation_table(table* tab);
	bool __fastcall test_block_by_template(unsigned int testblock, char* tt, unsigned int num, int rlen, int len);
#endif //#ifdef PublicRelease
	String& __fastcall getfilename(){return filename;};
	unsigned int getpagesize(){return pagesize;};

};
//---------------------------------------------------------------------------
// ��������� ������ ��������� ������������
enum depot_ver
{
	depotVer3 = 3, // 0300000000000000
	depotVer5 = 5, // 0500000000000000
	depotVer6 = 6, // 0600000000000000
	depotVer7 = 7 // 0700000000000000
};

//---------------------------------------------------------------------------
tree* __fastcall get_treeFromV8file(v8file* f);

//---------------------------------------------------------------------------
#endif

