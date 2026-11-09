#include <CtrlLib/CtrlLib.h>

using namespace Upp;

struct FileEntry : Moveable<FileEntry> {
	String name;
	int64  length = 0;
	Time   time;
	bool   isFolder = false;
};

struct FilePane : public ParentCtrl {
	Label     pathLbl;
	ArrayCtrl list;
	String    currentDir;
	bool      active = false;
	Event<>   WhenChangeActive;
	Event<String> WhenOpenFile;

	void SetActive(bool b)
	{
		active = b;
		pathLbl.SetInk(active ? SColorHighlight() : SColorText());
		if(active)
			list.SetFocus();
	}

	void LoadDir(const String& dir)
	{
		currentDir = NormalizePath(dir);
		pathLbl.SetText(currentDir);
		list.Clear();

		// Add ".." if not root
		String parent = GetFileFolder(currentDir);
		if(!parent.IsEmpty() && parent != currentDir)
			list.Add(CtrlImg::Dir(), "..", "<DIR>", "");

		Vector<FileEntry> dirs;
		Vector<FileEntry> files;

		for(FindFile ff(AppendFileName(currentDir, "*.*")); ff; ff.Next()) {
			String name = ff.GetName();
			if(name == "." || name == "..")
				continue;
			FileEntry fe;
			fe.name = name;
			fe.length = ff.GetLength();
			fe.time = ff.GetLastWriteTime();
			fe.isFolder = ff.IsFolder();
			if(fe.isFolder)
				dirs.Add(fe);
			else
				files.Add(fe);
		}

		Sort(dirs, [](const FileEntry& a, const FileEntry& b) { return a.name < b.name; });
		Sort(files, [](const FileEntry& a, const FileEntry& b) { return a.name < b.name; });

		for(const auto& d : dirs) {
			list.Add(CtrlImg::Dir(), d.name, "<DIR>", Format(d.time));
		}
		for(const auto& f : files) {
			int64 sz = f.length;
			String szStr;
			if(sz >= 1024 * 1024)
				szStr = Format("%d MB", sz / (1024 * 1024));
			else if(sz >= 1024)
				szStr = Format("%d KB", sz / 1024);
			else
				szStr = Format("%d B", sz);
			list.Add(CtrlImg::File(), f.name, szStr, Format(f.time));
		}

		if(list.GetCount() > 0)
			list.SetCursor(0);
	}

	String GetSelectedPath() const
	{
		int c = list.GetCursor();
		if(c < 0 || c >= list.GetCount())
			return String();
		String name = list.Get(c, 1);
		if(name == "..")
			return GetFileFolder(currentDir);
		return AppendFileName(currentDir, name);
	}

	bool IsSelectedDir() const
	{
		int c = list.GetCursor();
		if(c < 0 || c >= list.GetCount())
			return false;
		String sz = list.Get(c, 2);
		return sz == "<DIR>";
	}

	void ActivateSelected()
	{
		int c = list.GetCursor();
		if(c < 0 || c >= list.GetCount())
			return;
		String name = list.Get(c, 1);
		if(name == "..") {
			LoadDir(GetFileFolder(currentDir));
			return;
		}
		String target = AppendFileName(currentDir, name);
		if(DirectoryExists(target)) {
			LoadDir(target);
		}
		else if(FileExists(target)) {
			WhenOpenFile(target);
		}
	}

	FilePane()
	{
		pathLbl.SetFont(StdFont().Bold());
		Add(pathLbl.HSizePos(2, 2).TopPos(2, 20));

		list.AddColumn("Name", 20);
		list.AddColumn("Size", 8);
		list.AddColumn("Date/Time", 14);
		list.WhenLeftDouble = [=] { ActivateSelected(); };
		list.WhenEnterRow = [=] { WhenChangeActive(); };
		list.WhenLeftClick = [=] { WhenChangeActive(); };

		Add(list.HSizePos(0, 0).VSizePos(24, 0));
	}
};

class TuiCommanderWindow : public TopWindow {
	MenuBar   menu;
	ToolBar   toolbar;
	StatusBar status;
	Splitter  splitter;

