#include <CtrlLib/CtrlLib.h>

#ifdef PLATFORM_TUI
#include <CtrlCore/TuiScreen.h>
#include <CtrlCore/TuiRaster.h>
#include <CtrlCore/TuiIcon.h>
#endif

using namespace Upp;

struct TuiWidgetsWindow : public TopWindow {
	MenuBar   menu;
	ToolBar   toolbar;
	StatusBar status;
	Splitter  splitter;

	// Left pane: data grid
	ArrayCtrl grid;
	EditString editGridItem;
	EditInt    editGridVal;

	// Right pane: tabbed controls
	TabCtrl tabs;
	ParentCtrl inputPane;
	ParentCtrl buttonPane;
	ParentCtrl treePane;
	ParentCtrl themePane;

	// Input controls
	LabelBox    boxInputs;
	Label       lblEdit;
	EditField   edit;
	Label       lblDrop;
	DropList    drop;
	Label       lblChoice;
	DropChoice  choice;
	Label       lblSuggest;
	SuggestCtrl suggest;
	Label       lblDoc;
	DocEdit     doc;

	// Button & toggle controls
	LabelBox          boxButtons;
	Button            btnNormal;
	Button            btnOk;
	Option            opt1;
	Option            opt2;
	Switch            sw;
	MultiButton       multiBtn;
	Label             lblSlider;
	SliderCtrl        slider;
	Label             lblProg;
	ProgressIndicator prog;

	// Tree controls
	LabelBox boxTree;
	TreeCtrl tree;

	// Theme controls
	LabelBox boxTheme;
	Button   btnDark;
	Button   btnLight;
	Button   btnAnsi16;
	Button   btnBorderNone;
	Button   btnBorderGraphic;
	Option   optNerdFont;
	Switch   swBlockMode;

	void MainMenu(Bar& bar)
	{
		bar.Sub("File", [=](Bar& b) {
			b.Add("New\tCtrl+N", CtrlImg::new_doc(), [=] { status.Set("File: New"); }).Key(K_CTRL_N);
			b.Add("Open...\tCtrl+O", CtrlImg::open(), [=] { status.Set("File: Open"); }).Key(K_CTRL_O);
			b.Add("Save\tCtrl+S", CtrlImg::save(), [=] { status.Set("File: Save"); }).Key(K_CTRL_S);
			b.Separator();
			b.Add("Exit\tCtrl+Q", [=] { Break(); }).Key(K_CTRL_Q);
		});
		bar.Sub("Theme", [=](Bar& b) {
#ifdef PLATFORM_TUI
			b.Add("TUI Dark", [=] { SetSkinDark(); });
			b.Add("TUI Light", [=] { SetSkinLight(); });
			b.Add("TUI ANSI 16", [=] { SetSkinAnsi16(); });
#else
			b.Add("Dark", [=] { ChDarkSkin(); RefreshAll(); status.Set("Theme: Dark"); });
			b.Add("Standard", [=] { ChStdSkin(); RefreshAll(); status.Set("Theme: Standard"); });
#endif
		});
		bar.Sub("Borders", [=](Bar& b) {
			b.Add("None", [=] {
				ChBorderPolicyValue_Write(CHBORDER_NONE);
				RefreshAll();
				status.Set("Border policy: None");
			});
			b.Add("Graphic (box-drawing)", [=] {
				ChBorderPolicyValue_Write(CHBORDER_GRAPHIC);
				RefreshAll();
				status.Set("Border policy: Graphic");
			});
		});
#ifdef PLATFORM_TUI
		bar.Sub("Color Depth", [=](Bar& b) {
			b.Add("RGB (TrueColor)", [=] {
				TheTuiScreen().SetColorPolicy(TuiScreen::COLOR_RGB);
				RefreshAll();
				status.Set("Color depth: RGB TrueColor");
			});
			b.Add("ANSI 16 (terminal native palette)", [=] {
				TheTuiScreen().SetColorPolicy(TuiScreen::COLOR_ANSI16);
				RefreshAll();
				status.Set("Color depth: ANSI 16");
			});
		});
		bar.Sub("Block Level", [=](Bar& b) {
			b.Add("L0 (1x1 Flat Cells)", [=] { SetTuiBlockMode(TUI_BLOCKS_L0); RefreshAll(); status.Set("Block level: L0 (1x1)"); });
			b.Add("L1 (2x1 Half Blocks)", [=] { SetTuiBlockMode(TUI_BLOCKS_L1); RefreshAll(); status.Set("Block level: L1 (2x1)"); });
			b.Add("L2 (2x2 Quadrants)", [=] { SetTuiBlockMode(TUI_BLOCKS_L2); RefreshAll(); status.Set("Block level: L2 (2x2)"); });
			b.Add("L3 (2x3 Sextants)", [=] { SetTuiBlockMode(TUI_BLOCKS_L3); RefreshAll(); status.Set("Block level: L3 (2x3)"); });
			b.Add("L4 (2x4 Octants)", [=] { SetTuiBlockMode(TUI_BLOCKS_L4); RefreshAll(); status.Set("Block level: L4 (2x4)"); });
			b.Add("Braille (2x4 Monochrome)", [=] { SetTuiBlockMode(TUI_BLOCKS_BRAILLE); RefreshAll(); status.Set("Block level: Braille"); });
		});
#endif
		bar.Sub("Help", [=](Bar& b) {
			b.Add("About...", [=] {
				PromptOK("TUI Widgets Catalogue\nUltimate++ Terminal UI Component Showcase");
			});
		});
	}

