//---------------------------------------------------------------------------

#ifndef MetaDataH
#define MetaDataH

#include "vector"
#include "map"

#include "NodeTypes.h"
#include "Parse_tree.h"
#include "Common.h"
#include "ConfigStorage.h"
#include "TempStream.h"

//---------------------------------------------------------------------------
extern TGUID EmptyUID;

//---------------------------------------------------------------------------
class MetaBase;
class MetaValue;
class MetaProperty;
struct MetaPropertyLess;
struct GeneratedType;
class Class;
class MetaType;
class MetaTypeSet;
class Value1C;
class Value1C_obj;
class Value1C_metaobj;
class MetaContainer;
class ConfigStorage;
struct VarValidValue;
class Value1C_obj_ExportThread;

//---------------------------------------------------------------------------
// ������ ���������� 1�
// �������� ������ ������ ����������� ������������� �� �����������, ����� ����� ���� ���������� ������ �� >, < � =
enum ContainerVer
{
	cv_2_0 = 1,
	cv_5_0 = 2,
	cv_6_0 = 3,
	cv_106_0 = 4,
	cv_200_0 = 5,
	cv_202_2 = 6,
	cv_216_0 = 7
};

//---------------------------------------------------------------------------
// ������ 1�
// �������� ������ ������ ����������� ������������� �� �����������, ����� ����� ���� ���������� ������ �� >, < � =
enum Version1C
{
	v1C_min = 0,

	v1C_8_0 = 1,
	v1C_8_1 = 2,
	v1C_8_2 = 3,
	v1C_8_2_14 = 4,
	v1C_8_3_1 = 5,
	v1C_8_3_2 = 6,
	v1C_8_3_3 = 7,
	v1C_8_3_4 = 8,
	v1C_8_3_5 = 9,
	v1C_8_3_6 = 10
};

//---------------------------------------------------------------------------
// ������� ��������
enum ExportType
{
	et_default = 0,
	et_catalog = 1,
	et_file = 2
};

//---------------------------------------------------------------------------
// ������� ����� ���������� 1�
class MetaBase
{
protected:
	String fname;
	String fename;
public:
	__fastcall MetaBase(){};
	__fastcall MetaBase(
		const String& _name
		,const String& _ename
	) : fname(_name), fename(_ename){};
	~MetaBase(){};
	void setname(const String& _name){fname = _name;};
	void setename(const String& _ename){fename = _ename;};
	__property String name = {read = fname};
	__property String ename = {read = fename};
	String getname(bool english = false)
	{
		if(english) return fename;
		else return fname;
	};
};

//---------------------------------------------------------------------------
// ���������������� �������� ����������
class MetaValue : public MetaBase
{
protected:
	MetaType* owner;
	int fvalue;
	TGUID fvalueUID;
public:
	__fastcall MetaValue(
		MetaType* _owner,
		const String& _name,
		const String& _ename,
		const int _value
	) : MetaBase(_name, _ename), fvalue(_value), owner(_owner){};
	__fastcall MetaValue(MetaType* _owner, tree* tr);
	__fastcall ~MetaValue(){};
	__property int value = {read = fvalue};
	__property TGUID valueUID = {read = fvalueUID};
	MetaType* __fastcall getowner(){return owner;};
};

//---------------------------------------------------------------------------
// ���� �������� �� ���������
enum DefaultValueType
{
	dvt_novalue = 0, // ��� �������� �� ���������
	dvt_bool = 1, // ������
	dvt_number = 2, // �����
	dvt_string = 3, // ������
	dvt_date = 4, // ����
	dvt_undef = 5, // ������������
	dvt_null = 6, // Null
	dvt_type = 7, // ���
	dvt_enum = 8 // �������� ���������� ������������
};

//---------------------------------------------------------------------------
// �������� ����������
class MetaProperty : public MetaBase
{
protected:
	std::vector<MetaType*> ftypes;
	std::vector<String> fstypes;
	MetaType* owner;
	bool fpredefined;
	ExportType fexporttype;
	Class* f_class;
public:
	DefaultValueType defaultvaluetype;
	union
	{
		bool dv_bool;
		int dv_number;
		String* dv_string;
		char dv_date[7];
		MetaType* dv_type;
		MetaValue* dv_enum;
	};

	__fastcall MetaProperty(
		MetaType* _owner,
		const String& _name,
		const String& _ename
	) : MetaBase(_name, _ename), owner(_owner){};
	__fastcall MetaProperty(MetaType* _owner, tree* tr);
	__fastcall ~MetaProperty();
	void __fastcall filltypes();
	__property std::vector<MetaType*> types = {read = ftypes};
	MetaType* __fastcall getowner(){return owner;};
	__property bool predefined = {read = fpredefined};
	__property ExportType exporttype = {read = fexporttype};
	__property Class* _class = {read = f_class};

};

//---------------------------------------------------------------------------
// ��������� ������� ����������
struct MetaPropertyLess
{
	bool operator()(MetaProperty* const l, MetaProperty* const r) const
	{
		return l->name < r->name;
	}
};

//---------------------------------------------------------------------------
// ������ ����������
class MetaObject : public MetaBase
{
protected:
	String ffullname;
	String fefullname;
	TGUID fuid;
	Value1C_metaobj* fvalue;
public:
	static std::map<TGUID, MetaObject*> map;
	static std::map<String, MetaObject*> smap;

	__fastcall MetaObject(const TGUID& _uid, Value1C_metaobj* _value) : fuid(_uid), fvalue(_value){};
	__fastcall MetaObject(const TGUID& _uid, Value1C_metaobj* _value, const String _name, const String _ename) : MetaBase(_name, _ename), fuid(_uid), fvalue(_value){};
	void setfullname(const String& _fullname){ffullname = _fullname;};
	void setefullname(const String& _efullname){fefullname = _efullname;};
	__property String fullname = {read = ffullname};
	__property String efullname = {read = fefullname};
	__property TGUID uid = {read = fuid};
	__property Value1C_metaobj* value = {read = fvalue};
	String getfullname(bool english = false)
	{
		if(english) return fefullname;
		else return ffullname;
	};
};

