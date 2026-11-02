#include "Manager.h"

NAMESPACE_UPP

namespace {

int ResolvePhaseSeq(ManagerStore& store, const String& project_slug, const String& id) {
	int seq = ScanInt(id);
	if(seq > 0 && store.FindPhase(project_slug, seq))
		return seq;
	const Phase* p = store.FindPhaseByTitle(project_slug, id);
	if(p) return p->seq;
	return 0;
}

int ResolveTaskSeq(ManagerStore& store, const String& project_slug, const String& id) {
	int seq = ScanInt(id);
	if(seq > 0 && store.FindTask(project_slug, seq))
		return seq;
	const Task* t = store.FindTaskByTitle(project_slug, id);
	if(t) return t->seq;
	return 0;
}

void PrintProject(const Project& p) {
	Cout() << "[" << p.slug << "] " << p.title << "\n";
	Cout() << "    status: " << (p.status.IsEmpty() ? String("open") : p.status) << "\n";
	if(!p.description.IsEmpty()) Cout() << "    description: " << p.description << "\n";
}

void PrintPhase(const Phase& p) {
	Cout() << "[" << p.seq << "] " << p.title << "\n";
	Cout() << "    status: " << (p.status.IsEmpty() ? String("open") : p.status) << "\n";
	if(!p.description.IsEmpty()) Cout() << "    description: " << p.description << "\n";
}

void PrintTask(const Task& t) {
	Cout() << "[" << t.seq << "] " << t.title << "\n";
	Cout() << "    status: " << (t.status.IsEmpty() ? String("open") : t.status) << "\n";
	if(t.phase_seq > 0)           Cout() << "    phase: " << t.phase_seq << "\n";
	if(!t.description.IsEmpty())  Cout() << "    description: " << t.description << "\n";
	if(!t.details.IsEmpty())      Cout() << "    details: " << t.details << "\n";
	if(!t.instructions.IsEmpty()) Cout() << "    instructions: " << t.instructions << "\n";
	if(t.link_next_task > 0)      Cout() << "    next: task " << t.link_next_task << "\n";
}

} // namespace

// ============================================================================
// add
// ============================================================================

int CmdAddProject(ManagerStore& store, const Vector<String>& args) {
	if(args.GetCount() < 1) {
		Cerr() << "usage: Manager add project <title> [field value ...]\n";
		return 1;
	}

	Project p;
	p.title = args[0];
	p.slug = ManagerSlugify(p.title);
	p.status = "open";

	if(store.FindProject(p.slug)) {
		Cerr() << "Manager: project with slug '" << p.slug << "' already exists\n";
		return 1;
	}

	for(int i = 1; i + 1 < args.GetCount(); i += 2) {
		String field = args[i];
		String value = args[i + 1];
		if(field == "title")            { p.title = value; p.slug = ManagerSlugify(value); }
		else if(field == "description") p.description = value;
		else if(field == "status")      p.status = value;
		else {
			Cerr() << "Manager: unknown project field '" << field << "'\n";
			return 1;
		}
	}

	if(!store.SaveProject(p)) return 1;
	Cout() << "added project " << p.slug << " \"" << p.title << "\"\n";
	return 0;
}

int CmdAddPhase(ManagerStore& store, const Vector<String>& args) {
	if(args.GetCount() < 2) {
		Cerr() << "usage: Manager add phase <project> <title> [field value ...]\n";
		return 1;
	}

	String project_slug = store.ResolveProjectSlug(args[0]);
	if(project_slug.IsEmpty()) {
		Cerr() << "Manager: project '" << args[0] << "' not found\n";
		return 1;
	}

	Phase p;
	p.project = project_slug;
	p.seq = store.NextPhaseSeq(project_slug);
	p.title = args[1];
	p.slug = ManagerSlugify(p.title);
	p.status = "open";

	for(int i = 2; i + 1 < args.GetCount(); i += 2) {
		String field = args[i];
		String value = args[i + 1];
		if(field == "title")            { p.title = value; p.slug = ManagerSlugify(value); }
		else if(field == "description") p.description = value;
		else if(field == "status")      p.status = value;
		else {
			Cerr() << "Manager: unknown phase field '" << field << "'\n";
			return 1;
		}
	}

	if(!store.SavePhase(p)) return 1;
	Cout() << "added phase " << p.seq << " \"" << p.title << "\" to project " << project_slug << "\n";
	return 0;
}

