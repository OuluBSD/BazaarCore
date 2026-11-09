#include "TuiDesktop.h"

NAMESPACE_UPP

TuiVirtualGui::TuiVirtualGui()
{
	mouse_buttons = 0;
	mod_keys = 0;
	mouse_in = true;
	mouse_pos = Point(0, 0);
	parser_init = false;
	Size cs = TheTuiTerminal().GetCellSize();
	TheTuiScreen().SetSize(cs);
	Size cell = TuiCellSize();
	desktop_size = Size(cs.cx * cell.cx, cs.cy * cell.cy);
}

TuiVirtualGui::~TuiVirtualGui()
{
}

dword TuiVirtualGui::GetOptions()
{
	return GUI_SETCARET | GUI_SETMOUSECURSOR;
}

bool TuiVirtualGui::WantsOwnWindowFrame()
{
	return true;
}

Size TuiVirtualGui::GetSize()
{
	Size cs = TheTuiTerminal().GetCellSize();
	Size cell = TuiCellSize();
	return Size(cs.cx * cell.cx, cs.cy * cell.cy);
}

dword TuiVirtualGui::GetMouseButtons()
{
	return mouse_buttons;
}

dword TuiVirtualGui::GetModKeys()
{
	return mod_keys;
}

bool TuiVirtualGui::IsMouseIn()
{
	return mouse_in;
}

SystemDraw& TuiVirtualGui::BeginDraw()
{
	sys_draw.SetTarget(&tui_draw);
	return sys_draw;
}

void TuiVirtualGui::CommitDraw()
{
	TheTuiScreen().Flush(TheTuiTerminal());
}

void TuiVirtualGui::WaitEvent(int ms)
{
	TheTuiTerminal().WaitInput(ms);
}

void TuiVirtualGui::WakeUpGuiThread()
{
	TheTuiTerminal().WakeUp();
}

void TuiVirtualGui::Quit()
{
	TheTuiTerminal().Stop();
}

bool TuiVirtualGui::IsWaitingEvent()
{
	return TheTuiTerminal().WaitInput(0);
}

void TuiVirtualGui::InitParser()
{
	if(parser_init)
		return;
	parser_init = true;
	parser.WhenChr = [this](dword u) { OnChr(u); };
	parser.WhenCtl = [this](byte c) { OnCtl(c); };
	parser.WhenEsc = [this](const VTInStream::Sequence& seq) { OnEsc(seq); };
	parser.WhenCsi = [this](const VTInStream::Sequence& seq) { OnCsi(seq); };
}

bool TuiVirtualGui::ProcessEvent(bool *quit)
{
	InitParser();

	Size cs = TheTuiTerminal().GetCellSize();
	Size cell = TuiCellSize();
	Size new_sz(cs.cx * cell.cx, cs.cy * cell.cy);
	if(new_sz != desktop_size || TheTuiScreen().GetSize() != cs) {
		desktop_size = new_sz;
		TheTuiScreen().SetSize(cs);
		Ctrl::SetDesktopSize(new_sz);
		return true;
	}

	bool did = false;
	if(TheTuiTerminal().WaitInput(0)) {
		String in = TheTuiTerminal().ReadInput();
		if(in.GetCount()) {
			parser.Parse(in, true);
			did = true;
		}
	}
	return did;
}

void TuiVirtualGui::OnChr(dword u)
{
	if(u >= 0x20)
		Ctrl::DoKeyFB(u, 1);
}

void TuiVirtualGui::OnCtl(byte c)
{
	switch(c) {
	case 0x09: Ctrl::DoKeyFB(K_TAB, 1); break;
	case 0x0A:
	case 0x0D: Ctrl::DoKeyFB(K_ENTER, 1); break;
	case 0x08:
	case 0x7F: Ctrl::DoKeyFB(K_BACKSPACE, 1); break;
	case 0x1B: Ctrl::DoKeyFB(K_ESCAPE, 1); break;
	default:
		if(c >= 1 && c <= 26) {
			if(c == 3)
				Ctrl::DoKeyFB(K_CTRL | 'C', 1);
			else
				Ctrl::DoKeyFB(K_CTRL | ('A' + c - 1), 1);
		}
		break;
	}
}

void TuiVirtualGui::OnEsc(const VTInStream::Sequence& seq)
{
	if(seq.opcode == 0x09) {
		TaskBar::CycleWindows();
		return;
	}
	if(seq.opcode >= 0x20 && seq.opcode < 0x7F) {
		Ctrl::DoKeyFB(seq.opcode | K_ALT, 1);
	}
}

