#include "Manager.h"

using namespace Upp;

namespace {

void ShowHelp() {
	Cout() << "Manager - local project/phase/task tracker\n"
	       << "\n"
	       << "hierarchy: project -> phase -> task (all required)\n"
	       << "\n"
	       << "usage:\n"
	       << "  Manager add    project <title> [field value ...]\n"
	       << "  Manager add    phase   <project> <title> [field value ...]\n"
	       << "  Manager add    task    <project> <phase> <title> [field value ...]\n"
	       << "\n"
	       << "  Manager remove project <slug|title>\n"
	       << "  Manager remove phase   <project> <idx|title>\n"
	       << "  Manager remove task    <project> <phase> <idx|title>\n"
	       << "\n"
	       << "  Manager list   project\n"
	       << "  Manager list   phase   <project>\n"
	       << "  Manager list   task    <project> [<phase>]\n"
	       << "\n"
	       << "  Manager set    project <slug|title>            <field> <value>\n"
	       << "  Manager set    phase   <project> <idx|title>   <field> <value>\n"
	       << "  Manager set    task    <project> <phase> <idx|title> <field> <value>\n"
	       << "\n"
	       << "  Manager print  project <slug|title>\n"
	       << "  Manager print  phase   <project> <idx|title>\n"
	       << "  Manager print  task    <project> <phase> <idx|title>\n"
	       << "\n"
	       << "  Manager next   task    <project>\n"
	       << "                 print the first task with status 'open' (phase seq,\n"
	       << "                 then task seq order), same format as print task.\n"
	       << "                 exits 2 when the queue is empty.\n"
	       << "\n"
	       << "  Manager git    {commit,pull,push} [extra git args]\n"
	       << "\n"
	       << "options:\n"
	       << "  -h, --help             show this help\n"
	       << "  -v, --verbose          verbose output\n"
	       << "\n"
	       << "project fields: title, description, status\n"
	       << "phase fields:   title, description, status\n"
	       << "task fields:    title, description, details, instructions, result,\n"
	       << "                status, link-next-task, phase\n"
	       << "\n"
	       << "data lives under ./mgr/projects/<project-slug>/...\n";
}

} // namespace

#ifdef flagMAIN
CONSOLE_APP_MAIN {
	CommandLineArguments cmd;
	cmd.SetDescription("Local project/phase/task tracker");
	cmd.AddArg('h', "help",    false, "show this help");
	cmd.AddArg("help",    'h', "show this help", false);
	cmd.AddArg('v', "verbose", false, "verbose output");
	cmd.AddArg("verbose", 'v', "verbose output", false);
	// verb-driven grammar: do NOT pre-declare positionals — we want every
	// non-option token to land in GetRest() verbatim.
	cmd.SetStopOnPositional(true);

	if(!cmd.Parse()) {
		ShowHelp();
		SetExitCode(1);
		return;
	}

	if(cmd.IsArg('h') || cmd.IsArg("help")) {
		ShowHelp();
		return;
	}

	Vector<String> argv = cmd.GetRest();
	// GetRest() also picks up the very first positional — but with
	// stop_on_positional the parser halts at it, and last_index points
	// *before* it. Confirm by checking positionals; if the parser consumed
	// one as a Value, prepend it back so the verb is in argv[0].
	for(int i = 0; i < cmd.GetPositionalCount(); i++) {
		Value v = cmd.GetPositional(i);
		argv.Insert(0, v.Is<String>() ? v.ToString() : AsString(v));
	}

	if(argv.IsEmpty()) {
		ShowHelp();
		return;
	}

	String verb = argv[0];
	argv.Remove(0);

	ManagerStore store;
	if(!store.LoadAll()) {
		Cerr() << "Manager: failed to load store from " << store.RootDir() << "\n";
		SetExitCode(1);
		return;
	}

	int rc = 0;

	if(verb == "add" || verb == "remove" || verb == "list" || verb == "set" || verb == "print") {
		if(argv.IsEmpty()) {
			Cerr() << "Manager: " << verb << " requires {project,phase,task}\n";
			ShowHelp();
			SetExitCode(1);
			return;
		}
		String kind = argv[0];
		argv.Remove(0);

		bool is_project = (kind == "project" || kind == "projects");
		bool is_phase   = (kind == "phase"   || kind == "phases");
		bool is_task    = (kind == "task"    || kind == "tasks");
		if(!is_project && !is_phase && !is_task) {
			Cerr() << "Manager: unknown kind '" << kind << "' (expected project, phase or task)\n";
			SetExitCode(1);
			return;
		}

		if(verb == "add") {
			rc = is_project ? CmdAddProject(store, argv)
			     : is_phase ? CmdAddPhase(store, argv)
			     :            CmdAddTask(store, argv);
		}
		else if(verb == "remove") {
			rc = is_project ? CmdRemoveProject(store, argv)
			     : is_phase ? CmdRemovePhase(store, argv)
			     :            CmdRemoveTask(store, argv);
		}
		else if(verb == "list") {
			rc = is_project ? CmdListProjects(store, argv)
			     : is_phase ? CmdListPhases(store, argv)
			     :            CmdListTasks(store, argv);
		}
		else if(verb == "set") {
			rc = is_project ? CmdSetProject(store, argv)
			     : is_phase ? CmdSetPhase(store, argv)
			     :            CmdSetTask(store, argv);
		}
		else /* print */ {
			rc = is_project ? CmdPrintProject(store, argv)
			     : is_phase ? CmdPrintPhase(store, argv)
			     :            CmdPrintTask(store, argv);
		}
	}
	else if(verb == "next") {
		if(argv.IsEmpty()) {
			Cerr() << "Manager: next requires a kind (task)\n";
			ShowHelp();
			SetExitCode(1);
			return;
		}
		String kind = argv[0];
		argv.Remove(0);
		if(kind == "task" || kind == "tasks")
			rc = CmdNextTask(store, argv);
		else {
			Cerr() << "Manager: next is defined for tasks only, not '" << kind << "'\n";
			rc = 1;
		}
	}
	else if(verb == "git") {
		if(argv.IsEmpty()) {
			Cerr() << "Manager: git requires {commit,pull,push}\n";
			ShowHelp();
			SetExitCode(1);
			return;
		}
		String sub = argv[0];
		argv.Remove(0);
		if(sub == "commit")      rc = CmdGitCommit(store, argv);
		else if(sub == "pull")   rc = CmdGitPull(store, argv);
		else if(sub == "push")   rc = CmdGitPush(store, argv);
		else {
			Cerr() << "Manager: unknown git subcommand '" << sub << "'\n";
			rc = 1;
		}
	}
	else if(verb == "help") {
		ShowHelp();
	}
	else {
		Cerr() << "Manager: unknown verb '" << verb << "'\n";
		ShowHelp();
		rc = 1;
	}

	if(rc != 0)
		SetExitCode(rc);
}
#endif