	FilePane  leftPane;
	FilePane  rightPane;
	int       activePaneIdx = 0; // 0 = left, 1 = right

public:
	FilePane& ActivePane()   { return activePaneIdx == 0 ? leftPane : rightPane; }
	FilePane& InactivePane() { return activePaneIdx == 0 ? rightPane : leftPane; }

	void SetActive(int idx)
	{
		activePaneIdx = idx;
		leftPane.SetActive(idx == 0);
		rightPane.SetActive(idx == 1);
		UpdateStatus();
	}

	void UpdateStatus()
	{
		FilePane& pane = ActivePane();
		String sel = pane.GetSelectedPath();
		int total = pane.list.GetCount();
		if(!sel.IsEmpty())
			status.Set(Format("[%s]  Items: %d  |  Selected: %s",
			                  GetFileName(pane.currentDir), total, GetFileName(sel)));
		else
			status.Set(Format("[%s]  Items: %d", GetFileName(pane.currentDir), total));
	}

	void ViewFile(const String& path)
	{
		String p = path.IsEmpty() ? ActivePane().GetSelectedPath() : path;
		if(p.IsEmpty() || DirectoryExists(p))
			return;

		TopWindow viewer;
		viewer.Title("File View: " + GetFileName(p));
		viewer.SetRect(0, 0, 640, 400);
		DocEdit edit;
		edit.SetFont(Monospace());
		edit.Set(LoadFile(p));
		viewer.Add(edit.SizePos());
		viewer.Execute();
	}

	void CopySelected()
	{
		String src = ActivePane().GetSelectedPath();
		if(src.IsEmpty() || DirectoryExists(src)) {
			PromptOK("Please select a file to copy.");
			return;
		}
		String dst = AppendFileName(InactivePane().currentDir, GetFileName(src));
		if(FileExists(dst) && !PromptYesNo(Format("Overwrite existing file '%s'?", GetFileName(dst))))
			return;

		if(FileCopy(src, dst)) {
			InactivePane().LoadDir(InactivePane().currentDir);
			status.Set(Format("Copied '%s' -> '%s'", GetFileName(src), GetFileName(dst)));
		}
		else
			PromptOK("Failed to copy file.");
	}

	void Mkdir()
	{
		String folderName;
		if(!EditText(folderName, "New Folder", "Folder name:"))
			return;
		folderName = TrimBoth(folderName);
		if(folderName.IsEmpty())
			return;

		String newPath = AppendFileName(ActivePane().currentDir, folderName);
		if(RealizeDirectory(newPath)) {
			ActivePane().LoadDir(ActivePane().currentDir);
			status.Set("Created folder: " + folderName);
		}
		else
			PromptOK("Failed to create folder: " + folderName);
	}

	void DeleteSelected()
	{
		String path = ActivePane().GetSelectedPath();
		if(path.IsEmpty() || path == ActivePane().currentDir)
			return;

		if(!PromptYesNo(Format("Are you sure you want to delete '%s'?", GetFileName(path))))
			return;

		bool ok = false;
		if(DirectoryExists(path))
			ok = DeleteFolderDeep(path);
		else
			ok = FileDelete(path);

		if(ok) {
			ActivePane().LoadDir(ActivePane().currentDir);
			status.Set("Deleted: " + GetFileName(path));
		}
		else
			PromptOK("Failed to delete: " + GetFileName(path));
	}

	void RefreshPanes()
	{
		leftPane.LoadDir(leftPane.currentDir);
		rightPane.LoadDir(rightPane.currentDir);
		UpdateStatus();
	}

