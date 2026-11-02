#include "Ext.h"

NAMESPACE_UPP

Vector<String> CommandLineArguments::GetRest() const
{
	Vector<String> rest;
	for(int i = last_index + 1; i < full_args.GetCount(); i++)
		rest.Add(full_args[i]);
	return rest;
}

bool CommandLineArguments::IsArg(char c) const {
	for (const CmdInput& in : inputs) {
		if (in.key == c)
			return true;
	}
	return false;
}

bool CommandLineArguments::IsArg(const char* name) const {
	for (const CmdInput& in : inputs) {
		if (in.name == name)
			return true;
	}
	return false;
}

String CommandLineArguments::GetArg(char c) const {
	for (const CmdInput& in : inputs) {
		if (in.key == c)
			return in.value;
	}
	return String();
}

String CommandLineArguments::GetArg(const char* name) const {
	for (const CmdInput& in : inputs) {
		if (in.name == name)
			return in.value;
	}
	return String();
}

void CommandLineArguments::AddArg(char key, const char* desc, bool has_value, String value_desc) {
	CmdArg& a = args.Add();
	a.key = key;
	a.desc = desc;
	a.has_value = has_value;
	a.value_desc = value_desc;
}

void CommandLineArguments::AddArg(const char* name, char key, const char* desc, bool has_value, String value_desc) {
	CmdArg& a = args.Add();
	a.name = name;
	a.key = key;
	a.desc = desc;
	a.has_value = has_value;
	a.value_desc = value_desc;
}

void CommandLineArguments::AddPositional(const char* desc, dword type, Value def) {
	String label;
	if (desc && *desc)
		label = desc;
	else
		label = Format("arg%d", positional_desc.GetCount() + 1);
	positional_desc.Add(label);
	positional_help.Add("");
	positional_type.Add(type);
	default_positionals.Add(def);
}

void CommandLineArguments::AddPositional(const char* label, const char* help, dword type, Value def) {
	positional_desc.Add(label);
	positional_help.Add(help);
	positional_type.Add(type);
	default_positionals.Add(def);
}

bool CommandLineArguments::Parse() {
	return Parse(CommandLine());
}