//---------------------------------------------------------------------------
// ������������ ���
class MetaGeneratedType : public MetaBase
{
protected:
	bool fwoprefix; // ������� "��� ��������"
public:
	__property bool woprefix = {read = fwoprefix};

	__fastcall MetaGeneratedType(
		const String& _name,
		const String& _ename
	) : MetaBase(_name, _ename), fwoprefix(false)
	{};
	__fastcall MetaGeneratedType(tree* tr);

};

//---------------------------------------------------------------------------
// �����
class MetaRight : public MetaBase
{
protected:
	TGUID fuid;
	Version1C fver1C;
public:
	static std::map<TGUID, MetaRight*> map;
	static std::map<String, MetaRight*> smap;

	//__fastcall MetaRight(const TGUID& _uid) : fuid(_uid){map[fuid] = this;};
	//__fastcall MetaRight(const TGUID& _uid, const String _name, const String _ename) : MetaBase(_name, _ename), fuid(_uid){
	//	map[fuid] = this;
	//	smap[fname] = this;
	//	smap[fename] = this;
	//};
	__fastcall MetaRight(tree* tr);
	static MetaRight* __fastcall getright(const TGUID& _uid);
	static MetaRight* __fastcall getright(const String& _name);
	__property TGUID uid = {read = fuid};
	__property Version1C ver1C = {read = fver1C};
};



//---------------------------------------------------------------------------
// ����������� ��������
class MetaStandartAttribute : public MetaBase
{
protected:
	int fvalue;
	bool fcount;
	int fvaluemax;
	TGUID fuid;
public:
	__fastcall MetaStandartAttribute(
		const String& _name,
		const String& _ename
	) : MetaBase(_name, _ename)
	{fcount = false;};
	__fastcall MetaStandartAttribute(tree* tr);
	__property int value = {read = fvalue};
	__property bool count = {read = fcount};
	__property int valuemax = {read = fvaluemax};
	__property TGUID uid = {read = fuid};
};

//---------------------------------------------------------------------------
// ����������� ��������� �����
class MetaStandartTabularSection : public MetaBase
{
protected:
	int fvalue;
	Class* f_class;
public:
	TGUID class_uid;
	static std::vector<MetaStandartTabularSection*> list;

	__fastcall MetaStandartTabularSection(
		const String& _name,
		const String& _ename
	) : MetaBase(_name, _ename)
	{
		f_class = NULL;
		class_uid = EmptyUID;
	};
	__fastcall MetaStandartTabularSection(tree* tr);
	__property int value = {read = fvalue};
	__property Class* _class = {read = f_class, write = f_class};
};

//---------------------------------------------------------------------------
// ��������� �������
class ClassParameter
{
private:
	String fname;
	static std::map<String, ClassParameter*> map;
public:
	//__fastcall ClassParameter(String _name) : fname(_name){map[fname] = this;};
	__fastcall ClassParameter(tree* tr);
	__property String name = {read = fname};
	static ClassParameter* __fastcall getparam(const String& paramname);
};

//---------------------------------------------------------------------------
// ������
class Class
{
private:
	TGUID fuid;
	std::vector<VarValidValue> fvervalidvalues;
	std::map<ClassParameter*, int> fparamvalues;
	static std::map<TGUID, Class*> map;
	std::vector<MetaStandartAttribute*> fstandartattributes; // ����������� ���������
	std::vector<MetaStandartTabularSection*> fstandarttabularsections; // ����������� ��������� �����

public:
	__fastcall Class(tree* tr);
	__fastcall ~Class();

	__property TGUID uid = {read = fuid};
	__property std::vector<VarValidValue> vervalidvalues = {read = fvervalidvalues};
	__property std::map<ClassParameter*, int> paramvalues = {read = fparamvalues};
	__property std::vector<MetaStandartAttribute*> standartattributes = {read = fstandartattributes};
	__property std::vector<MetaStandartTabularSection*> standarttabularsections = {read = fstandarttabularsections};

	int __fastcall getparamvalue(ClassParameter* p);
	static Class* __fastcall getclass(const TGUID& id);

};

//---------------------------------------------------------------------------
// ��������� ������
class ClassItem
{
private:
	Class* fcl;
	bool fversionisset;
	int fversion;
	int __fastcall setversion(int v);
	int __fastcall getversion();
public:
	__fastcall ClassItem(Class* _cl) : fcl(_cl){fversionisset = false;};
	__property Class* cl = {read = fcl};
	__property int version = {read = getversion, write = setversion};
};

//---------------------------------------------------------------------------
// ���������� �������� ���������� ������ ������������
struct VarValidValue
{
	int value;
	Version1C ver1C;
	int globalvalue;
};

//---------------------------------------------------------------------------
// ���������� ������ ������������
class SerializationTreeVar
{
private:
	String fname;
	bool fcolcount;
	bool fisglobal;
	bool fisfix;
	int ffixvalue;
	std::vector<VarValidValue> fvalidvalues;
public:
	__fastcall SerializationTreeVar(tree* tr);
	__property String name = {read = fname};
	__property bool colcount = {read = fcolcount};
	__property bool isglobal = {read = fisglobal};
	__property bool isfix = {read = fisfix};
	__property int fixvalue = {read = ffixvalue};
	__property std::vector<VarValidValue> validvalues = {read = fvalidvalues};
};


//---------------------------------------------------------------------------
// ���� ����� ������ ������������
enum SerializationTreeNodeType
{
	stt_min = 0, // �������, ��� ��������

	stt_const = 1, // ���������
	stt_var = 2, // ����������
	stt_list = 3, // ������
	stt_prop = 4, // ��������
	stt_elcol = 5, // ����������������
	stt_gentype = 6, // ���������������
	stt_cond = 7, // �������
	stt_metaid = 8, // ������, ������������� ������� ����������
	stt_classcol = 9, // ��������� �������
	stt_class = 10, // �����
	stt_idcol = 11, // ��������� ��-���������
	stt_idel = 12, // ��-�������

	stt_max // ��������, ��� ��������
};

//---------------------------------------------------------------------------
// ���� �������� ������ ������������
enum SerializationTreeValueType
{
	//stv_min = 0, // �������, ��� ��������

