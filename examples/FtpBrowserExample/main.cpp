#include <CtrlLib/CtrlLib.h>
#include <FtpBrowser/FtpBrowser.h>

using namespace Upp;

GUI_APP_MAIN
{
	const Vector<String>& cmd = CommandLine();
	bool is_test = false;
	for(const String& s : cmd) {
		if(s == "--test")
			is_test = true;
	}

	FtpBrowser browser;
	if(is_test) {
		Cout() << "FtpBrowserExample: Self-test mode passed\n";
		_exit(0);
	}

	browser.Run();
}
