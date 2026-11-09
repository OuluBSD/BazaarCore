#include <CtrlLib/CtrlLib.h>

using namespace Upp;

class TuiHelloWindow : public TopWindow {
	MenuBar   menu;
	StatusBar status;
	Label     label;
	Button    button;
	int       clicks = 0;

public:
	TuiHelloWindow()
	{
		Title("TUI Hello World");
		SetRect(0, 0, 480, 240);
		Sizeable().Zoomable();

		AddFrame(menu);
		AddFrame(status);

		menu.Set([=](Bar& bar) {
			bar.Sub("File", [=](Bar& b) {
				b.Add("About...", [=] {
					PromptOK("TUI Hello World\nUltimate++ Terminal Backend Smoke Test");
				});
				b.Separator();
				b.Add("Exit\tCtrl+Q", [=] { Break(); }).Key(K_CTRL_Q);
			});
		});

		label.SetText("Hello, Terminal UI!");
		label.SetAlign(ALIGN_CENTER);

		button.SetLabel("Click Me!");
		button << [=] {
			clicks++;
			label.SetText(Format("Button clicked %d time(s)!", clicks));
			status.Set(Format("Total clicks: %d", clicks));
		};

		Add(label.HSizePos(20, 20).TopPos(50, 24));
		Add(button.HCenterPos(140).TopPos(90, 28));

		status.Set("Ready. Press Tab to focus, Space/Enter to activate, Ctrl+Q to exit.");
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
	TuiHelloWindow win;
	win.Run();
}
