#ifndef _Manager_GitCommands_h_
#define _Manager_GitCommands_h_

// Git integration for the Manager CLI.
// `commit` stages only ./mgr/ and writes an auto-generated commit message.
// `pull` / `push` are repo-wide pass-throughs (git does not support path-
// scoped push/pull).

int CmdGitCommit(ManagerStore& store, const Vector<String>& args);
int CmdGitPull  (ManagerStore& store, const Vector<String>& args);
int CmdGitPush  (ManagerStore& store, const Vector<String>& args);

#endif
