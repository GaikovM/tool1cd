//$$---- axlib proj source ---- (stAXLibProjectSource)
//#include <vcl.h>
#include <sysutils.hpp>
//#pragma hdrstop
#include <atl\atlvcl.h>

#include "Cv8cfImpl.h"
#pragma package(smart_init)
TComModule  v8cfModule;
TComModule &_Module = v8cfModule;

param parStr(varOleStr);
param parInt(varInteger);
param parBoolDef(varBoolean, false);
param parStrDef(varOleStr, "");
param parStrStr[] = {parStr,parStr};
param parStrInt[] = {parStr,parInt};
param parStrStrInt[] = {parStr,parStr,parInt};
param parStrBoolDef[] = {parStr,parBoolDef};
param parStrStrStr[] = {parStr,parStr,parStr};

method TCv8cf2Impl::methods[] = {
	method("�������", "Open", true, 2, parStrBoolDef),
	method("�������", "Close", false, 0, NULL),
	method("��������������", "FileExist", true, 1, &parStr),
	method("�����������������", "CatalogExist", true, 1, &parStr),
	method("��������������������", "GetFileList", true, 1, &parStrDef),
	method("��������������������������", "GetFileOnlyList", true, 1, &parStrDef),
	method("�����������������������������", "GetCatalogList", true, 1, &parStrDef),
	method("����������", "IsCatalog", true, 1, &parStr),
	method("���������", "Read", true, 1, &parStr),
	method("��������", "Write", true, 2, parStrStr),
	method("�������", "Delete", true, 1, &parStr),
	method("�����������", "Inflate", true, 2, parStrStr),
	method("���������", "Deflate", true, 2, parStrStr),
	method("��������", "TypeOfFile", true, 1, &parStr),
	method("����������������", "TypeOfExternFile", true, 1, &parStr),
	method("���������������", "ReadAsType", true, 2, parStrInt),
	method("����������������������", "ReadExternalAsType", true, 2, parStrInt),
	method("��������������", "WriteAsType", true, 3, parStrStrInt),
	method("���������������������", "WriteExternAsType", true, 3, parStrStrInt),
	method("����������������������", "ReadToExternalFile", true, 2, parStrStr),
	method("�����������������������", "WriteFromExternalFile", true, 2, parStrStr),
	method("�������������������������", "ExtractExternalZipFile", true, 2, parStrStr),
	method("���������������������������������", "ExtractFileFromExternalZipFile", true, 3, parStrStrStr)
};

int TCv8cf2Impl::nMet = sizeof(methods) / sizeof(methods[0]);

property TCv8cf2Impl::properties[] = {
	property("������", "Opened", true, false),
	property("��������", "FileName", true, false)
};

int TCv8cf2Impl::nProp = sizeof(properties) / sizeof(properties[0]);


// The ATL Object map holds an array of _ATL_OBJMAP_ENTRY structures that
// described the objects of your OLE server. The MAP is handed to your
// project's CComModule-derived _Module object via the Init method.
//
BEGIN_OBJECT_MAP(ObjectMap)
  OBJECT_ENTRY(CLSID_Cv8cf2, TCv8cf2Impl)
END_OBJECT_MAP()




// Entry point of your Server invoked by Windows for processes or threads are
// initialized or terminated.
//
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void*)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		_Module.Init(ObjectMap, hinst);
		DisableThreadLibraryCalls(hinst);
	}
	return TRUE;
}

// _Module.Term is typically invoked from the DLL_PROCESS_DETACH of your
// DllEntryPoint. However, this may result in an incorrect shutdown sequence.
// Instead an Exit routine is setup to invoke the cleanup routine
// CComModule::Term.
//
void ModuleTerm(void)
{
	_Module.Term();
}
#pragma exit ModuleTerm 63

// Entry point of your Server invoked to inquire whether the DLL is no
// longer in use and should be unloaded.
//
STDAPI __export DllCanUnloadNow(void)
{
	return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

// Entry point of your Server allowing OLE to retrieve a class object from
// your Server
//
STDAPI __export DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	return _Module.GetClassObject(rclsid, riid, ppv);
}

// Entry point of your Server invoked to instruct the server to create
// registry entries for all classes supported by the module
//
STDAPI __export DllRegisterServer(void)
{
	return _Module.RegisterServer(TRUE);
}

// Entry point of your Server invoked to instruct the server to remove
// all registry entries created through DllRegisterServer.
//
STDAPI __export DllUnregisterServer(void)
{
	return _Module.UnregisterServer();
}
//---------------------------------------------------------------------------