int CmdAddTask(ManagerStore& store, const Vector<String>& args) {
	if(args.GetCount() < 3) {
		Cerr() << "usage: Manager add task <project> <phase> <title> [field value ...]\n";
		return 1;
	}

	String project_slug = store.ResolveProjectSlug(args[0]);
	if(project_slug.IsEmpty()) {
		Cerr() << "Manager: project '" << args[0] << "' not found\n";
		return 1;
	}

	int phase_seq = ResolvePhaseSeq(store, project_slug, args[1]);
	if(phase_seq <= 0) {
		Cerr() << "Manager: phase '" << args[1] << "' not found in project " << project_slug << "\n";
		return 1;
	}

	Task t;
	t.project = project_slug;
	t.phase_seq = phase_seq;
	t.seq = store.NextTaskSeq(project_slug);
	t.title = args[2];
	t.slug = ManagerSlugify(t.title);
	t.status = "open";
	t.link_next_task = t.seq + 1;

	for(int i = 3; i + 1 < args.GetCount(); i += 2) {
		String field = args[i];
		String value = args[i + 1];
		if(field == "title")            { t.title = value; t.slug = ManagerSlugify(value); }
		else if(field == "description") t.description = value;
		else if(field == "details")     t.details = value;
		else if(field == "instructions") t.instructions = value;
		else if(field == "result")      t.result = value;
		else if(field == "status")      t.status = value;
		else if(field == "link-next-task") {
			int n = ScanInt(value);
			if(n <= 0) { Cerr() << "Manager: link-next-task must be a positive seq\n"; return 1; }
			t.link_next_task = n;
		}
		else if(field == "phase") {
			int n = ResolvePhaseSeq(store, project_slug, value);
			if(n <= 0) { Cerr() << "Manager: phase '" << value << "' not found\n"; return 1; }
			t.phase_seq = n;
		}
		else {
			Cerr() << "Manager: unknown task field '" << field << "'\n";
			return 1;
		}
	}

	if(!store.SaveTask(t)) return 1;
	Cout() << "added task " << t.seq << " \"" << t.title << "\" to project "
	       << project_slug << " phase " << phase_seq << "\n";
	return 0;
}

// ============================================================================
// remove
// ============================================================================

int CmdRemoveProject(ManagerStore& store, const Vector<String>& args) {
	if(args.GetCount() < 1) {
		Cerr() << "usage: Manager remove project <slug|title>\n";
		return 1;
	}
	String slug = store.ResolveProjectSlug(args[0]);
	if(slug.IsEmpty()) {
		Cerr() << "Manager: project '" << args[0] << "' not found\n";
		return 1;
	}
	if(!store.DeleteProject(slug)) {
		Cerr() << "Manager: failed to delete project " << slug << "\n";
		return 1;
	}
	Cout() << "removed project " << slug << "\n";
	return 0;
}

int CmdRemovePhase(ManagerStore& store, const Vector<String>& args) {
	if(args.GetCount() < 2) {
		Cerr() << "usage: Manager remove phase <project> <idx|title>\n";
		return 1;
	}
	String project_slug = store.ResolveProjectSlug(args[0]);
	if(project_slug.IsEmpty()) {
		Cerr() << "Manager: project '" << args[0] << "' not found\n";
		return 1;
	}
	int seq = ResolvePhaseSeq(store, project_slug, args[1]);
	if(seq <= 0) {
		Cerr() << "Manager: phase '" << args[1] << "' not found in project " << project_slug << "\n";
		return 1;
	}
	// refuse if tasks still point at this phase
	if(!store.TasksForPhase(project_slug, seq).IsEmpty()) {
		Cerr() << "Manager: phase " << seq << " still has tasks in project " << project_slug
		       << " — remove or reassign them first\n";
		return 1;
	}
	if(!store.DeletePhase(project_slug, seq)) {
		Cerr() << "Manager: failed to delete phase " << seq << "\n";
		return 1;
	}
	Cout() << "removed phase " << seq << " from project " << project_slug << "\n";
	return 0;
}

