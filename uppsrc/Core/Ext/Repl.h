#ifndef _Core_Repl_Repl_h_
#define _Core_Repl_Repl_h_

#include <Core/Core.h>
#include <signal.h>

#include "CmdArgs.h"

NAMESPACE_UPP

class ReplSession {
public:
	struct CommandInfo {
		String description;
		Event<const String&> callback;
	};

private:
	String                         prompt;
	String                         multiline_prompt;
	Vector<String>                 history;
	int                            history_limit;
	ArrayMap<String, CommandInfo>  slash_commands;
	bool                           running;
	bool                           eof_reached;
	CommandLineArguments           cmd_args;

	static volatile sig_atomic_t   s_interrupted;
	static void                    SignalHandler(int sig);
	static bool                    s_handler_installed;

	void RegisterDefaultCommands();

public:
	ReplSession();
	virtual ~ReplSession();

	ReplSession& Prompt(const String& p)                  { prompt = p; return *this; }
	ReplSession& MultilinePrompt(const String& mp)         { multiline_prompt = mp; return *this; }
	ReplSession& HistoryLimit(int limit)                  { history_limit = limit; return *this; }

	const String& GetPrompt() const                       { return prompt; }
	const String& GetMultilinePrompt() const              { return multiline_prompt; }

	static void InstallSignalHandler();
	static bool WasInterrupted();
	static void ResetInterrupted();

	void AddHistory(const String& cmd);
	const Vector<String>& GetHistory() const              { return history; }
	void ClearHistory()                                   { history.Clear(); }
	bool SaveHistory(const String& path) const;
	bool LoadHistory(const String& path);

	void AddSlashCommand(const String& name, const String& desc, Event<const String&> cb);
	void RemoveSlashCommand(const String& name)           { slash_commands.RemoveKey(name); }
	bool HasSlashCommand(const String& name) const        { return slash_commands.Find(name) >= 0; }
	bool ProcessSlashCommand(const String& line);
	void PrintHelp() const;

	String ReadLine(const String& custom_prompt = String());
	String ReadMultiline(const String& terminator = String(), const String& custom_prompt = String());

	bool IsEof() const                                    { return eof_reached; }
	bool IsRunning() const                                { return running; }
	void Stop()                                           { running = false; }

	CommandLineArguments& Args()                          { return cmd_args; }
	const CommandLineArguments& Args() const              { return cmd_args; }
	bool ParseCommandLine(const Vector<String>& args)     { return cmd_args.Parse(args); }
	bool ParseCommandLine()                               { return cmd_args.Parse(); }

	Event<const String&> WhenCommand;
	Event<>              WhenStart;
	Event<>              WhenStop;

	void Run();
};

END_UPP_NAMESPACE

#endif
