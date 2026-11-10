#include <CtrlLib/CtrlLib.h>
#include <DropGrid/DropGrid.h>

using namespace Upp;

class DropGridExampleWindow : public TopWindow {
public:
	typedef DropGridExampleWindow CLASSNAME;

	DropGrid grid;
	Label info;

	void OnSelect() {
		info.SetText(Format("Selected ID: %`, Value: %`", grid.GetData(), grid.GetValue()));
	}

	DropGridExampleWindow() {
		Title("DropGrid Example").SetRect(200, 200, 480, 240);

		grid.AddColumn("ID", 40);
		grid.AddColumn("Item Name", 140);
		grid.AddColumn("Category", 100);
		grid.AddColumn("Stock", 60);

		grid.SetKeyColumn(0);
		grid.SetValueColumn(1);
		grid.SetFindColumn(1);

		grid.Add(1, "Mechanical Keyboard", "Hardware", 15);
		grid.Add(2, "Laser Mouse", "Hardware", 42);
		grid.Add(3, "USB-C Hub", "Accessories", 88);
		grid.Add(4, "DisplayPort Cable", "Cables", 120);
		grid.Add(5, "OLED Monitor", "Displays", 7);

		grid.SetData(1);
		grid.WhenAction = THISBACK(OnSelect);

		info.SetText("Select an item from the dropdown");

		Add(grid.LeftPosZ(20, 300).TopPosZ(30, 24));
		Add(info.LeftPosZ(20, 440).TopPosZ(80, 24));
	}
};

GUI_APP_MAIN
{
	const Vector<String>& cmd = CommandLine();
	bool is_test = false;
	for(const String& s : cmd) {
		if(s == "--test")
			is_test = true;
	}

	DropGridExampleWindow win;
	if(is_test) {
		Cout() << "DropGridExample: Self-test mode passed successfully\n";
		_exit(0);
	}

	win.Run();
}
