#include "Manager.h"

NAMESPACE_UPP

namespace {

// Run a shell command, streaming output to Cout. Returns process exit code.
int RunShell(const String& cmd) {
	Cout() << "$ " << cmd << "\n";
	String out;
	int rc = Sys(cmd, out);
	if(!out.IsEmpty())
		Cout() << out;
	return rc;
}

// Run a shell command quietly and return {exit_code, stdout}.
struct ShellResult {
	int    rc;
	String out;
};

ShellResult RunQuiet(const String& cmd) {
	ShellResult r;
	r.rc = Sys(cmd, r.out);
	return r;
}

// Collect a short summary of pending ./mgr changes for the commit message.
// Falls back to "update Manager state" when git produces nothing usable.
// -uall expands untracked directories into individual untracked files; the
// default would otherwise collapse a fresh ./mgr/ tree into a lone "?? mgr/"
// entry and produce an empty summary on the very first commit.
String SummarizeMgrChanges() {
	ShellResult r = RunQuiet("git status --porcelain -uall -- mgr");
	String status = r.out;
	if(r.rc != 0 || status.IsEmpty())
		return "update Manager state";

	Vector<String> added, modified, deleted;
	Vector<String> lines = Split(status, '\n', false);
	for(const String& ln : lines) {
		if(ln.GetCount() < 4) continue;
		String code = ln.Mid(0, 2);
		String path = TrimBoth(ln.Mid(3));
		// extract basename without extension for readability
		String base = GetFileName(path);
		if(base.EndsWith(".json")) base.Trim(base.GetCount() - 5);

		if(code.Find('?') >= 0 || code.Find('A') >= 0)      added.Add(base);
		else if(code.Find('D') >= 0)                         deleted.Add(base);
		else                                                 modified.Add(base);
	}

	String msg;
	auto join = [](const Vector<String>& v) {
		String s;
		for(int i = 0; i < v.GetCount(); i++) {
			if(i) s << ", ";
			s << v[i];
		}
		return s;
	};

	Vector<String> parts;
	if(!added.IsEmpty())    parts.Add("add " + join(added));
	if(!modified.IsEmpty()) parts.Add("update " + join(modified));
	if(!deleted.IsEmpty())  parts.Add("remove " + join(deleted));

	for(int i = 0; i < parts.GetCount(); i++) {
		if(i) msg << "; ";
		msg << parts[i];
	}
	if(msg.IsEmpty()) msg = "update Manager state";
	return msg;
}

} // namespace

int CmdGitCommit(ManagerStore& store, const Vector<String>& args) {
	// verify we are inside a git work tree
	ShellResult inside = RunQuiet("git rev-parse --is-inside-work-tree");
	if(inside.rc != 0 || inside.out.Find("true") < 0) {
		Cerr() << "Manager: not inside a git repository\n";
		return 1;
	}

	// refuse to commit when there is nothing to stage
	ShellResult pending = RunQuiet("git status --porcelain -uall -- mgr");
	if(pending.rc != 0) {
		Cerr() << "Manager: git status failed\n";
		return 1;
	}
	if(pending.out.IsEmpty()) {
		Cout() << "Manager: no ./mgr changes to commit\n";
		return 0;
	}

	String summary = SummarizeMgrChanges();
	String msg = "Manager: " + summary;

	// stage only ./mgr
	if(RunShell("git add -- mgr") != 0) {
		Cerr() << "Manager: git add failed\n";
		return 1;
	}

	// build commit command with proper shell quoting for the message
	String quoted = msg;
	quoted.Replace("\"", "\\\"");
	String commit_cmd = "git commit -m \"" + quoted + "\"";
	if(RunShell(commit_cmd) != 0) {
		Cerr() << "Manager: git commit failed\n";
		return 1;
	}
	return 0;
}

int CmdGitPull(ManagerStore& store, const Vector<String>& args) {
	String extra;
	for(const auto& a : args) {
		if(!extra.IsEmpty()) extra << " ";
		extra << a;
	}
	String cmd = "git pull";
	if(!extra.IsEmpty()) cmd << " " << extra;
	return RunShell(cmd);
}

int CmdGitPush(ManagerStore& store, const Vector<String>& args) {
	String extra;
	for(const auto& a : args) {
		if(!extra.IsEmpty()) extra << " ";
		extra << a;
	}
	String cmd = "git push";
	if(!extra.IsEmpty()) cmd << " " << extra;
	return RunShell(cmd);
}

END_UPP_NAMESPACE