bool CommandLineArguments::Parse(const Vector<String>& args) {
	//DUMPC(args);
	full_args <<= args;
	last_index = -1;
	
	inputs.Clear();
	positionals.Clear();
	vars = ValueMap();
	
	for(int i = 0; i < args.GetCount(); i++) {
		last_index = i;
		String arg = args[i];
		
		if (arg.IsEmpty()) {Cerr() << "Invalid argument: empty" << EOL; return false;}
		
		bool found = false;
		
		if (arg[0] == '-') {
			if (arg.GetCount() < 2) {
				Cerr() << "Invalid argument: " << arg << EOL;
				return false;
			}
			
			if (arg[1] == '-') {
				// Long option --name
				String name = arg.Mid(2);
				for(int j = 0; j < this->args.GetCount(); j++) {
					CmdArg& a = this->args[j];
					if (a.name == name) {
						found = true;
						CmdInput& in = inputs.Add();
						in.name = name;
						in.key = a.key;
						if (a.has_value) {
							if (i+1 >= args.GetCount()) {
								Cerr() << "No value provided: " << arg << EOL;
								return false;
							}
							in.value = args[i+1];
							i++;
						}
						break;
					}
				}
				if (!found) {
					Cerr() << "Unknown long option: " << arg << EOL;
					return false;
				}
			}
			else {
				bool is_valid_key = true;
				bool is_var = false;
				int key_size = 0;
				for(int k = 1; k < arg.GetCount(); k++) {
					char c = arg[k];
					if (k > 1 && c == '=') {
						is_var = true;
						break;
					}
					if (!IsAlpha(c) && !IsDigit(c) && c != '_') {
						is_valid_key = false;
						break;
					}
					++key_size;
				}
				if (is_valid_key && is_var) {
					String key = arg.Mid(1, key_size);
					String value = arg.Mid(2+key_size);
					CParser p(value);
					if (p.IsDouble())
						vars(key) = p.ReadDouble();
					else if (p.IsInt())
						vars(key) = p.ReadInt64();
					else
						vars(key) = value;
					found = true;
				}
				else {
					char key = arg[1];
					if (key) {
						for(int j = 0; j < this->args.GetCount(); j++) {
							CmdArg& a = this->args[j];
							if (a.key == key) {
								found = true;
								CmdInput& in = inputs.Add();
								in.key = key;
								in.name = a.name;
								if (a.has_value) {
									if (i+1 >= args.GetCount()) {
										Cerr() << "No value provided: " << arg << EOL;
										return false;
									}
									in.value = args[i+1];
									i++;
								}
								break;
							}
						}
					}
				}
			}
		}
		else {
			int positional_index = positionals.GetCount();
			if (!positional_desc.IsEmpty() && positional_index >= positional_desc.GetCount()) {
				last_index--; // Revert last_index so GetRest() includes this arg
				return true;
			}
			Value positional = arg; 
			// Only try ParseJSON if it looks like JSON object/array
			if((arg.StartsWith("{") && arg.EndsWith("}")) || (arg.StartsWith("[") && arg.EndsWith("]"))) {
				Value v = ParseJSON(arg);
				if(!IsError(v)) positional = v;
			}
			
			if (positional_index < positional_type.GetCount()) {
				dword expected = positional_type[positional_index];
				if (expected != UNKNOWN_V) {
					// If we have an expected type, we SHOULD try to parse it even if it doesn't look like JSON (e.g. simple number)
					Value v = ParseJSON(arg);
					if(IsError(v)) v = arg;
					positional = v;
					
					auto fail_type = [&](const char* detail) {
						Cerr() << "Invalid positional argument";
						if (positional_index < positional_desc.GetCount())
							Cerr() << " for " << positional_desc[positional_index];
						Cerr() << ": " << detail << EOL;
						return false;
					};
					auto ensure_int_range = [&](int64 value) -> bool {
						if (value < INT_MIN || value > INT_MAX)
							return fail_type("integer out of range");
						positional = Value((int)value);
						return true;
					};
					auto ensure_int64 = [&](int64 value) -> bool {
						positional = Value(value);
						return true;
					};
					auto ensure_double = [&](double value, bool is_float) -> bool {
						if (IsNaN(value))
							return fail_type("not-a-number not allowed");
						if (is_float)
							positional = Value((float)value);
						else
							positional = Value(value);
						return true;
					};
					auto ensure_float = [&](double value, bool is_float) -> bool {
						if (IsNaN(value))
							return fail_type("not-a-number not allowed");
						positional = Value((float)value);
						return true;
					};
					
					switch(expected) {
					case INT_V:
						if (positional.Is<int>())
							break;
						if (positional.Is<int64>()) {
							if (!ensure_int_range((int64)positional))
								return false;
							break;
						}
						if (positional.Is<double>() || positional.Is<float>()) {
							double d = (double)positional;
							if (d != (double)(int64)d)
								return fail_type("expected integer");
							if (!ensure_int_range((int64)d))
								return false;
							break;
						}
						if (positional.Is<bool>()) {
							positional = Value((int)((bool)positional ? 1 : 0));
							break;
						}
						return fail_type("expected integer");
					
					case INT64_V:
						if (positional.Is<int64>())
							break;
						if (positional.Is<int>()) {
							positional = Value((int64)(int)positional);
							break;
						}
						if (positional.Is<double>() || positional.Is<float>()) {
							double d = (double)positional;
							if (d != (double)(int64)d)
								return fail_type("expected 64-bit integer");
							if (!ensure_int64((int64)d))
								return false;
							break;
						}
						if (positional.Is<bool>()) {
							positional = Value((int64)((bool)positional ? 1 : 0));
							break;
						}
						return fail_type("expected 64-bit integer");
					
					case DOUBLE_V:
						if (positional.Is<double>())
							break;
						if (positional.Is<float>() || positional.Is<int>() || positional.Is<int64>() || positional.Is<bool>()) {
							double d = positional.Is<bool>() ? ((bool)positional ? 1.0 : 0.0) : (double)positional;
							if (!ensure_double(d, false))
								return false;
							break;
						}
						return fail_type("expected double-precision number");
					
					case FLOAT_V:
						if (positional.Is<float>())
							break;
						if (positional.Is<double>() || positional.Is<int>() || positional.Is<int64>() || positional.Is<bool>()) {
							double d = positional.Is<bool>() ? ((bool)positional ? 1.0 : 0.0) : (double)positional;
							if (!ensure_float(d, true))
								return false;
							break;
						}
						return fail_type("expected floating point number");
					
                                        case STRING_V: positional = arg; break;
					case BOOL_V:
						if (positional.Is<bool>())
							break;
						if (positional.Is<int>() || positional.Is<int64>()) {
							positional = Value((bool)((int64)positional != 0));
							break;
						}
						if (positional.Is<double>() || positional.Is<float>()) {
							double d = (double)positional;
							if (IsNaN(d))
								return fail_type("not-a-number not allowed");
							positional = Value((bool)(d != 0.0));
							break;
						}
						return fail_type("expected boolean");
					
					default:
						if (positional.GetType() != expected)
							return fail_type("unexpected type");
						break;
					}
				}
			}
			positionals.Add(positional);
			found = true;
			if (stop_on_positional) return true;
		}
		
		if (!found) {Cerr() << "Invalid argument: " << arg << EOL; return false;}
	}
	
	for(int i = positionals.GetCount(); i < default_positionals.GetCount(); i++) {
		Value def = default_positionals[i];
		if (def.IsVoid()) {
			// Cerr() << "not all positional arguments were given" << EOL;
			return false;
		}
		else positionals.Add(def);
	}
	
	return true;
}

