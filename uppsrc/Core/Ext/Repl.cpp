#include "Ext.h"
#include <iostream>

NAMESPACE_UPP

volatile sig_atomic_t ReplSession::s_interrupted = 0;
bool ReplSession::s_handler_installed = false;

void ReplSession::SignalHandler(int sig)
{
	if(sig == SIGINT) {
		s_interrupted = 1;
	}
}

void ReplSession::InstallSignalHandler()
{
	if(!s_handler_installed) {
		signal(SIGINT, SignalHandler);
		s_handler_installed = true;
	}
}

bool ReplSession::WasInterrupted()
{
	if(s_interrupted) {
		s_interrupted = 0;
		return true;
	}
	return false;
}

void ReplSession::ResetInterrupted()
{
	s_interrupted = 0;
}

ReplSession::ReplSession()
	: prompt("> ")
	, multiline_prompt("... ")
	, history_limit(1000)
	, running(false)
	, eof_reached(false)
{
	InstallSignalHandler();
	RegisterDefaultCommands();
}

ReplSession::~ReplSession()
{
}

void ReplSession::RegisterDefaultCommands()
{
	auto help_cb = [this](const String&) {
		PrintHelp();
	};

	auto quit_cb = [this](const String&) {
		Stop();
	};

	auto hist_cb = [this](const String&) {
		Cout() << "Command history (" << history.GetCount() << " entries):\n";
		for(int i = 0; i < history.GetCount(); i++)
			Cout() << "  " << FormatInt(i + 1) << ": " << history[i] << "\n";
	};

	AddSlashCommand("/help", "Show available slash commands", help_cb);
	AddSlashCommand("/h", "Alias for /help", help_cb);
	AddSlashCommand("/?", "Alias for /help", help_cb);
	AddSlashCommand("/quit", "Exit the REPL session", quit_cb);
	AddSlashCommand("/q", "Alias for /quit", quit_cb);
	AddSlashCommand("/exit", "Alias for /quit", quit_cb);
	AddSlashCommand("/history", "Print recent command history", hist_cb);
}

void ReplSession::AddHistory(const String& cmd)
{
	String trimmed = TrimBoth(cmd);
	if(trimmed.IsEmpty())
		return;

	// Do not duplicate the immediately preceding command
	if(!history.IsEmpty() && history.Top() == trimmed)
		return;

	history.Add(trimmed);
	if(history.GetCount() > history_limit)
		history.Remove(0, history.GetCount() - history_limit);
}

bool ReplSession::SaveHistory(const String& path) const
{
	FileOut out(path);
	if(!out)
		return false;
	for(int i = 0; i < history.GetCount(); i++)
		out.PutLine(history[i]);
	return true;
}

bool ReplSession::LoadHistory(const String& path)
{
	FileIn in(path);
	if(!in)
		return false;
	history.Clear();
	while(!in.IsEof()) {
		String line = in.GetLine();
		if(!line.IsEmpty())
			history.Add(line);
	}
	return true;
}

void ReplSession::AddSlashCommand(const String& name, const String& desc, Event<const String&> cb)
{
	String key = ToLower(TrimBoth(name));
	if(!key.StartsWith("/"))
		key = "/" + key;
	int f = slash_commands.Find(key);
	if(f >= 0) {
		slash_commands[f].description = desc;
		slash_commands[f].callback = cb;
	}
	else {
		CommandInfo& ci = slash_commands.Add(key);
		ci.description = desc;
		ci.callback = cb;
	}
}

bool ReplSession::ProcessSlashCommand(const String& line)
{
	String s = TrimBoth(line);
	if(!s.StartsWith("/"))
		return false;

	int space_idx = s.Find(' ');
	String cmd = (space_idx >= 0) ? s.Mid(0, space_idx) : s;
	String args = (space_idx >= 0) ? TrimBoth(s.Mid(space_idx + 1)) : String();
	cmd = ToLower(cmd);

	int f = slash_commands.Find(cmd);
	if(f >= 0) {
		slash_commands[f].callback(args);
		return true;
	}

	Cout() << "Unknown slash command: '" << cmd << "'. Type '/help' for a list of commands.\n";
	return true;
}

void ReplSession::PrintHelp() const
{
	Cout() << "\nAvailable commands:\n";
	for(int i = 0; i < slash_commands.GetCount(); i++) {
		String key = slash_commands.GetKey(i);
		const CommandInfo& ci = slash_commands[i];
		Cout() << "  " << key;
		for(int sp = key.GetCount(); sp < 16; sp++)
			Cout() << ' ';
		Cout() << ci.description << "\n";
	}
	Cout() << "\n";
}

String ReplSession::ReadLine(const String& custom_prompt)
{
	String p = custom_prompt.IsVoid() || custom_prompt.IsEmpty() ? prompt : custom_prompt;
	Cout() << p;
	Cout().Flush();

	std::string input;
	if(!std::getline(std::cin, input)) {
		if(std::cin.eof()) {
			eof_reached = true;
			Cout() << "\n";
		}
		if(WasInterrupted()) {
			std::cin.clear();
			return String();
		}
		return String();
	}

	if(WasInterrupted()) {
		return String();
	}

	return String(input.c_str());
}

String ReplSession::ReadMultiline(const String& terminator, const String& custom_prompt)
{
	String p = custom_prompt.IsVoid() || custom_prompt.IsEmpty() ? prompt : custom_prompt;
	Cout() << p;
	Cout().Flush();

	String result;
	bool first_line = true;
	bool code_fence_mode = false;

	while(true) {
		std::string input;
		if(!std::getline(std::cin, input)) {
			if(std::cin.eof()) {
				eof_reached = true;
				Cout() << "\n";
			}
			break;
		}

		if(WasInterrupted()) {
			Cout() << "\n[Input cancelled]\n";
			return String();
		}

		String line(input.c_str());

		if(first_line) {
			first_line = false;
			if(line.StartsWith("```")) {
				code_fence_mode = true;
				result << line << "\n";
				Cout() << multiline_prompt;
				Cout().Flush();
				continue;
			}
		}

		if(code_fence_mode) {
			if(TrimBoth(line).StartsWith("```")) {
				result << line << "\n";
				break;
			}
			result << line << "\n";
			Cout() << multiline_prompt;
			Cout().Flush();
			continue;
		}

		if(!terminator.IsEmpty()) {
			if(TrimBoth(line) == terminator)
				break;
		}
		else {
			// Default multiline termination: empty line
			if(line.IsEmpty())
				break;
		}

		result << line << "\n";
		Cout() << multiline_prompt;
		Cout().Flush();
	}

	return result;
}

void ReplSession::Run()
{
	running = true;
	eof_reached = false;
	WhenStart();

	while(running && !eof_reached) {
		String line = ReadLine();
		if(eof_reached)
			break;

		if(WasInterrupted()) {
			Cout() << "\n[Interrupted]\n";
			continue;
		}

		String trimmed = TrimBoth(line);
		if(trimmed.IsEmpty())
			continue;

		if(ProcessSlashCommand(trimmed))
			continue;

		AddHistory(trimmed);
		WhenCommand(trimmed);
	}

	running = false;
	WhenStop();
}

END_UPP_NAMESPACE
