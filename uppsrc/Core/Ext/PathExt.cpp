#include "Ext.h"

NAMESPACE_UPP

String AppendUnixFileName(const String& path, const char *filename)
{
	String result = path;
	if(result.GetLength() && *result.Last() != '/' && *filename != '/')
		result += '/';
	result += filename;
	return result;
}

String AppendUnixFileName(String path, String filename)
{
	if(path.GetCount() && path[path.GetCount() - 1] == '/')
		path = path.Left(path.GetCount() - 1);
	if(filename.GetCount() && filename[0] == '/')
		filename = filename.Mid(1);
	return path + "/" + filename;
}

String AppendCpmFileName(const String& path, const char *filename)
{
	String result = path;
	if(result.GetLength() && *result.Last() != '\\' && *filename != '\\')
		result += '\\';
	result += filename;
	return result;
}

END_UPP_NAMESPACE