	stv_string = 1, // ������
	stv_number = 2, // �����
	stv_uid = 3, // �����������������������
	stv_value = 4, // �������� (MetaValue)
	stv_var = 5, // ���������� (SerializationTreeVar)
	stv_prop = 6, // �������� (MetaProperty)
	stv_vercon = 7, // ������ ����������
	stv_ver1C = 8, // ������ 1C
	stv_classpar = 9, // �������� ������
	stv_globalvar = 10, // ���������� ���������� (SerializationTreeVar)
	stv_none = 11 // ��� ��������

	//stv_max // ��������, ��� ��������
};

//---------------------------------------------------------------------------
// ���� ������� ������ ������������
enum SerializationTreeCondition
{
	stc_min = 0, // �������, ��� ��������

	stc_e = 1, // �����
	stc_ne = 2, // �������
	stc_l = 3, // ������
	stc_g = 4, // ������
	stc_le = 5, // ��������������
	stc_ge = 6, // ��������������
	stc_bs = 7, // �������������
	stc_bn = 8, // ���������������

	stc_max // ��������, ��� ��������
};

//---------------------------------------------------------------------------
// ���� ��������� ������� ������ ������������
enum SerializationTreeClassType
{
	stct_min = 0, // �������, ��� ��������

	stct_inlist = 1, // ������ � ������
	stct_notinlist = 2, // ������ �� � ������

	stct_max // ��������, ��� ��������
};

//---------------------------------------------------------------------------
// ������� ������� ������ ���� (������� �������� ������)
enum ExternalFileFormat
{
	eff_min = 0,

	eff_servalue = 1, // �����������������������
	eff_text = 2, // ����� (�������������)
	eff_tabdoc = 3, // �����������������
	eff_binary = 4, // ��������������
	eff_activedoc = 5, // Active��������
	eff_htmldoc = 6, // HTML��������
	eff_textdoc = 7, // �����������������
	eff_geo = 8, // �������������������
	eff_kd = 9, // ���������������������
	eff_mkd = 10, // �������������������������������
	eff_graf = 11, // ����������������
	eff_xml = 12, // XML
	eff_wsdl = 13, // WSDL
	eff_picture = 14, // ��������
	eff_string = 15, // ������ (������ ������ > maxStringLength)

	eff_max
};

//---------------------------------------------------------------------------
// ���� ������������ �������� ������
enum BinarySerializationType
{
	bst_min = 0,

	bst_empty = 1, // �����������
	bst_base64 = 2, // ������� base64
	bst_data = 3, // ������� data
	bst_base64_or_data = 4, // ������� base64 ��� data (������������ base64)

	bst_max
};

//---------------------------------------------------------------------------
// ���� ������ ������������
struct SerializationTreeNode
{
	SerializationTreeNode* next; // ��������� �� ���� ������
	SerializationTreeNode* first; // ������ �� ����������� ������
	SerializationTreeNode* parent; // ��������
	unsigned int index; // ������ �� ������� ������
	MetaType* owner;

	SerializationTreeNodeType type;
	SerializationTreeCondition condition; // (type == stt_cond)

	SerializationTreeValueType typeval1; // ��� �������� 1 (type == stt_const ��� type == stt_cond ��� type == stt_elcol)
	String str1; // ((type == stt_const ��� type == stt_cond ��� type == stt_var ��� type == stt_elcol) � typeval1 = stv_string ��� typeval1 = stv_var ��� typeval1 = stv_globalvar)
	union
	{
		int num1; // ((type == stt_const ��� type == stt_cond ��� type == stt_elcol) � typeval1 = stv_number)
		TGUID uid1; // (((type == stt_const ��� type == stt_cond) � typeval1 = stv_uid) ��� type == stt_class ��� type == stt_idel)
		MetaValue* val1; // �������� (type == stt_cond � typeval1 = stv_value)
		MetaProperty* prop1; // �������� (type == stt_prop ��� ((type == stt_cond ��� type == stt_elcol) � typeval1 = stv_prop))
		MetaGeneratedType* gentype; // ������������ ��� (type == stt_gentype)
		ContainerVer vercon1; // ������ ���������� (type == stt_cond � typeval1 = stv_vercon)
		Version1C ver1C1; // ������ 1� (type == stt_cond � typeval1 = stv_ver1�)
		SerializationTreeClassType classtype; // ��� ��������� ������� ((type == stt_classcol)
		ClassParameter* classpar1; // �������� ������ (type == stt_cond � typeval1 = stv_classpar)
	};
	MetaType* typeprop; // ��� �������� (type == stt_prop ��� type == stt_elcol ��� type == stt_idel)

	SerializationTreeValueType typeval2; // ��� �������� 2 (type == stt_cond)
	String str2; // (type == stt_cond � (typeval2 = stv_string ��� typeval2 = stv_var ��� typeval2 = stv_globalvar))
	union
	{
		int num2; // (type == stt_cond � typeval2 = stv_number)
		TGUID uid2; // (type == stt_cond � typeval2 = stv_uid)
		MetaValue* val2; // �������� (type == stt_cond � typeval2 = stv_value)
		MetaProperty* prop2; // �������� (type == stt_prop ��� (type == stt_cond � typeval2 = stv_prop))
		ContainerVer vercon2; // ������ ���������� (type == stt_cond � typeval2 = stv_vercon)
		Version1C ver1C2; // ������ 1� ((type == stt_cond � typeval2 = stv_ver1�) ��� type == stt_class ��� type == stt_idel)
		ClassParameter* classpar2; // �������� ������ (type == stt_cond � typeval2 = stv_classpar)
	};

	bool nomove; //
	bool isref; // ������� ������ �� ������ ���������� (�.�. ��� ���� ������ UID, ������ ����������� ������� ���������� ��� ���)
	bool isrefpre; // ������� ������ �� ���������������� ������� (�.�. ��� ���� ������ UID, ������ ����������������� �������� ��� ���)
	bool isright; // ��� �����
	bool exnernal; // ������ ��������� �� ������� �����, ��� ������ UID, ������� �������� ������ �������� �����
	BinarySerializationType binsertype;
	ExternalFileFormat binformat;

