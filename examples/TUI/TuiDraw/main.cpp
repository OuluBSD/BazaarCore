#include <CtrlLib/CtrlLib.h>

#ifdef PLATFORM_TUI
#include <CtrlCore/TuiRaster.h>
#include <CtrlCore/TuiScreen.h>
#endif

using namespace Upp;

static Image CreateGradientImage()
{
	ImageBuffer ib(64, 32);
	for(int y = 0; y < 32; y++) {
		for(int x = 0; x < 64; x++) {
			int r = (x * 255) / 64;
			int g = (y * 255) / 32;
			int b = 255 - r;
			ib[y][x] = Color(r, g, b);
		}
	}
	return ib;
}

struct TuiCanvas : public Ctrl {
	Image gradImg;

	TuiCanvas()
	{
		gradImg = CreateGradientImage();
	}

	void Paint(Draw& w) override
	{
		Size sz = GetSize();
		w.DrawRect(sz, SColorPaper());

		// 1. Title and labels
		Font headerFont = StdFont(16).Bold();
		w.DrawText(16, 12, "Graphic Primitives & Sub-Cell Rasterisation", headerFont, SColorText());

		// 2. Rectangles & boxes
		w.DrawText(16, 36, "Rectangles & Fill:", StdFont().Bold(), SColorText());
		w.DrawRect(16, 52, 90, 48, LtBlue());
		w.DrawRect(26, 60, 70, 32, Blue());

		w.DrawRect(120, 52, 90, 48, SColorFace());
		DrawFrame(w, 120, 52, 90, 48, SColorHighlight());

		// 3. Lines & diagonal strokes
		w.DrawText(230, 36, "Lines & Diagonals:", StdFont().Bold(), SColorText());
		w.DrawLine(230, 52, 330, 52, 1, Red());
		w.DrawLine(230, 64, 330, 64, 2, Green());
		w.DrawLine(230, 78, 330, 78, 3, Yellow());
		w.DrawLine(230, 52, 330, 100, 1, LtCyan());
		w.DrawLine(230, 100, 330, 52, 1, LtMagenta());

		// 4. Ellipses & Circles
		w.DrawText(360, 36, "Ellipses & Arcs:", StdFont().Bold(), SColorText());
		w.DrawEllipse(360, 52, 70, 48, LtGreen(), 1, Green());
		w.DrawEllipse(445, 52, 50, 48, LtYellow(), 2, Brown());

		// 5. Image & Gradient rasterisation
		w.DrawText(16, 116, "2D Image Gradient:", StdFont().Bold(), SColorText());
		w.DrawImage(16, 134, 160, 64, gradImg);

		// 6. Styled & Oversized text
		w.DrawText(200, 116, "Oversized & Styled Fonts:", StdFont().Bold(), SColorText());
		Font bigFont = Arial(24).Bold();
		w.DrawText(200, 134, "Terminal UI", bigFont, SColorHighlight());

		Font monoFont = Monospace(14).Italic();
		w.DrawText(200, 172, "Fast Monospace & Sub-cell Blocks", monoFont, SColorText());

		// 7. Grid pattern at bottom
		w.DrawText(16, 210, "Coordinate Grid Pattern:", StdFont().Bold(), SColorText());
		for(int i = 0; i <= 200; i += 20) {
			w.DrawLine(16 + i, 228, 16 + i, 268, 1, GrayColor(160));
		}
		for(int j = 0; j <= 40; j += 10) {
			w.DrawLine(16, 228 + j, 216, 228 + j, 1, GrayColor(160));
		}
	}
};

class TuiDrawWindow : public TopWindow {
	MenuBar   menu;
	StatusBar status;
	TuiCanvas canvas;
	ParentCtrl topBar;
	Label     lblMode;
	Switch    swBlockMode;
	Button    btnTheme;

public:
	void RefreshAll()
	{
		ChInvalidate();
		RefreshLayoutDeep();
		canvas.Refresh();
		Refresh();
	}

	void SetBlock(int mode, const char *name)
	{
#ifdef PLATFORM_TUI
		SetTuiBlockMode(mode);
		swBlockMode.SetData(mode);
		RefreshAll();
		status.Set(Format("Block mode set to: %s", name));
#else
		status.Set("Block modes active in TUI mode only.");
#endif
	}

