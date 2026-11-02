#include "Manager.h"

NAMESPACE_UPP

void Project::Jsonize(JsonIO& jio) {
	jio("title", title)
	   ("description", description)
	   ("status", status)
	   ("slug", slug)
	   ;
}

void Phase::Jsonize(JsonIO& jio) {
	jio("seq", seq)
	   ("project", project)
	   ("title", title)
	   ("description", description)
	   ("status", status)
	   ("slug", slug)
	   ;
}

void Task::Jsonize(JsonIO& jio) {
	jio("seq", seq)
	   ("project", project)
	   ("phase_seq", phase_seq)
	   ("title", title)
	   ("description", description)
	   ("details", details)
	   ("instructions", instructions)
	   ("result", result)
	   ("status", status)
	   ("link_next_task", link_next_task)
	   ("slug", slug)
	   ;
}

String ManagerSlugify(const String& title) {
	String out;
	bool last_dash = false;
	for(int i = 0; i < title.GetCount(); i++) {
		int c = title[i];
		if((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')) {
			out.Cat(c);
			last_dash = false;
		}
		else if(c >= 'A' && c <= 'Z') {
			out.Cat(c - 'A' + 'a');
			last_dash = false;
		}
		else {
			if(!last_dash && !out.IsEmpty()) {
				out.Cat('-');
				last_dash = true;
			}
		}
	}
	// strip trailing dash
	while(out.GetCount() && out[out.GetCount() - 1] == '-')
		out.Trim(out.GetCount() - 1);
	if(out.IsEmpty())
		out = "untitled";
	return out;
}

END_UPP_NAMESPACE