void CommandLineArguments::PrintHelp() {
	String exe = GetExeFilePath();
	int i = exe.ReverseFind(DIR_SEPS);
	if (i >= 0)
		exe = exe.Mid(i+1);
	
	Stream& cout = Cout();
	cout << "usage: " << exe;
	
	// Print options in usage line
	for(int i = 0; i < args.GetCount(); i++) {
		const CmdArg& a = args[i];
		cout << " [";
		if (a.key) cout << "-" << a.key;
		if (a.key && !a.name.IsEmpty()) cout << "|";
		if (!a.name.IsEmpty()) cout << "--" << a.name;
		if (a.has_value) cout << " " << a.value_desc;
		cout << "]";
	}
	
	// Print positionals in usage line
	for (int i = 0; i < positional_desc.GetCount(); i++) {
		cout << " " << positional_desc[i];
	}
	
	cout << EOL << EOL;
	if (!description.IsEmpty())
		cout << description << EOL << EOL;

	if (positional_desc.GetCount() > 0) {
		cout << "positional arguments:" << EOL;
		for (int i = 0; i < positional_desc.GetCount(); i++) {
			cout << "  " << Format("%-60s", positional_desc[i]) << positional_help[i] << EOL;
		}
		cout << EOL;
	}
	
	if (!help_text.IsEmpty())
		cout << help_text << EOL;

	if (args.GetCount() > 0) {
		cout << "options:" << EOL;
		for (int i = 0; i < args.GetCount(); i++) {
			const CmdArg& a = args[i];
			String s = "  ";
			if (a.key) s << "-" << a.key;
			if (a.key && !a.name.IsEmpty()) s << ", ";
			if (!a.name.IsEmpty()) s << "--" << a.name;
			if (a.has_value) s << " " << a.value_desc;
			
			cout << Format("%-60s", s) << a.desc << EOL;
		}
		cout << EOL;
	}
}



END_UPP_NAMESPACE