	__fastcall SerializationTreeNode()
	{
		next = NULL;
		first = NULL;
		nomove = false;
	};
	__fastcall SerializationTreeNode(MetaType* _owner, tree* tr, SerializationTreeNode* _parent, unsigned int _index);
	__fastcall ~SerializationTreeNode();
	String __fastcall path() const;
	static SerializationTreeNode* __fastcall SerializationTree(MetaType* _owner, tree* tr, SerializationTreeNode* parent);
	static String __fastcall typevalpresentation(SerializationTreeValueType typeval);
	String __fastcall typeval1presentation() const{return typevalpresentation(typeval1);};
	String __fastcall typeval2presentation() const{return typevalpresentation(typeval2);};
};

//---------------------------------------------------------------------------
// ������� ���� ����
struct ExternalFile
{
	MetaType* owner;
	MetaProperty* prop; // ��������
	bool relativepath; // ������� �������������� ���� �����
	String name; // ���
	String ext; // ���������� (��� ����� �������)
	bool catalog; // ������� �������� (���� catalog == true, �� ��� ���� <name+"."+ext+"/"+filename>, ����� <name+"."+ext>)
	String filename; // ��� ����� (������ ���� catalog == true)
	ExternalFileFormat format; // ������ �����
	MetaType* type; // ��� ��������
	bool optional; // ������� ����������������
	Version1C ver1C; // ����������� ������ 1�
	bool havecondition; // ������� ������� �������
	SerializationTreeCondition condition; // (type == stt_cond)

	SerializationTreeValueType typeval1; // ��� �������� 1 (havecondition == true)
	String str1; // typeval1 = stv_string ��� typeval1 = stv_var ��� typeval1 = stv_globalvar
	union
	{
		int num1; // ����� (typeval1 = stv_number)
		TGUID uid1; // ��� (typeval1 = stv_uid)
		MetaValue* val1; // �������� (typeval1 = stv_value)
		MetaProperty* prop1; // �������� (typeval1 = stv_prop)
		ContainerVer vercon1; // ������ ���������� (typeval1 = stv_vercon)
		Version1C ver1C1; // ������ 1� (typeval1 = stv_ver1�)
		ClassParameter* classpar1; // �������� ������ (typeval1 = stv_classpar)
	};

	SerializationTreeValueType typeval2; // ��� �������� 2 (havecondition == true)
	String str2; // typeval2 = stv_string ��� typeval2 = stv_var ��� typeval2 = stv_globalvar
	union
	{
		int num2; // ����� (typeval2 = stv_number)
		TGUID uid2; // ��� (typeval2 = stv_uid)
		MetaValue* val2; // �������� (typeval2 = stv_value)
		MetaProperty* prop2; // �������� (typeval2 = stv_prop)
		ContainerVer vercon2; // ������ ���������� (typeval2 = stv_vercon)
		Version1C ver1C2; // ������ 1� (typeval2 = stv_ver1�)
		ClassParameter* classpar2; // �������� ������ (typeval2 = stv_classpar)
	};

	__fastcall ExternalFile(MetaType* _owner, tree* tr);
	String __fastcall typeval1presentation() const{return SerializationTreeNode::typevalpresentation(typeval1);};
	String __fastcall typeval2presentation() const{return SerializationTreeNode::typevalpresentation(typeval2);};
};

//---------------------------------------------------------------------------
// ��� ����������
class MetaType : public MetaBase
{
private:
	void __fastcall init();
protected:
	MetaTypeSet* typeSet; // ����� �����, �������� ����������� ���� ���

	TGUID fuid; // ��� ����
	bool fhasuid; // ������� ������� ���
	String fmetaname;
	String femetaname;
	String fgentypeprefix;
	String fegentypeprefix;
	unsigned int fserialization_ver; // ������� ������������
	int fimageindex; // ������ ��������
	unsigned int fprenameindex; // ������ ������� ��� �����������������
	unsigned int fpreidindex; // ��������������������������������
	std::vector<MetaProperty*> fproperties; // ��������
	std::map<String, MetaProperty*> fproperties_by_name; // ������������ ���� (������� � ����������) ���������
	std::vector<MetaValue*> fvalues; // ���������������� �������� ����
	std::map<int, MetaValue*> fvalues_by_value; // ������������ �������� �������� ���������������� ���������
	std::map<String, MetaValue*> fvalues_by_name; // ������������ ���� (������� � ����������) ���������������� ���������
	std::vector<MetaType*> fcollectiontypes; // ���� ��������� ���������
	std::vector<String> fscollectiontypes; // ����� ����� ��������� ���������
	std::vector<MetaGeneratedType*> fgeneratedtypes; // ������������ ����
	bool fmeta; // ������� ������� ����������
	ExportType fexporttype;
	Class* fdefaultclass;

	// ������ ������������
	std::vector<SerializationTreeVar*> fserializationvars;
	SerializationTreeNode* fserializationtree; // ���� NULL - ������ ������������ ���
	std::vector<ExternalFile*> fexternalfiles;

public:
	__fastcall MetaType(
		MetaTypeSet* _typeSet,
		const String& _name,
		const String& _ename,
		const String& _metaname,
		const String& _emetaname,
		const String& _uid
	);
	__fastcall MetaType(
		MetaTypeSet* _typeSet,
		const String& _name,
		const String& _ename,
		const String& _metaname,
		const String& _emetaname,
		const TGUID& _uid
	);
	MetaGeneratedType* gentypeRef; // ������������ ��� ������
	__fastcall MetaType(MetaTypeSet* _typeSet, tree* tr);
	__fastcall ~MetaType();
	void __fastcall setSerializationTree(tree* tr);
	__property String metaname = {read = fmetaname};
	__property String emetaname = {read = femetaname};
	__property String gentypeprefix = {read = fgentypeprefix};
	__property String egentypeprefix = {read = fegentypeprefix};
	__property bool hasuid = {read = fhasuid};
	__property TGUID uid = {read = fuid};
	__property unsigned int serialization_ver = {read = fserialization_ver};
	__property int imageindex = {read = fimageindex};
	__property unsigned int prenameindex = {read = fprenameindex};
	__property unsigned int preidindex = {read = fpreidindex};
	__property std::vector<MetaProperty*> properties = {read = fproperties};
	__property std::vector<MetaValue*> values = {read = fvalues};
	__property std::vector<MetaType*> collectiontypes = {read = fcollectiontypes};
	__property std::vector<MetaGeneratedType*> generatedtypes = {read = fgeneratedtypes};
	__property MetaTypeSet* TypeSet = {read = typeSet};
	__property std::vector<SerializationTreeVar*> serializationvars = {read = fserializationvars};
	__property SerializationTreeNode* serializationtree = {read = fserializationtree};
	__property std::vector<ExternalFile*> externalfiles = {read = fexternalfiles};
	__property bool meta = {read = fmeta};
	__property ExportType exporttype = {read = fexporttype};
	__property Class* defaultclass = {read = fdefaultclass};

