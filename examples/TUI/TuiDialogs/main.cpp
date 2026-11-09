#include <CtrlLib/CtrlLib.h>

using namespace Upp;

#define LAYOUTFILE <TuiDialogs/TuiDialogs.lay>
#include <CtrlCore/lay.h>


class TuiDialogsWindow : public TopWindow {
	MenuBar   menu;
	StatusBar status;

	Button btnLayDialog;
	Button btnNestedModals;
	Button btnPrompts;
	Button btnResizable;
	Button btnOversized;

public:
	void OpenLayDialog()
	{
		WithSampleDialogLayout<TopWindow> dlg;
		CtrlLayoutOKCancel(dlg, "Sample Layout Dialog (.lay)");
		dlg.name.SetData("Linus Torvalds");
		dlg.age.MinMax(1, 120);
		dlg.age.SetData(54);
		dlg.optSubscribe.Set(1);

		if(dlg.Execute() == IDOK) {
			status.Set(Format("Form submitted: Name='%s', Age=%d, Subscribed=%d",
			                  ~dlg.name, (int)~dlg.age, (int)~dlg.optSubscribe));
		}
		else {
			status.Set("Form cancelled.");
		}
	}

	void OpenNestedModals()
	{
		TopWindow parentDlg;
		parentDlg.Title("Modal Level 1");
		parentDlg.SetRect(0, 0, 440, 260);

		Label lbl;
		lbl.SetText("Level 1 Modal Dialog\nClick below to open Level 2.");
		lbl.SetAlign(ALIGN_CENTER);

		Button btnOpenL2;
		btnOpenL2.SetLabel("Open Level 2 Modal");
		btnOpenL2 << [=, &parentDlg] {
			TopWindow childDlg;
			childDlg.Title("Modal Level 2");
			childDlg.SetRect(0, 0, 360, 200);

			Label childLbl;
			childLbl.SetText("Level 2 Modal Dialog\nClick below to trigger Prompt.");
			childLbl.SetAlign(ALIGN_CENTER);

			Button btnPrompt;
			btnPrompt.SetLabel("Ask PromptYesNo");
			btnPrompt << [=, &childDlg] {
				if(PromptYesNo("Confirm action from Level 2 modal?")) {
					PromptOK("Action confirmed in Level 2!");
					childDlg.Break(IDOK);
				}
			};

			Button btnClose;
			btnClose.Cancel().SetLabel("Close Level 2");
			btnClose << [=, &childDlg] { childDlg.Break(IDCANCEL); };

			childDlg.Add(childLbl.HSizePos(10, 10).TopPos(20, 40));
			childDlg.Add(btnPrompt.HCenterPos(180).TopPos(80, 26));
			childDlg.Add(btnClose.HCenterPos(140).TopPos(120, 24));

			int res = childDlg.Execute();
			if(res == IDOK) {
				status.Set("Nested modal completed successfully.");
				parentDlg.Break(IDOK);
			}
		};

		Button btnCloseParent;
		btnCloseParent.Cancel().SetLabel("Close");
		btnCloseParent << [=, &parentDlg] { parentDlg.Break(IDCANCEL); };

		parentDlg.Add(lbl.HSizePos(10, 10).TopPos(30, 40));
		parentDlg.Add(btnOpenL2.HCenterPos(200).TopPos(90, 28));
		parentDlg.Add(btnCloseParent.HCenterPos(120).TopPos(140, 24));

		parentDlg.Execute();
	}

	void OpenPrompts()
	{
		if(PromptYesNo("Do you want to test standard U++ Prompt dialogs?")) {
			if(PromptOKCancel("PromptOKCancel: Proceed with information display?")) {
				PromptOK("PromptOK: All dialog mechanisms functioning nominally!");
				status.Set("Prompts tested successfully.");
			}
			else
				status.Set("PromptOKCancel cancelled.");
		}
		else
			status.Set("PromptYesNo rejected.");
	}

	void OpenResizable()
	{
		TopWindow dlg;
		dlg.Title("Resizable Modal Dialog");
		dlg.SetRect(0, 0, 500, 320);
		dlg.Sizeable().Zoomable();

		DocEdit edit;
		edit.SetFont(Monospace());
		edit.Set(String("This dialog is fully resizable and zoomable.\n"
		                "Resize the window to observe layout reflow.\n"
		                "Press Esc or Click OK when done."));

		Button btnOk;
		btnOk.Ok().SetLabel("OK");
		btnOk << [=, &dlg] { dlg.Break(IDOK); };

		dlg.Add(edit.HSizePos(10, 10).VSizePos(10, 48));
		dlg.Add(btnOk.RightPos(10, 80).BottomPos(10, 26));

		dlg.Execute();
		status.Set("Resizable modal closed.");
	}