int CmdRemoveTask(ManagerStore& store, const Vector<String>& args) {
	if(args.GetCount() < 3) {
		Cerr() << "usage: Manager remove task <project> <phase> <idx|title>\n";
		return 1;
	}
	String project_slug = store.ResolveProjectSlug(args[0]);
	if(project_slug.IsEmpty()) {
		Cerr() << "Manager: project '" << args[0] << "' not found\n";
		return 1;
	}
	int phase_seq = ResolvePhaseSeq(store, project_slug, args[1]);
	if(phase_seq <= 0) {
		Cerr() << "Manager: phase '" << args[1] << "' not found in project " << project_slug << "\n";
		return 1;
	}
	int seq = ResolveTaskSeq(store, project_slug, args[2]);
	if(seq <= 0) {
		Cerr() << "Manager: task '" << args[2] << "' not found in project " << project_slug << "\n";
		return 1;
	}
	// sanity: verify the task actually belongs to the named phase
	const Task* t = store.FindTask(project_slug, seq);
	ASSERT(t);
	if(t->phase_seq != phase_seq) {
		Cerr() << "Manager: task " << seq << " is in phase " << t->phase_seq
		       << ", not phase " << phase_seq << "\n";
		return 1;
	}
	if(!store.DeleteTask(project_slug, seq)) {
		Cerr() << "Manager: failed to delete task " << seq << "\n";
		return 1;
	}
	Cout() << "removed task " << seq << " from project " << project_slug << "\n";
	return 0;
}

// ============================================================================
// list
// ============================================================================

int CmdListProjects(ManagerStore& store, const Vector<String>& args) {
	const auto& projects = store.Projects();
	if(projects.IsEmpty()) {
		Cout() << "no projects (store: " << store.RootDir() << ")\n";
		return 0;
	}
	Cout() << "projects (" << projects.GetCount() << "):\n";
	for(const auto& p : projects)
		PrintProject(p);
	return 0;
}

int CmdListPhases(ManagerStore& store, const Vector<String>& args) {
	if(args.GetCount() < 1) {
		Cerr() << "usage: Manager list phase <project>\n";
		return 1;
	}
	String project_slug = store.ResolveProjectSlug(args[0]);
	if(project_slug.IsEmpty()) {
		Cerr() << "Manager: project '" << args[0] << "' not found\n";
		return 1;
	}
	Vector<const Phase*> phases = store.PhasesFor(project_slug);
	if(phases.IsEmpty()) {
		Cout() << "no phases in project " << project_slug << "\n";
		return 0;
	}
	Cout() << "phases in project " << project_slug << " (" << phases.GetCount() << "):\n";
	for(const Phase* p : phases)
		PrintPhase(*p);
	return 0;
}

int CmdListTasks(ManagerStore& store, const Vector<String>& args) {
	if(args.GetCount() < 1) {
		Cerr() << "usage: Manager list task <project> [<phase>]\n";
		return 1;
	}
	String project_slug = store.ResolveProjectSlug(args[0]);
	if(project_slug.IsEmpty()) {
		Cerr() << "Manager: project '" << args[0] << "' not found\n";
		return 1;
	}

	Vector<const Task*> tasks;
	String scope_desc = "project " + project_slug;
	if(args.GetCount() >= 2) {
		int phase_seq = ResolvePhaseSeq(store, project_slug, args[1]);
		if(phase_seq <= 0) {
			Cerr() << "Manager: phase '" << args[1] << "' not found in project " << project_slug << "\n";
			return 1;
		}
		tasks = store.TasksForPhase(project_slug, phase_seq);
		scope_desc << " phase " << phase_seq;
	}
	else {
		tasks = store.TasksFor(project_slug);
	}

	if(tasks.IsEmpty()) {
		Cout() << "no tasks in " << scope_desc << "\n";
		return 0;
	}
	Cout() << "tasks in " << scope_desc << " (" << tasks.GetCount() << "):\n";
	for(const Task* t : tasks)
		PrintTask(*t);
	return 0;
}

// ============================================================================
// set
// ============================================================================

int CmdSetProject(ManagerStore& store, const Vector<String>& args) {
	if(args.GetCount() < 3) {
		Cerr() << "usage: Manager set project <slug|title> <field> <value>\n"
		       << "fields: title, description, status\n";
		return 1;
	}
	String slug = store.ResolveProjectSlug(args[0]);
	if(slug.IsEmpty()) {
		Cerr() << "Manager: project '" << args[0] << "' not found\n";
		return 1;
	}
	const Project* existing = store.FindProject(slug);
	ASSERT(existing);
	Project p = *existing;

	String field = args[1];
	String value = args[2];

	if(field == "title") {
		// title rename does NOT rename the slug/directory — slug is the identity
		p.title = value;
	}
	else if(field == "description") p.description = value;
	else if(field == "status")      p.status = value;
	else {
		Cerr() << "Manager: unknown project field '" << field << "'\n";
		return 1;
	}

	if(!store.SaveProject(p)) return 1;
	Cout() << "project " << slug << ": " << field << " = " << value << "\n";
	return 0;
}