	MetaProperty* __fastcall getProperty(const String& n);
	MetaProperty* __fastcall getProperty(int index);
	MetaValue* __fastcall getValue(const String& n);
	MetaValue* __fastcall getValue(int value);
	int __fastcall numberOfProperties();
	void __fastcall fillcollectiontypes(); // ��������� ���� ��������� ��������� �� �� ������ (�� fscollectiontypes ��������� fcollectiontypes)
	String getmetaname(bool english = false)
	{
		if(english) return femetaname;
		else return fmetaname;
	}

};

//---------------------------------------------------------------------------
// ����� ����� ���������� (����������� ��� ������������)
class MetaTypeSet
{
private:
	std::map<String, MetaType*> mapname; // ������������ ���� (������� � ����������) �����
	std::map<TGUID, MetaType*> mapuid; // ������������ ��������������� �����
	std::vector<MetaType*> alltype; // ������ ���� �����

public:
	static MetaTypeSet* staticTypes; // C���������� ����
	// ������ ���
	static MetaType* mt_empty;
	// ����������� ����
	static MetaType* mt_string;
	static MetaType* mt_number;
	static MetaType* mt_bool;
	static MetaType* mt_date;
	static MetaType* mt_undef;
	static MetaType* mt_null;
	static MetaType* mt_type;
	// �����������������������
	static MetaType* mt_uid;
	// �����������������
	static MetaType* mt_typedescrinternal;
	// �������� ������
	static MetaType* mt_binarydata;
	// ������������ ���
	static MetaType* mt_arbitrary;
	// �������� ���
	static MetaType* mt_container;
	static MetaType* mt_config;
	// ������-��� ����������� �������
	static MetaType* mt_standart_attribute;
	// ������-��� ����������� ��������� �����
	static MetaType* mt_standart_tabular_section;
	// �������� ������ ���� ��� ������������� ����
	static MetaValue* mv_datefractionsdate;
	static MetaValue* mv_datefractionstime;
	// ��� ���������
	static MetaType* mt_datefractions;
	// �������� ��������� ���� �����������������
	static MetaProperty* mp_datefractions;
	// ����������������: ��������������
	static MetaType* mt_tabularsection;
	// ����������������: ��������
	static MetaType* mt_attribute;
	// ����������������������
	static MetaType* mt_metaobjref;
	// ���������������������������
	static MetaType* mt_metarefint; // ����������� ��� ��� ������� � �������� ������ � �������� ������������
	// ��������������
	static MetaType* mt_tabsection;
	// ����������
	static MetaType* mt_metaref;

	__fastcall MetaTypeSet(){};
	__fastcall ~MetaTypeSet();
	MetaType* __fastcall getTypeByName(const String& n); // �������� ��� �� �����
	MetaType* __fastcall getTypeByUID(const TGUID& u);  // �������� ��� �� ���
	void __fastcall fillall(); //
	void __fastcall add(MetaType* t);
	static void __fastcall staticTypesLoad(TStream* str);
	int __fastcall number();
	MetaType* getType(int index);
};

//---------------------------------------------------------------------------
// ���� �������� 1� (��� Value1C)
enum KindOfValue1C
{
	kv_unknown,   //�������������������� ��������
	kv_bool,      //������
	kv_string,    //������
	kv_number,    //����� �����
	kv_number_exp,//����� � ��������� �������
	kv_date,      //����
	kv_null,      //Null
	kv_undef,     //������������
	kv_type,      //���
	kv_uid,       //���������� �������������
	kv_enum,      //��������� ������������
	kv_stdattr,   //����������� ��������
	kv_stdtabsec, //����������� ��������� �����
	kv_obj,       //������
	kv_metaobj,   //������, ���������� �������� ����������
	kv_refobj,    //������ �� ������, ���������� �������� ����������
	kv_refpre,    //������ �� ���������������� �������
	kv_right,     //�����
	kv_binary,    //�������� ������
	kv_extobj	  //������� ������
};

//---------------------------------------------------------------------------
// �������� ������������� ����
struct GeneratedType
{
	TGUID typeuid; // ��� ����
	TGUID valueuid; // ��� ��������
	__fastcall GeneratedType(tree** ptr, String path);
};

//---------------------------------------------------------------------------
// ���������������� �������
struct PredefinedValue
{
	String name; // ���
	TGUID ref; // ������
	Value1C_metaobj* owner; // �������� ����������������� ��������
	__fastcall PredefinedValue(const String& n, const TGUID& r, Value1C_metaobj* o) : name(n), ref(r), owner(o){};
	__fastcall ~PredefinedValue(){};
	String _fastcall getfullname(bool english = false);
};


//---------------------------------------------------------------------------
// ������� ����� �������� 1�
class Value1C
{
protected:
	static wchar_t indentstring[31];
public:
	Value1C_obj* parent;
	int index; // ������ � �������� (-1 - ��� �� ���������, �� 0 �� (parent->v_objpropv.size() - 1) - ���������� ��������, �� parent->v_objpropv.size() - ������� ���������)
	MetaType* type; // ��� �������� 1�
	KindOfValue1C kind; // ��� ��������� ��������

	__fastcall Value1C(Value1C_obj* _parent);
	virtual __fastcall ~Value1C(){};
	virtual String __fastcall presentation(bool english = false) = 0;
	virtual String __fastcall valuepresentation(bool english = false) = 0;
	virtual bool __fastcall Export(const String& path, TStreamWriter* str, int indent, bool english = false) = 0;
	virtual bool __fastcall isempty() = 0;
	String __fastcall path(MetaContainer* mc, bool english = false);
	String __fastcall fullpath(MetaContainer* mc, bool english = false);
};