	void OpenOversized()
	{
		TopWindow dlg;
		dlg.Title("Oversized Boundary Dialog (1600x1200)");
		dlg.SetRect(0, 0, 1600, 1200);

		Label lbl;
		lbl.SetText("This dialog requested 1600x1200 pixels.\n"
		            "In TUI mode, it is automatically clamped to screen bounds.\n"
		            "Press Enter or Esc to dismiss.");
		lbl.SetAlign(ALIGN_CENTER);

		Button btnOk;
		btnOk.Ok().SetLabel("Dismiss");
		btnOk << [=, &dlg] { dlg.Break(IDOK); };

		dlg.Add(lbl.HSizePos(10, 10).TopPos(20, 60));
		dlg.Add(btnOk.HCenterPos(120).TopPos(100, 26));

		dlg.Execute();
		status.Set("Oversized dialog clamped and dismissed.");
	}

	void MainMenu(Bar& bar)
	{
		bar.Sub("File", [=](Bar& b) {
			b.Add("Exit\tCtrl+Q", [=] { Break(); }).Key(K_CTRL_Q);
		});
		bar.Sub("Dialogs", [=](Bar& b) {
			b.Add("Open .lay Dialog\tF2", [=] { OpenLayDialog(); }).Key(K_F2);
			b.Add("Nested Modals\tF3", [=] { OpenNestedModals(); }).Key(K_F3);
			b.Add("Prompts Test\tF4", [=] { OpenPrompts(); }).Key(K_F4);
			b.Add("Resizable Dialog\tF5", [=] { OpenResizable(); }).Key(K_F5);
			b.Add("Oversized Dialog\tF6", [=] { OpenOversized(); }).Key(K_F6);
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
				PromptOK("TUI Dialogs Demo\nModal loops, .lay layouts, and screen clamping\nUltimate++ Terminal Backend");
			});
		});
	}

	TuiDialogsWindow()
	{
		Title("TUI Dialogs Showcase");
		SetRect(0, 0, 600, 360);
		Sizeable().Zoomable();

		AddFrame(menu);
		AddFrame(status);

		menu.Set([=](Bar& b) { MainMenu(b); });
		status.Set("Ready. Click any button or use F2-F6 to test dialogs.");

		btnLayDialog.SetLabel("1. Open .lay Form Dialog (F2)");
		btnLayDialog << [=] { OpenLayDialog(); };

		btnNestedModals.SetLabel("2. Open Nested Modals (F3)");
		btnNestedModals << [=] { OpenNestedModals(); };

		btnPrompts.SetLabel("3. Standard Prompts (F4)");
		btnPrompts << [=] { OpenPrompts(); };

		btnResizable.SetLabel("4. Resizable Modal Dialog (F5)");
		btnResizable << [=] { OpenResizable(); };

		btnOversized.SetLabel("5. Oversized Boundary Dialog (F6)");
		btnOversized << [=] { OpenOversized(); };

		Add(btnLayDialog.HCenterPos(280).TopPos(40, 30));
		Add(btnNestedModals.HCenterPos(280).TopPos(80, 30));
		Add(btnPrompts.HCenterPos(280).TopPos(120, 30));
		Add(btnResizable.HCenterPos(280).TopPos(160, 30));
		Add(btnOversized.HCenterPos(280).TopPos(200, 30));
	}

	bool Key(dword key, int count) override
	{
		if(key == K_ESCAPE) {
			Break();
			return true;
		}
		if(key == K_F2) {
			OpenLayDialog();
			return true;
		}
		if(key == K_F3) {
			OpenNestedModals();
			return true;
		}
		if(key == K_F4) {
			OpenPrompts();
			return true;
		}
		if(key == K_F5) {
			OpenResizable();
			return true;
		}
		if(key == K_F6) {
			OpenOversized();
			return true;
		}
		return TopWindow::Key(key, count);
	}
};

GUI_APP_MAIN
{
	TuiDialogsWindow win;
	win.Run();
}
