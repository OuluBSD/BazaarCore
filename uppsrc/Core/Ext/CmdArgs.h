#ifndef _Core_Ext_CmdArgs_h_
#define _Core_Ext_CmdArgs_h_

#include <Core/Core.h>

NAMESPACE_UPP

struct CmdArg {
	char key;
	String name;
	String desc, value_desc;
	bool has_value;
};

struct CmdInput {
	char key;
	String name;
	String value;
};

class CommandLineArguments {
	static const int max_inputs = 300;
	static const int max_args = 100;
	
	Array<CmdArg>				args;
	Array<CmdInput>				inputs;
	Vector<Value>				positionals, default_positionals;
	Vector<String>				positional_desc, positional_help;
	Vector<dword>				positional_type;
	Value						vars;
	int                         last_index = -1;
	Vector<String>              full_args;
	String                      description;
	String                      help_text;
	bool                        stop_on_positional = false;
	
public:
	CommandLineArguments() {vars = ValueMap();}
	
	void AddArg(char key, const char* desc, bool has_value, String value_desc="value");
	void AddArg(const char* name, char key, const char* desc, bool has_value, String value_desc="value");
	void AddPositional(const char* desc, dword type = UNKNOWN_V, Value def=Value());
	void AddPositional(const char* label, const char* help, dword type = UNKNOWN_V, Value def=Value());
	void SetDescription(const char* desc) { description = desc; }
	void AddHelpText(const char* text) { help_text << text << "\n"; }
	void SetStopOnPositional(bool b = true) { stop_on_positional = b; }

	bool Parse();
	bool Parse(const Vector<String>& args);
	void PrintHelp();
	
	int GetInputCount() const {return inputs.GetCount();}
	int GetPositionalCount() const {return positionals.GetCount();}
	bool IsArg(char c) const;
	bool IsArg(const char* name) const;
	String GetArg(char c) const;
	String GetArg(const char* name) const;
	
	const Array<CmdInput>& GetInputs() const {return inputs;}
	const Vector<Value>& GetPositionals() const {return positionals;}
	Vector<String> GetRest() const;
	Value GetPositional(int i) const {return positionals[i];}
	Value GetVariables() const {return vars;}
	
	Value operator[](const char* str) const {return vars[str];}
	Value operator[](char c) const {return vars[String(c, 1)];}
	
	bool Has(const char* str) const {return !vars[str].IsVoid();}
	bool Has(char c) const {return !vars[String(c, 1)].IsVoid();}
	
	Value Get(const char* str, const Value& def) const {Value v = vars[str]; return v.IsVoid() ? def : v;}
	Value Get(char c, const Value& def) const {Value v = vars[String(c, 1)]; return v.IsVoid() ? def : v;}
	
	template <class T>
	T Get(const char* str, const T& def) const {Value v = vars[str]; return v.IsVoid() ? def : (T)v;}
	template <class T>
	T Get(char c, const T& def) const {Value v = vars[String(c, 1)]; return v.IsVoid() ? def : (T)v;}
};

END_UPP_NAMESPACE

#endif
