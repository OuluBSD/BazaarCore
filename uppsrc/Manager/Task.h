#ifndef _Manager_Task_h_
#define _Manager_Task_h_

struct Project : Moveable<Project> {
	String title;
	String description;
	String status;              // open, active, archived (free-form)
	String slug;                // filesystem-safe slug; also directory name

	void Jsonize(JsonIO& jio);
};

struct Phase : Moveable<Phase> {
	int    seq = 0;             // 1-based, per-project sequence
	String project;             // owning project slug
	String title;
	String description;
	String status;              // open, in-progress, done, blocked, cancelled (free-form)
	String slug;

	void Jsonize(JsonIO& jio);
};

struct Task : Moveable<Task> {
	int    seq = 0;             // 1-based, per-project sequence
	String project;             // owning project slug
	int    phase_seq = 0;       // required owning phase seq within project
	String title;
	String description;
	String details;
	String instructions;        // planner -> worker: what to do
	String result;              // worker -> planner: outcome + verification evidence
	String status;              // open, in-progress, done, blocked, cancelled (free-form)
	int    link_next_task = 0;  // seq of next task within project; 0 = none
	String slug;

	void Jsonize(JsonIO& jio);
};

String ManagerSlugify(const String& title);

#endif