int CmdSetPhase(ManagerStore& store, const Vector<String>& args) {
	if(args.GetCount() < 4) {
		Cerr() << "usage: Manager set phase <project> <idx|title> <field> <value>\n"
		       << "fields: title, description, status\n";
		return 1;
	}
	String project_slug = store.ResolveProjectSlug(args[0]);
	if(project_slug.IsEmpty()) {
		Cerr() << "Manager: project '" << args[0] << "' not found\n";
		return 1;
	}
	int seq = ResolvePhaseSeq(store, project_slug, args[1]);
	if(seq <= 0) {
		Cerr() << "Manager: phase '" << args[1] << "' not found in project " << project_slug << "\n";
		return 1;
	}
	const Phase* existing = store.FindPhase(project_slug, seq);
	ASSERT(existing);
	Phase p = *existing;

	String field = args[2];
	String value = args[3];

	if(field == "title")            { p.title = value; p.slug = ManagerSlugify(value); }
	else if(field == "description") p.description = value;
	else if(field == "status")      p.status = value;
	else {
		Cerr() << "Manager: unknown phase field '" << field << "'\n";
		return 1;
	}

	if(!store.SavePhase(p)) return 1;
	Cout() << "phase " << seq << " in " << project_slug << ": " << field << " = " << value << "\n";
	return 0;
}

int CmdSetTask(ManagerStore& store, const Vector<String>& args) {
	if(args.GetCount() < 5) {
		Cerr() << "usage: Manager set task <project> <phase> <idx|title> <field> <value>\n"
		       << "fields: title, description, details, instructions, result, status,\n"
		       << "        link-next-task, phase\n";
		return 1;
	}
	String project_slug = store.ResolveProjectSlug(args[0]);
	if(project_slug.IsEmpty()) {
		Cerr() << "Manager: project '" << args[0] << "' not found\n";
		return 1;
	}
	int phase_seq = ResolvePhaseSeq(store, project_slug, args[1]);
	if(phase_seq <= 0) {
		Cerr() << "Manager: phase '" << args[1] << "' not found in project " << project_slug << "\n";
		return 1;
	}
	int seq = ResolveTaskSeq(store, project_slug, args[2]);
	if(seq <= 0) {
		Cerr() << "Manager: task '" << args[2] << "' not found in project " << project_slug << "\n";
		return 1;
	}
	const Task* existing = store.FindTask(project_slug, seq);
	ASSERT(existing);
	if(existing->phase_seq != phase_seq) {
		Cerr() << "Manager: task " << seq << " is in phase " << existing->phase_seq
		       << ", not phase " << phase_seq << "\n";
		return 1;
	}
	Task t = *existing;

	String field = args[3];
	String value = args[4];

	if(field == "title")            { t.title = value; t.slug = ManagerSlugify(value); }
	else if(field == "description") t.description = value;
	else if(field == "details")     t.details = value;
	else if(field == "instructions") t.instructions = value;
	else if(field == "result")      t.result = value;
	else if(field == "status")      t.status = value;
	else if(field == "link-next-task") {
		int n = ScanInt(value);
		if(n < 0) { Cerr() << "Manager: link-next-task must be a non-negative seq (0 = clear)\n"; return 1; }
		t.link_next_task = n;
	}
	else if(field == "phase") {
		int n = ResolvePhaseSeq(store, project_slug, value);
		if(n <= 0) { Cerr() << "Manager: phase '" << value << "' not found in " << project_slug << "\n"; return 1; }
		t.phase_seq = n;
	}
	else {
		Cerr() << "Manager: unknown task field '" << field << "'\n";
		return 1;
	}

	if(!store.SaveTask(t)) return 1;
	Cout() << "task " << seq << " in " << project_slug << "/" << phase_seq
	       << ": " << field << " = " << value << "\n";
	return 0;
}

// ============================================================================
// print — full single-item inspection
// Unlike list, print shows every field so a missing value is visible.
// Field names match the set-grammar names so output greps 1:1 with set keys.
// ============================================================================

namespace {

String OrEmpty(const String& s) { return s.IsEmpty() ? String("(empty)") : s; }

// Full single-task dump shared by `print task` and `next task`.
void PrintTaskFull(const Task& t) {
	Cout() << "task " << t.project << "/" << t.phase_seq << "/" << t.seq << "\n";
	Cout() << "  title: " << t.title << "\n";
	Cout() << "  status: " << OrEmpty(t.status) << "\n";
	Cout() << "  description: " << OrEmpty(t.description) << "\n";
	Cout() << "  details: " << OrEmpty(t.details) << "\n";
	Cout() << "  instructions: " << OrEmpty(t.instructions) << "\n";
	Cout() << "  result: " << OrEmpty(t.result) << "\n";
	Cout() << "  link-next-task: " << (t.link_next_task > 0 ? AsString(t.link_next_task) : String("none")) << "\n";
	Cout() << "  phase: " << t.phase_seq << "\n";
}

} // namespace

