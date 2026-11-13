#include "Ext.h"

NAMESPACE_UPP

static Vector<String> local_file_dirs;
static String mlui_server_address;
static void (*mlui_runtime_starter)() = NULL;
static bool mlui_runtime_started = false;

String GetDataDirectoryFile(String file)
{
	return GetDataFile(file);
}

String GetDataDirectory()
{
	// Derive the base from the canonical file resolver so SetDataPath stays honored.
	return GetFileFolder(GetDataFile("__upp_data_directory_probe__"));
}

String GetMluiServerAddress()
{
	return mlui_server_address;
}

void SetMluiServerAddress__(const String& address)
{
	mlui_server_address = address;
}

void SetMluiRuntimeStarter(void (*fn)())
{
	mlui_runtime_starter = fn;
}

void AppStartMluiRuntime__()
{
	if(mlui_runtime_started || !mlui_runtime_starter)
		return;
#ifdef flagMLUI
	mlui_runtime_starter();
	mlui_runtime_started = true;
#else
	if(!mlui_server_address.IsEmpty()) {
		mlui_runtime_starter();
		mlui_runtime_started = true;
	}
#endif
}

String GetCommandLineArgValue(const char *name, const String& def)
{
	if(!name || !*name)
		return def;
	const Vector<String>& cmd = CommandLine();
	String prefix = String(name) + "=";
	for(int i = 0; i < cmd.GetCount(); i++) {
		const String& arg = cmd[i];
		if(arg == name)
			return i + 1 < cmd.GetCount() ? cmd[i + 1] : def;
		if(arg.StartsWith(prefix))
			return arg.Mid(prefix.GetCount());
	}
	return def;
}

int GetCommandLineArgInt(const char *name, int def)
{
	String value = GetCommandLineArgValue(name, Null);
	if(IsNull(value) || value.IsEmpty())
		return def;
	const char *end = NULL;
	int number = ScanInt(value, &end);
	return end && !*end ? number : def;
}

void AddLocalFileDirectory(String dir)
{
	local_file_dirs.Add(dir);
}

String FindLocalFile(String filename)
{
	String exe_dir_file = GetExeDirFile(filename);
	if(FileExists(exe_dir_file))
		return exe_dir_file;

	for(const String& dir : local_file_dirs) {
		String dir_file = AppendFileName(dir, filename);
		if(FileExists(dir_file) || DirectoryExists(dir_file))
			return dir_file;
	}
	return ConfigFile(filename);
}

END_UPP_NAMESPACE