	void MainMenu(Bar& bar)
	{
		bar.Sub("File", [=](Bar& b) {
			b.Add("View\tF3", CtrlImg::open(), [=] { ViewFile(String()); }).Key(K_F3);
			b.Add("Copy...\tF5", CtrlImg::copy(), [=] { CopySelected(); }).Key(K_F5);
			b.Add("New Folder...\tF7", CtrlImg::plus(), [=] { Mkdir(); }).Key(K_F7);
			b.Add("Delete...\tF8", CtrlImg::remove(), [=] { DeleteSelected(); }).Key(K_F8);
			b.Separator();
			b.Add("Refresh\tCtrl+R", [=] { RefreshPanes(); }).Key(K_CTRL_R);
			b.Separator();
			b.Add("Exit\tCtrl+Q", [=] { Break(); }).Key(K_CTRL_Q);
		});
		bar.Sub("Navigate", [=](Bar& b) {
			b.Add("Switch Pane\tTab", [=] { SetActive(1 - activePaneIdx); }).Key(K_TAB);
			b.Add("Parent Directory\tBackspace", [=] {
				ActivePane().LoadDir(GetFileFolder(ActivePane().currentDir));
			}).Key(K_BACKSPACE);
			b.Add("Root Directory", [=] { ActivePane().LoadDir("/"); });
			b.Add("Home Directory", [=] { ActivePane().LoadDir(GetHomeDirectory()); });
			b.Add("Current Working Dir", [=] { ActivePane().LoadDir(GetCurrentDirectory()); });
		});
		bar.Sub("Theme", [=](Bar& b) {
#ifdef PLATFORM_TUI
			b.Add("TUI Dark", [=] { ChTuiDarkSkin(); ChInvalidate(); RefreshLayoutDeep(); Refresh(); });
			b.Add("TUI Light", [=] { ChTuiLightSkin(); ChInvalidate(); RefreshLayoutDeep(); Refresh(); });
			b.Add("TUI ANSI 16", [=] { ChTuiAnsi16Skin(); ChInvalidate(); RefreshLayoutDeep(); Refresh(); });
#else
			b.Add("Dark", [=] { ChDarkSkin(); ChInvalidate(); RefreshLayoutDeep(); Refresh(); });
			b.Add("Standard", [=] { ChStdSkin(); ChInvalidate(); RefreshLayoutDeep(); Refresh(); });
#endif
		});
		bar.Sub("Help", [=](Bar& b) {
			b.Add("About...", [=] {
				PromptOK("TUI Commander\nTwo-pane terminal file manager\nUltimate++ Terminal Backend");
			});
		});
	}

	void MainTool(Bar& bar)
	{
		bar.Add("View (F3)", CtrlImg::open(), [=] { ViewFile(String()); });
		bar.Add("Copy (F5)", CtrlImg::copy(), [=] { CopySelected(); });
		bar.Add("Mkdir (F7)", CtrlImg::plus(), [=] { Mkdir(); });
		bar.Add("Delete (F8)", CtrlImg::remove(), [=] { DeleteSelected(); });
		bar.Separator();
		bar.Add("Refresh", [=] { RefreshPanes(); });
	}

	TuiCommanderWindow()
	{
		Title("TUI Commander");
		SetRect(0, 0, 760, 480);
		Sizeable().Zoomable();

		AddFrame(menu);
		AddFrame(toolbar);
		AddFrame(status);

		menu.Set([=](Bar& b) { MainMenu(b); });
		toolbar.Set([=](Bar& b) { MainTool(b); });

		leftPane.WhenChangeActive = [=] { SetActive(0); };
		rightPane.WhenChangeActive = [=] { SetActive(1); };
		leftPane.WhenOpenFile = [=](String p) { ViewFile(p); };
		rightPane.WhenOpenFile = [=](String p) { ViewFile(p); };

		splitter.Horz(leftPane, rightPane);
		splitter.SetPos(5000); // 50/50 split

		Add(splitter.SizePos());

		String workDir = GetCurrentDirectory();
		leftPane.LoadDir(workDir);
		rightPane.LoadDir(GetHomeDirectory());
		SetActive(0);
	}

	bool Key(dword key, int count) override
	{
		if(key == K_TAB) {
			SetActive(1 - activePaneIdx);
			return true;
		}
		if(key == K_ENTER) {
			ActivePane().ActivateSelected();
			return true;
		}
		if(key == K_BACKSPACE) {
			ActivePane().LoadDir(GetFileFolder(ActivePane().currentDir));
			return true;
		}
		if(key == K_F3) {
			ViewFile(String());
			return true;
		}
		if(key == K_F5) {
			CopySelected();
			return true;
		}
		if(key == K_F7) {
			Mkdir();
			return true;
		}
		if(key == K_F8) {
			DeleteSelected();
			return true;
		}
		if(key == K_ESCAPE) {
			Break();
			return true;
		}
		return TopWindow::Key(key, count);
	}
};

GUI_APP_MAIN
{
	TuiCommanderWindow win;
	win.Run();
}