	void MainTool(Bar& bar)
	{
		bar.Add("New", CtrlImg::new_doc(), [=] { status.Set("ToolBar: New"); });
		bar.Add("Open", CtrlImg::open(), [=] { status.Set("ToolBar: Open"); });
		bar.Add("Save", CtrlImg::save(), [=] { status.Set("ToolBar: Save"); });
		bar.Separator();
		bar.Add("Exit", CtrlImg::cross(), [=] { Break(); });
	}

	void SetSkinDark()
	{
#ifdef PLATFORM_TUI
		ChTuiDarkSkin();
		RefreshAll();
		status.Set("Theme: TUI Dark");
#endif
	}

	void SetSkinLight()
	{
#ifdef PLATFORM_TUI
		ChTuiLightSkin();
		RefreshAll();
		status.Set("Theme: TUI Light");
#endif
	}

	void SetSkinAnsi16()
	{
#ifdef PLATFORM_TUI
		ChTuiAnsi16Skin();
		RefreshAll();
		status.Set("Theme: TUI ANSI 16");
#endif
	}

	void RefreshAll()
	{
		ChInvalidate();
		RefreshLayoutDeep();
		Refresh();
	}

	TuiWidgetsWindow()
	{
		Title("TUI Widgets Catalogue");
		SetRect(0, 0, 720, 400);
		Sizeable().Zoomable();

		AddFrame(menu);
		AddFrame(toolbar);
		AddFrame(status);

		menu.Set([=](Bar& b) { MainMenu(b); });
		toolbar.Set([=](Bar& b) { MainTool(b); });
		status.Set("Ready - Tab: navigate, F10: menu, Ctrl+Q: exit");

		// Configure left pane: grid with editing and sorting
		grid.AddColumn("Item", 14).Edit(editGridItem).Sorting();
		grid.AddColumn("Value", 8).Edit(editGridVal).Sorting();
		grid.AddColumn("Status", 10).Sorting();
		grid.Add("Apple", 10, "Active");
		grid.Add("Banana", 25, "Pending");
		grid.Add("Cherry", 40, "Active");
		grid.Add("Date", 15, "Inactive");
		grid.Add("Elderberry", 80, "Active");
		grid.Add("Fig", 30, "Active");
		grid.Add("Grape", 55, "Pending");

		// Configure right pane: Tab 1 (Inputs & Text)
		boxInputs.SetText("Text Input Controls");
		lblEdit.SetText("EditField:");
		edit.SetData("Hello TUI");
		lblDrop.SetText("DropList:");
		drop.Add(1, "Option Alpha");
		drop.Add(2, "Option Beta");
		drop.Add(3, "Option Gamma");
		drop.SetData(1);
		lblChoice.SetText("DropChoice:");
		choice.Add("Red");
		choice.Add("Green");
		choice.Add("Blue");
		choice.SetData("Green");
		lblSuggest.SetText("SuggestCtrl:");
		suggest.AddList("Apple");
		suggest.AddList("Banana");
		suggest.AddList("Cherry");
		suggest.AddList("Date");
		suggest.SetData("Ap");
		lblDoc.SetText("DocEdit:");
		doc.SetData("Line 1: Monospace editor\nLine 2: Mono cell alignment\nLine 3: Multiple lines supported");

		inputPane.Add(boxInputs.SizePos());
		boxInputs.Add(lblEdit.LeftPos(10, 80).TopPos(20, 16));
		boxInputs.Add(edit.HSizePos(96, 10).TopPos(20, 16));
		boxInputs.Add(lblDrop.LeftPos(10, 80).TopPos(42, 16));
		boxInputs.Add(drop.HSizePos(96, 10).TopPos(42, 16));
		boxInputs.Add(lblChoice.LeftPos(10, 80).TopPos(64, 16));
		boxInputs.Add(choice.HSizePos(96, 10).TopPos(64, 16));
		boxInputs.Add(lblSuggest.LeftPos(10, 80).TopPos(86, 16));
		boxInputs.Add(suggest.HSizePos(96, 10).TopPos(86, 16));
		boxInputs.Add(lblDoc.LeftPos(10, 80).TopPos(108, 16));
		boxInputs.Add(doc.HSizePos(10, 10).VSizePos(128, 10));

		// Configure right pane: Tab 2 (Buttons & Sliders)
		boxButtons.SetText("Buttons, Options & Sliders");
		btnNormal.SetLabel("Normal Button");
		btnNormal << [=] {
			status.Set("Normal button clicked");
			prog++;
		};
		btnOk.Ok().SetLabel("OK Button");
		btnOk << [=] {
			status.Set("OK button clicked");
			prog += 5;
		};

		opt1.SetLabel("Feature Alpha Enabled");
		opt1.Set(1);
		opt2.SetLabel("Feature Beta Enabled");

		sw.Add(0, "Low");
		sw.Add(1, "Medium");
		sw.Add(2, "High");
		sw.SetData(1);

		multiBtn.AddButton().SetLabel("Action 1");
		multiBtn.AddButton().SetLabel("Action 2");
		multiBtn.AddButton().SetLabel("Action 3");

		lblSlider.SetText("Slider:");
		slider.MinMax(0, 100);
		slider <<= 40;
		slider << [=] {
			int val = ~slider;
			prog.Set(val);
			status.Set(Format("Slider: %d%%", val));
		};

		lblProg.SetText("Progress:");
		prog.SetTotal(100);
		prog.Set(40);

		buttonPane.Add(boxButtons.SizePos());
		boxButtons.Add(btnNormal.LeftPos(10, 130).TopPos(20, 20));
		boxButtons.Add(btnOk.LeftPos(150, 130).TopPos(20, 20));
		boxButtons.Add(opt1.LeftPos(10, 220).TopPos(46, 18));
		boxButtons.Add(opt2.LeftPos(10, 220).TopPos(68, 18));
		boxButtons.Add(sw.LeftPos(10, 260).TopPos(92, 18));
		boxButtons.Add(multiBtn.LeftPos(10, 200).TopPos(116, 20));
		boxButtons.Add(lblSlider.LeftPos(10, 60).TopPos(144, 18));
		boxButtons.Add(slider.HSizePos(76, 10).TopPos(144, 18));
		boxButtons.Add(lblProg.LeftPos(10, 60).TopPos(170, 18));
		boxButtons.Add(prog.HSizePos(76, 10).TopPos(170, 18));

		// Configure right pane: Tab 3 (TreeCtrl)
		boxTree.SetText("Hierarchical Tree");
		tree.SetRoot(CtrlImg::Dir(), "Root Project");
		int dir1 = tree.Add(0, CtrlImg::Dir(), "src");
		tree.Add(dir1, CtrlImg::File(), "main.cpp");
		tree.Add(dir1, CtrlImg::File(), "util.cpp");
		tree.Add(dir1, CtrlImg::File(), "util.h");
		int dir2 = tree.Add(0, CtrlImg::Dir(), "docs");
		tree.Add(dir2, CtrlImg::File(), "README.md");
		tree.Add(dir2, CtrlImg::File(), "MANUAL.txt");
		tree.Open(0);
		tree.Open(dir1);
		tree.WhenLeftDouble = [=] {
			int id = tree.GetCursor();
			if(id >= 0)
				status.Set(Format("Selected node: %s", ~tree.GetValue(id)));
		};

		treePane.Add(boxTree.SizePos());
		boxTree.Add(tree.HSizePos(10, 10).VSizePos(20, 10));

		// Configure right pane: Tab 4 (Theme & Display)
		boxTheme.SetText("Theme Axes & Display Settings");
		btnDark.SetLabel("TUI Dark Skin");
		btnDark << [=] { SetSkinDark(); };
		btnLight.SetLabel("TUI Light Skin");
		btnLight << [=] { SetSkinLight(); };
		btnAnsi16.SetLabel("ANSI 16 Skin");
		btnAnsi16 << [=] { SetSkinAnsi16(); };

		btnBorderNone.SetLabel("Border: None");
		btnBorderNone << [=] {
			ChBorderPolicyValue_Write(CHBORDER_NONE);
			RefreshAll();
			status.Set("Border policy: None");
		};
		btnBorderGraphic.SetLabel("Border: Graphic");
		btnBorderGraphic << [=] {
			ChBorderPolicyValue_Write(CHBORDER_GRAPHIC);
			RefreshAll();
			status.Set("Border policy: Graphic");
		};

#ifdef PLATFORM_TUI
		optNerdFont.SetLabel("Nerd Font Symbols");
		optNerdFont.Set(IsTuiNerdFontEnabled() ? 1 : 0);
		optNerdFont << [=] {
			SetTuiNerdFontMode(~optNerdFont ? 1 : 0);
			RefreshAll();
			status.Set(Format("Nerd Fonts: %s", ~optNerdFont ? "Enabled" : "Disabled"));
		};

		swBlockMode.Add(TUI_BLOCKS_L0, "L0 (1x1)");
		swBlockMode.Add(TUI_BLOCKS_L1, "L1 (2x1)");
		swBlockMode.Add(TUI_BLOCKS_L2, "L2 (2x2)");
		swBlockMode.Add(TUI_BLOCKS_L3, "L3 (2x3)");
		swBlockMode.Add(TUI_BLOCKS_L4, "L4 (2x4)");
		swBlockMode.Add(TUI_BLOCKS_BRAILLE, "Braille");
		swBlockMode.SetData(GetTuiBlockMode());
		swBlockMode << [=] {
			SetTuiBlockMode(~swBlockMode);
			RefreshAll();
			status.Set(Format("Block Mode: %d", (int)~swBlockMode));
		};
#endif

		themePane.Add(boxTheme.SizePos());
		boxTheme.Add(btnDark.LeftPos(10, 140).TopPos(20, 20));
		boxTheme.Add(btnLight.LeftPos(160, 140).TopPos(20, 20));
		boxTheme.Add(btnAnsi16.LeftPos(310, 140).TopPos(20, 20));
		boxTheme.Add(btnBorderNone.LeftPos(10, 140).TopPos(46, 20));
		boxTheme.Add(btnBorderGraphic.LeftPos(160, 140).TopPos(46, 20));
#ifdef PLATFORM_TUI
		boxTheme.Add(optNerdFont.LeftPos(10, 200).TopPos(72, 18));
		boxTheme.Add(swBlockMode.LeftPos(10, 400).TopPos(96, 22));
#endif

		// Add tabs
		tabs.Add(inputPane, "Inputs & Text");
		tabs.Add(buttonPane, "Buttons & Sliders");
		tabs.Add(treePane, "Tree View");
		tabs.Add(themePane, "Theme Settings");

		splitter.Horz(grid, tabs);
		splitter.SetPos(3200); // 32% grid, 68% tabs

		Add(splitter.SizePos());
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
	TuiWidgetsWindow win;
	win.Run();
}