	void MainMenu(Bar& bar)
	{
		bar.Sub("File", [=](Bar& b) {
			b.Add("Exit\tCtrl+Q", [=] { Break(); }).Key(K_CTRL_Q);
		});
#ifdef PLATFORM_TUI
		bar.Sub("Resolution Ladder", [=](Bar& b) {
			b.Add("L0: 1x1 Flat Cells", [=] { SetBlock(TUI_BLOCKS_L0, "L0 (1x1)"); });
			b.Add("L1: 2x1 Half Blocks", [=] { SetBlock(TUI_BLOCKS_L1, "L1 (2x1)"); });
			b.Add("L2: 2x2 Quadrants", [=] { SetBlock(TUI_BLOCKS_L2, "L2 (2x2)"); });
			b.Add("L3: 2x3 Sextants", [=] { SetBlock(TUI_BLOCKS_L3, "L3 (2x3)"); });
			b.Add("L4: 2x4 Octants", [=] { SetBlock(TUI_BLOCKS_L4, "L4 (2x4)"); });
			b.Add("Braille: 2x4 Monochrome", [=] { SetBlock(TUI_BLOCKS_BRAILLE, "Braille"); });
		});
		bar.Sub("Color Policy", [=](Bar& b) {
			b.Add("TrueColor (RGB)", [=] {
				TheTuiScreen().SetColorPolicy(TuiScreen::COLOR_RGB);
				RefreshAll();
				status.Set("Color policy: TrueColor RGB");
			});
			b.Add("ANSI 16 Palette", [=] {
				TheTuiScreen().SetColorPolicy(TuiScreen::COLOR_ANSI16);
				RefreshAll();
				status.Set("Color policy: ANSI 16 Palette");
			});
		});
#endif
		bar.Sub("Theme", [=](Bar& b) {
#ifdef PLATFORM_TUI
			b.Add("TUI Dark", [=] { ChTuiDarkSkin(); RefreshAll(); status.Set("Theme: TUI Dark"); });
			b.Add("TUI Light", [=] { ChTuiLightSkin(); RefreshAll(); status.Set("Theme: TUI Light"); });
			b.Add("TUI ANSI 16", [=] { ChTuiAnsi16Skin(); RefreshAll(); status.Set("Theme: TUI ANSI 16"); });
#else
			b.Add("Dark", [=] { ChDarkSkin(); RefreshAll(); status.Set("Theme: Dark"); });
			b.Add("Standard", [=] { ChStdSkin(); RefreshAll(); status.Set("Theme: Standard"); });
#endif
		});
		bar.Sub("Help", [=](Bar& b) {
			b.Add("About...", [=] {
				PromptOK("TUI Draw Showcase\nSub-cell graphic primitive rasterisation\nUltimate++ Terminal Backend");
			});
		});
	}

	TuiDrawWindow()
	{
		Title("TUI Draw - Graphics & Sub-Cell Rasterisation");
		SetRect(0, 0, 720, 440);
		Sizeable().Zoomable();

		AddFrame(menu);
		AddFrame(status);

		menu.Set([=](Bar& b) { MainMenu(b); });
		status.Set("Ready. Switch resolution modes to observe sub-cell rasterisation.");

		lblMode.SetText("Block Mode:");
#ifdef PLATFORM_TUI
		swBlockMode.Add(TUI_BLOCKS_L0, "L0 (1x1)");
		swBlockMode.Add(TUI_BLOCKS_L1, "L1 (2x1)");
		swBlockMode.Add(TUI_BLOCKS_L2, "L2 (2x2)");
		swBlockMode.Add(TUI_BLOCKS_L3, "L3 (2x3)");
		swBlockMode.Add(TUI_BLOCKS_L4, "L4 (2x4)");
		swBlockMode.Add(TUI_BLOCKS_BRAILLE, "Braille");
		swBlockMode.SetData(GetTuiBlockMode());
		swBlockMode << [=] {
			int m = ~swBlockMode;
			SetTuiBlockMode(m);
			RefreshAll();
			status.Set(Format("Block Mode changed to %d", m));
		};
#endif

		btnTheme.SetLabel("Toggle Theme");
		btnTheme << [=] {
#ifdef PLATFORM_TUI
			if(IsDark(SColorPaper()))
				ChTuiLightSkin();
			else
				ChTuiDarkSkin();
#else
			if(IsDark(SColorPaper()))
				ChStdSkin();
			else
				ChDarkSkin();
#endif
			RefreshAll();
			status.Set("Theme toggled.");
		};

		topBar.Add(lblMode.LeftPos(10, 80).TopPos(4, 20));
		topBar.Add(swBlockMode.HSizePos(90, 140).TopPos(4, 22));
		topBar.Add(btnTheme.RightPos(10, 120).TopPos(4, 22));

		Add(topBar.HSizePos(0, 0).TopPos(0, 30));
		Add(canvas.HSizePos(0, 0).VSizePos(30, 0));
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
	TuiDrawWindow win;
	win.Run();
}
