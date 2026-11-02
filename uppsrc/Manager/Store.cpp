#include "Manager.h"

NAMESPACE_UPP

ManagerStore::ManagerStore() {
	root_dir = NormalizePath(AppendFileName(GetCurrentDirectory(), "mgr"));
}

String ManagerStore::PadSeq(int seq) {
	return Format("%03d", seq);
}

String ManagerStore::FileNameFor(int seq, const String& slug) {
	return PadSeq(seq) + "-" + slug + ".json";
}

String ManagerStore::ProjectDir(const String& slug) const {
	return AppendFileName(ProjectsDir(), slug);
}

String ManagerStore::ProjectFile(const String& slug) const {
	return AppendFileName(ProjectDir(slug), "project.json");
}

String ManagerStore::PhasesDir(const String& project_slug) const {
	return AppendFileName(ProjectDir(project_slug), "phases");
}

String ManagerStore::TasksDir(const String& project_slug) const {
	return AppendFileName(ProjectDir(project_slug), "tasks");
}

String ManagerStore::PhasePath(const Phase& p) const {
	return AppendFileName(PhasesDir(p.project), FileNameFor(p.seq, p.slug));
}

String ManagerStore::TaskPath(const Task& t) const {
	return AppendFileName(TasksDir(t.project), FileNameFor(t.seq, t.slug));
}

bool ManagerStore::LoadAll() {
	projects.Clear();
	phases.Clear();
	tasks.Clear();

	if(!DirectoryExists(ProjectsDir()))
		return true;  // empty store is valid

	FindFile ff(AppendFileName(ProjectsDir(), "*"));
	while(ff) {
		if(ff.IsDirectory()) {
			String slug = ff.GetName();
			if(slug != "." && slug != "..") {
				if(!LoadProject(slug))
					return false;
			}
		}
		ff.Next();
	}

	Sort(phases, [](const Phase& a, const Phase& b) {
		if(a.project != b.project) return a.project < b.project;
		return a.seq < b.seq;
	});
	Sort(tasks, [](const Task& a, const Task& b) {
		if(a.project != b.project) return a.project < b.project;
		return a.seq < b.seq;
	});
	return true;
}

bool ManagerStore::LoadProject(const String& slug) {
	String proj_path = ProjectFile(slug);
	Project p;
	p.slug = slug;
	if(FileExists(proj_path)) {
		String content = LoadFile(proj_path);
		if(content.IsEmpty()) {
			Cerr() << "Manager: cannot read " << proj_path << "\n";
			return false;
		}
		if(!LoadFromJson(p, content)) {
			Cerr() << "Manager: malformed JSON in " << proj_path << "\n";
			return false;
		}
		// reassert directory-derived slug (path is source of truth)
		p.slug = slug;
	}
	else {
		Cerr() << "Manager: missing " << proj_path << "\n";
		return false;
	}
	projects.Add(p);

	String phases_dir = PhasesDir(slug);
	if(DirectoryExists(phases_dir)) {
		FindFile ff(AppendFileName(phases_dir, "*.json"));
		while(ff) {
			if(ff.IsFile()) {
				String path = ff.GetPath();
				String content = LoadFile(path);
				if(content.IsEmpty()) {
					Cerr() << "Manager: cannot read " << path << "\n";
					return false;
				}
				Phase ph;
				if(!LoadFromJson(ph, content)) {
					Cerr() << "Manager: malformed JSON in " << path << "\n";
					return false;
				}
				ph.project = slug;  // path is source of truth
				phases.Add(ph);
			}
			ff.Next();
		}
	}

	String tasks_dir = TasksDir(slug);
	if(DirectoryExists(tasks_dir)) {
		FindFile ff(AppendFileName(tasks_dir, "*.json"));
		while(ff) {
			if(ff.IsFile()) {
				String path = ff.GetPath();
				String content = LoadFile(path);
				if(content.IsEmpty()) {
					Cerr() << "Manager: cannot read " << path << "\n";
					return false;
				}
				Task t;
				if(!LoadFromJson(t, content)) {
					Cerr() << "Manager: malformed JSON in " << path << "\n";
					return false;
				}
				t.project = slug;
				tasks.Add(t);
			}
			ff.Next();
		}
	}

	return true;
}

// ---------- Projects ----------

bool ManagerStore::SaveProject(const Project& p) {
	String dir = ProjectDir(p.slug);
	if(!RealizeDirectory(dir)) {
		Cerr() << "Manager: failed to create " << dir << "\n";
		return false;
	}
	if(!RealizeDirectory(PhasesDir(p.slug))) {
		Cerr() << "Manager: failed to create " << PhasesDir(p.slug) << "\n";
		return false;
	}
	if(!RealizeDirectory(TasksDir(p.slug))) {
		Cerr() << "Manager: failed to create " << TasksDir(p.slug) << "\n";
		return false;
	}
	String path = ProjectFile(p.slug);
	if(!StoreAsJsonFile(p, path, true)) {
		Cerr() << "Manager: failed to write " << path << "\n";
		return false;
	}
	return true;
}