//---------------------------------------------------------------------------
// �������� 1� ���� ������
class Value1C_bool : public Value1C
{
public:
	bool v_bool;    //������

	__fastcall Value1C_bool(Value1C_obj* _parent);
	virtual __fastcall ~Value1C_bool(){};
	virtual String __fastcall presentation(bool english = false);
	virtual String __fastcall valuepresentation(bool english = false);
	virtual bool __fastcall Export(const String& path, TStreamWriter* str, int indent, bool english = false);
	virtual bool __fastcall isempty();
};

//---------------------------------------------------------------------------
// �������� 1� ���� ������
class Value1C_string : public Value1C
{
public:
	String v_string;

	__fastcall Value1C_string(Value1C_obj* _parent);
	virtual __fastcall ~Value1C_string(){};
	virtual String __fastcall presentation(bool english = false);
	virtual String __fastcall valuepresentation(bool english = false);
	virtual bool __fastcall Export(const String& path, TStreamWriter* str, int indent, bool english = false);
	virtual bool __fastcall isempty();
};

//---------------------------------------------------------------------------
// �������� 1� ���� �����
class Value1C_number : public Value1C
{
public:
	String v_string;
	int v_number;

	__fastcall Value1C_number(Value1C_obj* _parent);
	virtual __fastcall ~Value1C_number(){};
	virtual String __fastcall presentation(bool english = false);
	virtual String __fastcall valuepresentation(bool english = false);
	virtual bool __fastcall Export(const String& path, TStreamWriter* str, int indent, bool english = false);
	virtual bool __fastcall isempty();
};

//---------------------------------------------------------------------------
// �������� 1� ���� ����� � ��������� �������
class Value1C_number_exp : public Value1C
{
public:
	String v_string;
	double v_number;

	__fastcall Value1C_number_exp(Value1C_obj* _parent);
	virtual __fastcall ~Value1C_number_exp(){};
	virtual String __fastcall presentation(bool english = false);
	virtual String __fastcall valuepresentation(bool english = false);
	virtual bool __fastcall Export(const String& path, TStreamWriter* str, int indent, bool english = false);
	virtual bool __fastcall isempty();
};

//---------------------------------------------------------------------------
// �������� 1� ���� ����
class Value1C_date : public Value1C
{
private:
	static char emptydate[7];
public:
	unsigned char v_date[7];    //����

	__fastcall Value1C_date(Value1C_obj* _parent);
	virtual __fastcall ~Value1C_date(){};
	virtual String __fastcall presentation(bool english = false);
	virtual String __fastcall valuepresentation(bool english = false);
	virtual bool __fastcall Export(const String& path, TStreamWriter* str, int indent, bool english = false);
	virtual bool __fastcall isempty();
};

//---------------------------------------------------------------------------
// �������� 1� ���� NULL
class Value1C_null : public Value1C
{
public:
	__fastcall Value1C_null(Value1C_obj* _parent);
	virtual __fastcall ~Value1C_null(){};
	virtual String __fastcall presentation(bool english = false);
	virtual String __fastcall valuepresentation(bool english = false);
	virtual bool __fastcall Export(const String& path, TStreamWriter* str, int indent, bool english = false);
	virtual bool __fastcall isempty();
};

//---------------------------------------------------------------------------
// �������� 1� ���� ������������
class Value1C_undef : public Value1C
{
public:
	__fastcall Value1C_undef(Value1C_obj* _parent);
	virtual __fastcall ~Value1C_undef(){};
	virtual String __fastcall presentation(bool english = false);
	virtual String __fastcall valuepresentation(bool english = false);
	virtual bool __fastcall Export(const String& path, TStreamWriter* str, int indent, bool english = false);
	virtual bool __fastcall isempty();
};

//---------------------------------------------------------------------------
// �������� 1� ���� ���
class Value1C_type : public Value1C
{
public:
	MetaType* v_type;
	TGUID v_uid;

	__fastcall Value1C_type(Value1C_obj* _parent);
	virtual __fastcall ~Value1C_type(){};
	virtual String __fastcall presentation(bool english = false);
	virtual String __fastcall valuepresentation(bool english = false);
	virtual bool __fastcall Export(const String& path, TStreamWriter* str, int indent, bool english = false);
	virtual bool __fastcall isempty();
};

//---------------------------------------------------------------------------
// �������� 1� ���� �����������������������
class Value1C_uid : public Value1C
{
public:
	TGUID v_uid;

	__fastcall Value1C_uid(Value1C_obj* _parent);
	virtual __fastcall ~Value1C_uid(){};
	virtual String __fastcall presentation(bool english = false);
	virtual String __fastcall valuepresentation(bool english = false);
	virtual bool __fastcall Export(const String& path, TStreamWriter* str, int indent, bool english = false);
	virtual bool __fastcall isempty();
};

//---------------------------------------------------------------------------
// �������� 1� - ��������� ������������
class Value1C_enum : public Value1C
{
public:
	MetaValue* v_enum; // �������� ���������� ������������

	__fastcall Value1C_enum(Value1C_obj* _parent);
	virtual __fastcall ~Value1C_enum(){};
	virtual String __fastcall presentation(bool english = false);
	virtual String __fastcall valuepresentation(bool english = false);
	virtual bool __fastcall Export(const String& path, TStreamWriter* str, int indent, bool english = false);
	virtual bool __fastcall isempty();
};

//---------------------------------------------------------------------------
// �������� 1� - ����������� ��������
class Value1C_stdattr : public Value1C
{
public:
	MetaObject* v_metaobj; // ������ ����������
	MetaStandartAttribute* v_stdattr;
	int v_value;

	__fastcall Value1C_stdattr(Value1C_obj* _parent);
	virtual __fastcall ~Value1C_stdattr(){};
	virtual String __fastcall presentation(bool english = false);
	virtual String __fastcall valuepresentation(bool english = false);
	virtual bool __fastcall Export(const String& path, TStreamWriter* str, int indent, bool english = false);
	virtual bool __fastcall isempty();
};

