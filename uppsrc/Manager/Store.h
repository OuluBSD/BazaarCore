#ifndef _Manager_Store_h_
#define _Manager_Store_h_

// Store manages the ./mgr/ directory tree in the current working directory.
// Layout (project-scoped):
//   ./mgr/projects/<project-slug>/project.json
//   ./mgr/projects/<project-slug>/phases/NNN-slug.json
//   ./mgr/projects/<project-slug>/tasks/NNN-slug.json
// NNN is a zero-padded sequence number, independent per project.
// Old flat layouts (./mgr/tasks, ./mgr/phases) are ignored — start fresh.

class ManagerStore {
public:
	ManagerStore();

	String RootDir() const     { return root_dir; }
	String ProjectsDir() const { return AppendFileName(root_dir, "projects"); }

	// Loads all projects (each project subtree fully). Returns false if a
	// file is malformed (offending path is reported via Cerr).
	bool LoadAll();

	// ---------- Projects ----------
	bool SaveProject(const Project& p);
	bool DeleteProject(const String& slug);              // removes whole subtree
	const Project* FindProject(const String& slug) const;
	const Project* FindProjectByTitle(const String& title) const;
	String ResolveProjectSlug(const String& slug_or_title) const;  // "" if not found

	// ---------- Phases (per project) ----------
	String PhasesDir(const String& project_slug) const;
	bool SavePhase(const Phase& p);
	bool DeletePhase(const String& project_slug, int seq);
	const Phase* FindPhase(const String& project_slug, int seq) const;
	const Phase* FindPhaseByTitle(const String& project_slug, const String& title) const;
	int NextPhaseSeq(const String& project_slug) const;

	// ---------- Tasks (per project) ----------
	String TasksDir(const String& project_slug) const;
	bool SaveTask(const Task& t);
	bool DeleteTask(const String& project_slug, int seq);
	const Task* FindTask(const String& project_slug, int seq) const;
	const Task* FindTaskByTitle(const String& project_slug, const String& title) const;
	int NextTaskSeq(const String& project_slug) const;

	// Read-only views
	const Vector<Project>& Projects() const { return projects; }
	Vector<const Phase*> PhasesFor(const String& project_slug) const;
	Vector<const Task*>  TasksFor(const String& project_slug) const;
	Vector<const Task*>  TasksForPhase(const String& project_slug, int phase_seq) const;

	static String FileNameFor(int seq, const String& slug);

private:
	String          root_dir;
	Vector<Project> projects;
	Vector<Phase>   phases;   // all projects' phases; sort by (project, seq)
	Vector<Task>    tasks;    // all projects' tasks;  sort by (project, seq)

	static String PadSeq(int seq);
	String        ProjectDir(const String& slug) const;
	String        ProjectFile(const String& slug) const;
	String        PhasePath(const Phase& p) const;
	String        TaskPath(const Task& t) const;
	bool          LoadProject(const String& slug);
	bool          DeleteByPattern(const String& dir, int seq);
};

#endif
