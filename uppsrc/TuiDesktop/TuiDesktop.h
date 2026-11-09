#ifndef _TuiDesktop_TuiDesktop_h_
#define _TuiDesktop_TuiDesktop_h_

#include <CtrlLib/CtrlLib.h>
#include <VirtualGui/VirtualGui.h>
#include <CtrlCore/TuiDraw.h>
#include <CtrlCore/TuiTerminal.h>
#include <CtrlCore/TuiFontSys.h>
#include <DesktopSuite/DesktopSuite.h>
#include <TerminalCore/TerminalCore.h>

NAMESPACE_UPP

class TuiVirtualGui : public VirtualGui {
public:
	virtual dword       GetOptions() override;
	virtual bool        WantsOwnWindowFrame() override;
	virtual Size        GetSize() override;
	virtual dword       GetMouseButtons() override;
	virtual dword       GetModKeys() override;
	virtual bool        IsMouseIn() override;
	virtual bool        ProcessEvent(bool *quit) override;
	virtual void        WaitEvent(int ms) override;
	virtual void        WakeUpGuiThread() override;
	virtual void        Quit() override;
	virtual bool        IsWaitingEvent() override;
	virtual SystemDraw& BeginDraw() override;
	virtual void        CommitDraw() override;

	TuiVirtualGui();
	virtual ~TuiVirtualGui();

private:
	TuiDraw    tui_draw;
	SystemDraw sys_draw;
	Size       desktop_size;
	dword      mouse_buttons;
	dword      mod_keys;
	bool       mouse_in;
	Point      mouse_pos;

	VTInStream parser;
	bool       parser_init;

	void InitParser();
	void OnChr(dword u);
	void OnCtl(byte c);
	void OnEsc(const VTInStream::Sequence& seq);
	void OnCsi(const VTInStream::Sequence& seq);
	void HandleMouseCsi(const VTInStream::Sequence& seq);
	void HandleWin32KeyCsi(const VTInStream::Sequence& seq);
};

void RunTuiVirtualGui(TuiVirtualGui& gui, Event<> app_main);

END_UPP_NAMESPACE

#endif