//---------------------------------------------------------------------------
// �������� 1� - ����������� ��������� �����
class Value1C_stdtabsec : public Value1C
{
public:
	MetaObject* v_metaobj; // ������ ����������
	MetaStandartTabularSection* v_stdtabsec;
	int v_value;

	__fastcall Value1C_stdtabsec(Value1C_obj* _parent);
	virtual __fastcall ~Value1C_stdtabsec(){};
	virtual String __fastcall presentation(bool english = false);
	virtual String __fastcall valuepresentation(bool english = false);
	virtual bool __fastcall Export(const String& path, TStreamWriter* str, int indent, bool english = false);
	virtual bool __fastcall isempty();
};

//---------------------------------------------------------------------------
// �������� 1� ���������� ����
class Value1C_obj : public Value1C
{
protected:
	static bool __fastcall compare(MetaProperty* p, Value1C* v);
public:
	MetaContainer* owner;
	std::map<MetaProperty*, Value1C*, MetaPropertyLess> v_objprop; //������ - ��������� �������
	std::vector<std::pair<MetaProperty*, Value1C*> > v_objpropv; // ��������� ������� � �������
	std::vector<Value1C*> v_objcol; //������ (kv_obj ��� kv_metaobj???) - ��������� ������������� ���������
	std::map<String, int> globalvars; // ��������� ���������� ���������� �� ����������
	void __fastcall fillpropv();

	__fastcall Value1C_obj(Value1C_obj* _parent, MetaContainer* _owner);
	virtual __fastcall ~Value1C_obj();
	virtual String __fastcall presentation(bool english = false);
	virtual String __fastcall valuepresentation(bool english = false);
	virtual Value1C* __fastcall getproperty(MetaProperty* mp);
	virtual Value1C* __fastcall getproperty(const String& prop);
//	String __fastcall path(MetaContainer& mc);
//	String __fastcall fullpath(MetaContainer& mc);
	virtual bool __fastcall Export(const String& path, TStreamWriter* str, int indent, bool english = false);
	virtual bool __fastcall isempty();
	Class* __fastcall getclass(bool immediately = false);
};

//---------------------------------------------------------------------------
// �������� 1� - ������ ����������
class Value1C_metaobj : public Value1C_obj
{
public:
	MetaObject* v_metaobj;
	std::map<MetaGeneratedType*, GeneratedType*> v_objgentypes; //��������� ������������ �����
	std::vector<PredefinedValue*> v_prevalues; // ��������� ���������������� ���������

	__fastcall Value1C_metaobj(Value1C_obj* _parent, MetaContainer* _owner);
	virtual __fastcall ~Value1C_metaobj();
	virtual String __fastcall presentation(bool english = false);
	virtual String __fastcall valuepresentation(bool english = false);
//	String __fastcall path(MetaContainer& mc);
//	String __fastcall fullpath(MetaContainer& mc);
	virtual bool __fastcall Export(const String& path, TStreamWriter* str, int indent, bool english = false);
	virtual bool __fastcall isempty();
};

//---------------------------------------------------------------------------
// �������� 1� - ������ �� ������ ����������
class Value1C_refobj : public Value1C
{
public:
	MetaObject* v_metaobj;
	TGUID v_uid;

	__fastcall Value1C_refobj(Value1C_obj* _parent);
	virtual __fastcall ~Value1C_refobj(){};
	virtual String __fastcall presentation(bool english = false);
	virtual String __fastcall valuepresentation(bool english = false);
	virtual bool __fastcall Export(const String& path, TStreamWriter* str, int indent, bool english = false);
	virtual bool __fastcall isempty();
};

//---------------------------------------------------------------------------
// �������� 1� - ������ �� ���������������� ��������
class Value1C_refpre : public Value1C
{
public:
	PredefinedValue* v_prevalue; // ���������������� �������� (kv_refpre)

	__fastcall Value1C_refpre(Value1C_obj* _parent);
	virtual __fastcall ~Value1C_refpre(){};
	virtual String __fastcall presentation(bool english = false);
	virtual String __fastcall valuepresentation(bool english = false);
	virtual bool __fastcall Export(const String& path, TStreamWriter* str, int indent, bool english = false);
	virtual bool __fastcall isempty();
};

//---------------------------------------------------------------------------
// �������� 1� - ������ �� ���������������� ��������
class Value1C_right : public Value1C
{
public:
	MetaRight* v_right; // �����

	__fastcall Value1C_right(Value1C_obj* _parent);
	virtual __fastcall ~Value1C_right(){};
	virtual String __fastcall presentation(bool english = false);
	virtual String __fastcall valuepresentation(bool english = false);
	virtual bool __fastcall Export(const String& path, TStreamWriter* str, int indent, bool english = false);
	virtual bool __fastcall isempty();
};

//---------------------------------------------------------------------------
// �������� 1� ���� ��������������
class Value1C_binary : public Value1C
{
public:
	//String v_string; //��� ���������� �����
	ExternalFileFormat v_binformat; // ������ �������� ������
	TTempStream* v_binary; // �������� ������

	__fastcall Value1C_binary(Value1C_obj* _parent);
	virtual __fastcall ~Value1C_binary();
	virtual String __fastcall presentation(bool english = false);
	virtual String __fastcall valuepresentation(bool english = false);
	virtual bool __fastcall Export(const String& path, TStreamWriter* str, int indent, bool english = false);
	String get_file_extension();
	virtual bool __fastcall isempty();
};

//---------------------------------------------------------------------------
// �������� 1� - ������� ������
class Value1C_extobj : public Value1C_obj
{
public:
	bool opened; // �������, ������ �� ������� ������ (�.�. ���������, ��� ���)
	String path; // ���� �������� �����
	TGUID metauid;

	__fastcall Value1C_extobj(Value1C_obj* _parent, MetaContainer* _owner, const String& _path, MetaType* _type, TGUID _metauid);
	virtual __fastcall ~Value1C_extobj();

	void __fastcall open();
	void __fastcall close();
	virtual Value1C* __fastcall getproperty(MetaProperty* mp);
	virtual Value1C* __fastcall getproperty(const String& prop);
	virtual bool __fastcall Export(const String& path, TStreamWriter* str, int indent, bool english = false);
	virtual bool __fastcall isempty();
};