bool ManagerStore::DeleteProject(const String& slug) {
	String dir = ProjectDir(slug);
	if(!DirectoryExists(dir))
		return false;
	return DeleteFolderDeep(dir);
}

const Project* ManagerStore::FindProject(const String& slug) const {
	for(const auto& p : projects)
		if(p.slug == slug) return &p;
	return nullptr;
}

const Project* ManagerStore::FindProjectByTitle(const String& title) const {
	for(const auto& p : projects)
		if(p.title == title) return &p;
	return nullptr;
}

String ManagerStore::ResolveProjectSlug(const String& slug_or_title) const {
	if(FindProject(slug_or_title))
		return slug_or_title;
	// try slugified version, then title lookup
	String slugified = ManagerSlugify(slug_or_title);
	if(FindProject(slugified))
		return slugified;
	const Project* p = FindProjectByTitle(slug_or_title);
	if(p) return p->slug;
	return String();
}

// ---------- Phases ----------

bool ManagerStore::DeleteByPattern(const String& dir, int seq) {
	if(!DirectoryExists(dir)) return false;
	bool found = false;
	FindFile ff(AppendFileName(dir, PadSeq(seq) + "-*.json"));
	while(ff) {
		if(ff.IsFile()) {
			FileDelete(ff.GetPath());
			found = true;
		}
		ff.Next();
	}
	return found;
}

bool ManagerStore::SavePhase(const Phase& p) {
	String dir = PhasesDir(p.project);
	if(!RealizeDirectory(dir)) {
		Cerr() << "Manager: failed to create " << dir << "\n";
		return false;
	}

	String new_path = PhasePath(p);
	FindFile ff(AppendFileName(dir, PadSeq(p.seq) + "-*.json"));
	while(ff) {
		if(ff.IsFile()) {
			String existing = ff.GetPath();
			if(existing != new_path)
				FileDelete(existing);
		}
		ff.Next();
	}

	if(!StoreAsJsonFile(p, new_path, true)) {
		Cerr() << "Manager: failed to write " << new_path << "\n";
		return false;
	}
	return true;
}

bool ManagerStore::DeletePhase(const String& project_slug, int seq) {
	return DeleteByPattern(PhasesDir(project_slug), seq);
}

const Phase* ManagerStore::FindPhase(const String& project_slug, int seq) const {
	for(const auto& p : phases)
		if(p.project == project_slug && p.seq == seq) return &p;
	return nullptr;
}

const Phase* ManagerStore::FindPhaseByTitle(const String& project_slug, const String& title) const {
	for(const auto& p : phases)
		if(p.project == project_slug && p.title == title) return &p;
	return nullptr;
}

int ManagerStore::NextPhaseSeq(const String& project_slug) const {
	int max_seq = 0;
	for(const auto& p : phases)
		if(p.project == project_slug && p.seq > max_seq) max_seq = p.seq;
	return max_seq + 1;
}

// ---------- Tasks ----------

bool ManagerStore::SaveTask(const Task& t) {
	String dir = TasksDir(t.project);
	if(!RealizeDirectory(dir)) {
		Cerr() << "Manager: failed to create " << dir << "\n";
		return false;
	}

	String new_path = TaskPath(t);
	FindFile ff(AppendFileName(dir, PadSeq(t.seq) + "-*.json"));
	while(ff) {
		if(ff.IsFile()) {
			String existing = ff.GetPath();
			if(existing != new_path)
				FileDelete(existing);
		}
		ff.Next();
	}

	if(!StoreAsJsonFile(t, new_path, true)) {
		Cerr() << "Manager: failed to write " << new_path << "\n";
		return false;
	}
	return true;
}

bool ManagerStore::DeleteTask(const String& project_slug, int seq) {
	return DeleteByPattern(TasksDir(project_slug), seq);
}

const Task* ManagerStore::FindTask(const String& project_slug, int seq) const {
	for(const auto& t : tasks)
		if(t.project == project_slug && t.seq == seq) return &t;
	return nullptr;
}

const Task* ManagerStore::FindTaskByTitle(const String& project_slug, const String& title) const {
	for(const auto& t : tasks)
		if(t.project == project_slug && t.title == title) return &t;
	return nullptr;
}

int ManagerStore::NextTaskSeq(const String& project_slug) const {
	int max_seq = 0;
	for(const auto& t : tasks)
		if(t.project == project_slug && t.seq > max_seq) max_seq = t.seq;
	return max_seq + 1;
}

// ---------- Views ----------

Vector<const Phase*> ManagerStore::PhasesFor(const String& project_slug) const {
	Vector<const Phase*> out;
	for(const auto& p : phases)
		if(p.project == project_slug) out.Add(&p);
	return out;
}

Vector<const Task*> ManagerStore::TasksFor(const String& project_slug) const {
	Vector<const Task*> out;
	for(const auto& t : tasks)
		if(t.project == project_slug) out.Add(&t);
	return out;
}

Vector<const Task*> ManagerStore::TasksForPhase(const String& project_slug, int phase_seq) const {
	Vector<const Task*> out;
	for(const auto& t : tasks)
		if(t.project == project_slug && t.phase_seq == phase_seq) out.Add(&t);
	return out;
}

END_UPP_NAMESPACE