void TuiVirtualGui::OnCsi(const VTInStream::Sequence& seq)
{
	if(seq.mode == '<') {
		HandleMouseCsi(seq);
		return;
	}
	if(seq.mode == '_') {
		HandleWin32KeyCsi(seq);
		return;
	}

	int p1 = seq.parameters.GetCount() > 0 ? StrInt(seq.parameters[0]) : 0;
	int p2 = seq.parameters.GetCount() > 1 ? StrInt(seq.parameters[1]) : 1;
	dword mods = 0;
	int m = p2 - 1;
	if(m & 1) mods |= K_SHIFT;
	if(m & 2) mods |= K_ALT;
	if(m & 4) mods |= K_CTRL;

	dword key = 0;
	if(seq.opcode == 'A') key = K_UP;
	else if(seq.opcode == 'B') key = K_DOWN;
	else if(seq.opcode == 'C') key = K_RIGHT;
	else if(seq.opcode == 'D') key = K_LEFT;
	else if(seq.opcode == 'H') key = K_HOME;
	else if(seq.opcode == 'F') key = K_END;
	else if(seq.opcode == 'Z') key = K_SHIFT | K_TAB;
	else if(seq.opcode == '~') {
		switch(p1) {
		case 1:
		case 7: key = K_HOME; break;
		case 2: key = K_INSERT; break;
		case 3: key = K_DELETE; break;
		case 4:
		case 8: key = K_END; break;
		case 5: key = K_PAGEUP; break;
		case 6: key = K_PAGEDOWN; break;
		case 11: key = K_F1; break;
		case 12: key = K_F2; break;
		case 13: key = K_F3; break;
		case 14: key = K_F4; break;
		case 15: key = K_F5; break;
		case 17: key = K_F6; break;
		case 18: key = K_F7; break;
		case 19: key = K_F8; break;
		case 20: key = K_F9; break;
		case 21: key = K_F10; break;
		case 23: key = K_F11; break;
		case 24: key = K_F12; break;
		}
	}

	if(key) {
		if(key == K_TAB && (mods & K_ALT))
			TaskBar::CycleWindows();
		else
			Ctrl::DoKeyFB(key | mods, 1);
	}
}

void TuiVirtualGui::HandleWin32KeyCsi(const VTInStream::Sequence& seq)
{
	int vk = StrInt(seq.GetStr(1));
	int kd = StrInt(seq.GetStr(4));
	int cs = StrInt(seq.GetStr(5));
	if(!kd)
		return;

	dword mods = 0;
	if(cs & 0x0010) mods |= K_SHIFT;
	if(cs & (0x0004 | 0x0008)) mods |= K_CTRL;
	if(cs & (0x0001 | 0x0002)) mods |= K_ALT;

	dword key = 0;
	if(vk == 0x09) key = K_TAB;
	else if(vk == 0x0D) key = K_ENTER;
	else if(vk == 0x08) key = K_BACKSPACE;
	else if(vk == 0x1B) key = K_ESCAPE;
	else if(vk >= 0x20 && vk < 0x7F) key = vk;
	else if(vk >= 0x70 && vk <= 0x7B) key = K_F1 + (vk - 0x70);

	if(key) {
		if(key == K_TAB && (mods & K_ALT))
			TaskBar::CycleWindows();
		else
			Ctrl::DoKeyFB(key | mods, 1);
	}
}

void TuiVirtualGui::HandleMouseCsi(const VTInStream::Sequence& seq)
{
	int btn = StrInt(seq.GetStr(1));
	int col = StrInt(seq.GetStr(2));
	int row = StrInt(seq.GetStr(3));
	bool release = (seq.opcode == 'm');

	Size cell = TuiCellSize();
	Point p = Point((col - 1) * cell.cx + cell.cx / 2, (row - 1) * cell.cy + cell.cy / 2);
	mouse_pos = p;

	if(btn & 64) {
		int zdelta = (btn & 1) ? -120 : 120;
		Ctrl::DoMouseFB(Ctrl::MOUSEWHEEL, p, zdelta);
		return;
	}
	if(btn & 32) {
		Ctrl::DoMouseFB(Ctrl::MOUSEMOVE, p, 0);
		return;
	}

	int b = btn & 3;
	if(release) {
		if(b == 0) { mouse_buttons &= ~1; Ctrl::DoMouseFB(Ctrl::LEFTUP, p, 0); }
		else if(b == 2) { mouse_buttons &= ~2; Ctrl::DoMouseFB(Ctrl::RIGHTUP, p, 0); }
		else if(b == 1) { mouse_buttons &= ~4; Ctrl::DoMouseFB(Ctrl::MIDDLEUP, p, 0); }
	}
	else {
		if(b == 0) { mouse_buttons |= 1; Ctrl::DoMouseFB(Ctrl::LEFTDOWN, p, 0); }
		else if(b == 2) { mouse_buttons |= 2; Ctrl::DoMouseFB(Ctrl::RIGHTDOWN, p, 0); }
		else if(b == 1) { mouse_buttons |= 4; Ctrl::DoMouseFB(Ctrl::MIDDLEDOWN, p, 0); }
	}
}

void RunTuiVirtualGui(TuiVirtualGui& gui, Event<> app_main)
{
	TheTuiTerminal().Start();
	TuiInitFontSys();
	TuiInitDraw();
	TheTuiScreen().SetSize(TheTuiTerminal().GetCellSize());
	RunVirtualGui(gui, app_main);
	TheTuiTerminal().Stop();
}

END_UPP_NAMESPACE