//---------------------------------------------------------------------------
// �������� ���������� ������ ������������
struct VarValue
{
	int value;
	bool isset;
	__fastcall VarValue(){value = 0; isset = false;};
};

//---------------------------------------------------------------------------
// ��������� ��������������������� ��������
struct UninitValue1C
{
	Value1C* value; // �������������������� ��������
	TGUID uid; // ��� ��������
	String path; // ����
	TGUID sauid; // ��� ������������ ���������
	Value1C_stdtabsec* metats; // �������� ����������� ��������� ����� ��� ������������ ���������
	__fastcall UninitValue1C(Value1C* v, const String& p, const TGUID& u) : value(v), path(p), uid(u){};
	__fastcall UninitValue1C(Value1C* v, const String& p, const TGUID& u, const TGUID& su, Value1C_stdtabsec* mts) : value(v), path(p), uid(u), sauid(su), metats(mts){};
};

//---------------------------------------------------------------------------
// ��������� ���������� (������������, ������� ���������, ������� �����)
class MetaContainer
{
private:
	Value1C_obj* froot; // �������� ������ ����������
	MetaTypeSet* ftypes; // ����� ������������ �����
	std::map<TGUID, MetaObject*> fmetamap; // ������������ ��� �������� ����������
	std::map<String, MetaObject*> fsmetamap; // ������������ ������� ����� �������� ���������� (�� ���� ������)
	std::map<TGUID, PredefinedValue*> fpredefinedvalues; // ������������ ��� ���������������� ���������
	ContainerVer containerver;
	Version1C ver1C;
	bool useExternal; // ������������ ���������� �������� ������� ������

	// ==> ���������� ������� ��������
	String metaprefix;
	std::vector<UninitValue1C> uninitvalues;
	// <== ���������� ������� ��������

	// ==> ���������� ������� ��������
	unsigned int export_thread_count; // ���������� ������� ��������
	Value1C_obj_ExportThread** export_threads; // ������ ������� ��������
	// <== ���������� ������� ��������

	static TGUID sig_standart_attribute; // ��������� ������������ ���������
	static TGUID sig_standart_table_sec; // ��������� ����������� ��������� �����
	static TGUID sig_ext_dimension; // ��������� ������������ ��������� ��������
	static TGUID sig_ext_dimension_type; // ��������� ������������ ��������� �����������

	Value1C* __fastcall readValue1C(tree** ptr, MetaType* t, Value1C_obj* valparent, const TGUID& metauid, Value1C_stdtabsec*& metats, ClassItem* clitem, String path, bool checkend = false);
	Value1C* __fastcall readValue1C(MetaType* nt, const SerializationTreeNode* tn, tree*& tr, Value1C_obj* valparent, const TGUID& metauid, Value1C_stdtabsec*& metats, ClassItem* clitem, String& path);
	void __fastcall recursiveLoadValue1C(Value1C_obj* v, VarValue* varvalues, tree** ptr, const SerializationTreeNode* tn, TGUID& metauid, Value1C_stdtabsec*& metats, ClassItem* clitem, String& path, bool checkend = false);

	int __fastcall getVarValue(const String& vname, MetaType* t, VarValue* varvalues, ClassItem* clitem, String& path);
	void __fastcall setVarValue(const String& vname, MetaType* t, Value1C_obj* v, VarValue* varvalues, ClassItem* clitem, int value, String& path);

	void __fastcall readPredefinedValues(Value1C_metaobj* v, const int ni, const int ui, const Value1C_obj* vStrings, const String& spath);
public:
	ConfigStorage* stor;
	String storpresent;
	static tree* __fastcall gettree(ConfigStorage* stor, const String& path, bool reporterror = true);
	void __fastcall loadValue1C(Value1C_obj* v, tree** ptr, const SerializationTreeNode* tn, TGUID metauid, Value1C_stdtabsec* metats, ClassItem* clitem, String& path, bool checkend = false);
	void __fastcall inituninitvalues();

	__fastcall MetaContainer(ConfigStorage* _stor, bool _useExternal = false); // ���� _useExternal ������, _stor ����������� MetaContainer � ��������� � �����������. ����� _stor ����������� ���������� �������, � ����� ���� ������ ����� ����� ���������� ������������
	__fastcall ~MetaContainer();

	MetaObject* __fastcall getMetaObject(const String& n); // �������� ������ ���������� �� �����
	MetaObject* __fastcall getMetaObject(const TGUID& u);  // �������� ������ ���������� �� ���
	PredefinedValue* __fastcall getPreValue(const TGUID& u);  // �������� ���������������� ������� �� ���
	bool __fastcall Export(const String& path, bool english = false, unsigned int thread_count = 0);
	bool __fastcall ExportThread(Value1C_obj* v, const String& path, bool english);
	//bool __fastcall ExportThread(Value1C_obj* v, String path, bool english);

	__property Value1C_obj* root = {read = froot};
	__property MetaTypeSet* types = {read = ftypes};
	__property std::map<TGUID, MetaObject*> metamap = {read = fmetamap};
	__property std::map<String, MetaObject*> smetamap = {read = fsmetamap};
	__property std::map<TGUID, PredefinedValue*> predefinedvalues = {read = fpredefinedvalues};

	// ==> ���������� ������� ��������
	long export_work_count; // ���������� ������� ��������
	// <== ���������� ������� ��������

};

//---------------------------------------------------------------------------

class Value1C_obj_ExportThread : public TThread
{
private:
	MetaContainer* owner;
protected:
	void __fastcall Execute();
public:
	String path;
	bool english;
	Value1C_obj* v;
	long busy; // ������� ��������� ������ (0 - ��������, 1 - �����)
	TEvent* work; // ������, ����������, ��� �������� (v, path, english) ����������� � ����� ������� �������
	bool finish; // ������� ��������� ���� ���������.
	int singlethread; // �������-�������, ��� ������� ������� ����� ��������� � ���� �����

	__fastcall Value1C_obj_ExportThread(MetaContainer* _owner);
	__fastcall ~Value1C_obj_ExportThread();

	// ���������-��������� �������� MetaContainer
	std::vector<UninitValue1C> uninitvalues;

};

//---------------------------------------------------------------------------

#endif

