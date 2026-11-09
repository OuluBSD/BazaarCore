#include <CtrlLib/CtrlLib.h>

using namespace Upp;

class TuiEditorWindow : public TopWindow {
	MenuBar   menu;
	ToolBar   toolbar;
	StatusBar status;
	DocEdit   editor;
	FileSel   filesel;
	String    filename;

public:
	void UpdateStatus()
	{
		int line = editor.GetCursorLine();
		int64 pos = editor.GetCursor64();
		int col = (int)(pos - editor.GetPos64(line));
		String name = filename.IsEmpty() ? "Untitled" : GetFileName(filename);
		status.Set(Format("Line: %d, Col: %d  |  Chars: %d  |  %s%s",
		                  line + 1, col + 1, editor.GetLength(), name,
		                  editor.IsModified() ? " *" : ""));
	}

	void NewDoc()
	{
		if(editor.IsModified() && !PromptYesNo("Discard unsaved changes?"))
			return;
		editor.Clear();
		editor.ClearModify();
		filename.Clear();
		Title("TUI Editor - [Untitled]");
		UpdateStatus();
	}

	void OpenDoc()
	{
		if(filesel.ExecuteOpen("Open Text File")) {
			String fn = filesel.Get();
			String content = LoadFile(fn);
			editor.Set(content);
			editor.ClearModify();
			filename = fn;
			Title(Format("TUI Editor - [%s]", GetFileName(fn)));
			UpdateStatus();
		}
	}

	void SaveDoc()
	{
		if(filename.IsEmpty()) {
			SaveAsDoc();
			return;
		}
		if(SaveFile(filename, editor.Get())) {
			editor.ClearModify();
			UpdateStatus();
		}
		else
			PromptOK("Failed to save file: " + filename);
	}

	void SaveAsDoc()
	{
		if(filesel.ExecuteSaveAs("Save Text File As")) {
			filename = filesel.Get();
			SaveDoc();
			Title(Format("TUI Editor - [%s]", GetFileName(filename)));
		}
	}

	void MainMenu(Bar& bar)
	{
		bar.Sub("File", [=](Bar& b) {
			b.Add("New\tCtrl+N", CtrlImg::new_doc(), [=] { NewDoc(); }).Key(K_CTRL_N);
			b.Add("Open...\tCtrl+O", CtrlImg::open(), [=] { OpenDoc(); }).Key(K_CTRL_O);
			b.Add("Save\tCtrl+S", CtrlImg::save(), [=] { SaveDoc(); }).Key(K_CTRL_S);
			b.Add("Save As...", CtrlImg::save_as(), [=] { SaveAsDoc(); });
			b.Separator();
			b.Add("Exit\tCtrl+Q", [=] { Break(); }).Key(K_CTRL_Q);
		});
		bar.Sub("Edit", [=](Bar& b) {
			b.Add("Undo\tCtrl+Z", CtrlImg::undo(), [=] { editor.Undo(); UpdateStatus(); }).Key(K_CTRL_Z);
			b.Add("Redo\tCtrl+Y", CtrlImg::redo(), [=] { editor.Redo(); UpdateStatus(); }).Key(K_CTRL_Y);
			b.Separator();
			b.Add("Cut\tCtrl+X", CtrlImg::cut(), [=] { editor.Cut(); UpdateStatus(); }).Key(K_CTRL_X);
			b.Add("Copy\tCtrl+C", CtrlImg::copy(), [=] { editor.Copy(); }).Key(K_CTRL_C);
			b.Add("Paste\tCtrl+V", CtrlImg::paste(), [=] { editor.Paste(); UpdateStatus(); }).Key(K_CTRL_V);
			b.Separator();
			b.Add("Select All\tCtrl+A", [=] { editor.SelectAll(); }).Key(K_CTRL_A);
		});
		bar.Sub("Theme", [=](Bar& b) {
#ifdef PLATFORM_TUI
			b.Add("TUI Dark", [=] {
				ChTuiDarkSkin();
				ChInvalidate();
				RefreshLayoutDeep();
				Refresh();
			});
			b.Add("TUI Light", [=] {
				ChTuiLightSkin();
				ChInvalidate();
				RefreshLayoutDeep();
				Refresh();
			});
			b.Add("TUI ANSI 16", [=] {
				ChTuiAnsi16Skin();
				ChInvalidate();
				RefreshLayoutDeep();
				Refresh();
			});
#else
			b.Add("Dark", [=] { ChDarkSkin(); ChInvalidate(); RefreshLayoutDeep(); Refresh(); });
			b.Add("Standard", [=] { ChStdSkin(); ChInvalidate(); RefreshLayoutDeep(); Refresh(); });
#endif
		});
		bar.Sub("Help", [=](Bar& b) {
			b.Add("About...", [=] {
				PromptOK("TUI Text Editor\nMonospace text editor on cell grid\nUltimate++ Terminal Backend");
			});
		});
	}

	void MainTool(Bar& bar)
	{
		bar.Add("New", CtrlImg::new_doc(), [=] { NewDoc(); });
		bar.Add("Open", CtrlImg::open(), [=] { OpenDoc(); });
		bar.Add("Save", CtrlImg::save(), [=] { SaveDoc(); });
		bar.Separator();
		bar.Add("Undo", CtrlImg::undo(), [=] { editor.Undo(); UpdateStatus(); });
		bar.Add("Redo", CtrlImg::redo(), [=] { editor.Redo(); UpdateStatus(); });
		bar.Separator();
		bar.Add("Cut", CtrlImg::cut(), [=] { editor.Cut(); UpdateStatus(); });
		bar.Add("Copy", CtrlImg::copy(), [=] { editor.Copy(); });
		bar.Add("Paste", CtrlImg::paste(), [=] { editor.Paste(); UpdateStatus(); });
	}

	TuiEditorWindow()
	{
		Title("TUI Editor - [Untitled]");
		SetRect(0, 0, 720, 480);
		Sizeable().Zoomable();

		AddFrame(menu);
		AddFrame(toolbar);
		AddFrame(status);

		menu.Set([=](Bar& b) { MainMenu(b); });
		toolbar.Set([=](Bar& b) { MainTool(b); });

		filesel.Type("Text Files (*.txt)", "*.txt");
		filesel.Type("Source Files (*.cpp, *.h)", "*.cpp;*.h");
		filesel.AllFilesType();

		editor.SetFont(Monospace());
		editor.WhenAction = [=] { UpdateStatus(); };
		editor.WhenSel = [=] { UpdateStatus(); };

		editor.Set(String(
			"Welcome to TUI Editor!\n"
			"\n"
			"This is an ordinary U++ DocEdit running inside a terminal grid.\n"
			"Features demonstrated:\n"
			"- Monospace cell grid alignment\n"
			"- Terminal cursor as text caret\n"
			"- Bracketed paste and OSC 52 clipboard\n"
			"- Full keyboard navigation and mouse selection\n"
			"- MenuBar, ToolBar, and dynamic StatusBar\n"
		));
		editor.ClearModify();

		Add(editor.SizePos());
		UpdateStatus();
	}

	bool Key(dword key, int count) override
	{
		if(key == K_ESCAPE) {
			Break();
			return true;
		}
		return TopWindow::Key(key, count);
	}
};

GUI_APP_MAIN
{
	TuiEditorWindow win;
	win.Run();
}
