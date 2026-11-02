#ifndef _Manager_Commands_h_
#define _Manager_Commands_h_

// Implements the {add,remove,list,set} {project,phase,task} verb surface.
// Hierarchy: project → phase → task.
//   add project <title> [field value ...]
//   add phase   <project> <title> [field value ...]
//   add task    <project> <phase> <title> [field value ...]
//   remove project <project-slug|title>
//   remove phase   <project> <idx|title>
//   remove task    <project> <phase> <idx|title>
//   list   project
//   list   phase   <project>
//   list   task    <project> [<phase>]
//   set    project <slug|title> <field> <value>
//   set    phase   <project> <idx|title> <field> <value>
//   set    task    <project> <phase> <idx|title> <field> <value>
// Each function returns a process exit code (0 = success).

int CmdAddProject (ManagerStore& store, const Vector<String>& args);
int CmdAddPhase   (ManagerStore& store, const Vector<String>& args);
int CmdAddTask    (ManagerStore& store, const Vector<String>& args);

int CmdRemoveProject (ManagerStore& store, const Vector<String>& args);
int CmdRemovePhase   (ManagerStore& store, const Vector<String>& args);
int CmdRemoveTask    (ManagerStore& store, const Vector<String>& args);

int CmdListProjects (ManagerStore& store, const Vector<String>& args);
int CmdListPhases   (ManagerStore& store, const Vector<String>& args);
int CmdListTasks    (ManagerStore& store, const Vector<String>& args);

int CmdSetProject (ManagerStore& store, const Vector<String>& args);
int CmdSetPhase   (ManagerStore& store, const Vector<String>& args);
int CmdSetTask    (ManagerStore& store, const Vector<String>& args);

int CmdPrintProject (ManagerStore& store, const Vector<String>& args);
int CmdPrintPhase   (ManagerStore& store, const Vector<String>& args);
int CmdPrintTask    (ManagerStore& store, const Vector<String>& args);

// `next` is defined for tasks only: the worker-side "what do I work on" query.
int CmdNextTask     (ManagerStore& store, const Vector<String>& args);

#endif