int CmdPrintProject(ManagerStore& store, const Vector<String>& args) {
	if(args.GetCount() < 1) {
		Cerr() << "usage: Manager print project <slug|title>\n";
		return 1;
	}
	String slug = store.ResolveProjectSlug(args[0]);
	if(slug.IsEmpty()) {
		Cerr() << "Manager: project '" << args[0] << "' not found\n";
		return 1;
	}
	const Project* p = store.FindProject(slug);
	ASSERT(p);
	Cout() << "project " << p->slug << "\n";
	Cout() << "  title: " << p->title << "\n";
	Cout() << "  status: " << OrEmpty(p->status) << "\n";
	Cout() << "  description: " << OrEmpty(p->description) << "\n";
	return 0;
}

int CmdPrintPhase(ManagerStore& store, const Vector<String>& args) {
	if(args.GetCount() < 2) {
		Cerr() << "usage: Manager print phase <project> <idx|title>\n";
		return 1;
	}
	String project_slug = store.ResolveProjectSlug(args[0]);
	if(project_slug.IsEmpty()) {
		Cerr() << "Manager: project '" << args[0] << "' not found\n";
		return 1;
	}
	int seq = ResolvePhaseSeq(store, project_slug, args[1]);
	if(seq <= 0) {
		Cerr() << "Manager: phase '" << args[1] << "' not found in project " << project_slug << "\n";
		return 1;
	}
	const Phase* p = store.FindPhase(project_slug, seq);
	ASSERT(p);
	Cout() << "phase " << project_slug << "/" << p->seq << "\n";
	Cout() << "  title: " << p->title << "\n";
	Cout() << "  status: " << OrEmpty(p->status) << "\n";
	Cout() << "  description: " << OrEmpty(p->description) << "\n";
	return 0;
}

int CmdPrintTask(ManagerStore& store, const Vector<String>& args) {
	if(args.GetCount() < 3) {
		Cerr() << "usage: Manager print task <project> <phase> <idx|title>\n";
		return 1;
	}
	String project_slug = store.ResolveProjectSlug(args[0]);
	if(project_slug.IsEmpty()) {
		Cerr() << "Manager: project '" << args[0] << "' not found\n";
		return 1;
	}
	int phase_seq = ResolvePhaseSeq(store, project_slug, args[1]);
	if(phase_seq <= 0) {
		Cerr() << "Manager: phase '" << args[1] << "' not found in project " << project_slug << "\n";
		return 1;
	}
	int seq = ResolveTaskSeq(store, project_slug, args[2]);
	if(seq <= 0) {
		Cerr() << "Manager: task '" << args[2] << "' not found in project " << project_slug << "\n";
		return 1;
	}
	const Task* t = store.FindTask(project_slug, seq);
	ASSERT(t);
	if(t->phase_seq != phase_seq) {
		Cerr() << "Manager: task " << seq << " is in phase " << t->phase_seq
		       << ", not phase " << phase_seq << "\n";
		return 1;
	}
	PrintTaskFull(*t);
	return 0;
}

int CmdNextTask(ManagerStore& store, const Vector<String>& args) {
	if(args.GetCount() < 1) {
		Cerr() << "usage: Manager next task <project>\n"
		       << "prints the first task with status 'open' (phases and tasks in seq\n"
		       << "order) in the same format as 'print task'. Exits 2 when the\n"
		       << "queue is empty so worker scripts can branch without parsing.\n";
		return 1;
	}
	String project_slug = store.ResolveProjectSlug(args[0]);
	if(project_slug.IsEmpty()) {
		Cerr() << "Manager: project '" << args[0] << "' not found\n";
		return 1;
	}
	Vector<const Phase*> phases = store.PhasesFor(project_slug);
	for(const Phase* ph : phases) {
		Vector<const Task*> tasks = store.TasksForPhase(project_slug, ph->seq);
		for(const Task* t : tasks) {
			if(t->status == "open") {
				PrintTaskFull(*t);
				return 0;
			}
		}
	}
	Cout() << "no open tasks in project " << project_slug << "\n";
	return 2;
}

END_UPP_NAMESPACE
