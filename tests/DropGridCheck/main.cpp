#include <CtrlLib/CtrlLib.h>
#include <DropGrid/DropGrid.h>

using namespace Upp;

GUI_APP_MAIN
{
	StdLogSetup(LOG_COUT | LOG_FILE);
	Cout() << "=== DropGridCheck: Starting Automated Tests ===\n";

	DropGrid grid;

	// 1. Column configuration
	grid.AddColumn("ID", 50);
	grid.AddColumn("Name", 120);
	grid.AddColumn("Category", 100);

	grid.SetKeyColumn(0);
	grid.SetValueColumn(1);
	grid.SetFindColumn(1);

	Cout() << "[1/6] Columns configured: Key=0, Value=1, Find=1\n";

	// 2. Initial state verification
	ASSERT(grid.GetCount() == 0);
	ASSERT(grid.IsEmpty());
	Cout() << "[2/6] Initial state verified (count=0, empty=true)\n";

	// 3. Row addition
	grid.Add(101, "Alpha", "Cat-A");
	grid.Add(102, "Beta", "Cat-B");
	grid.Add(103, "Gamma", "Cat-G");

	ASSERT(grid.GetCount() == 3);
	ASSERT(!grid.IsEmpty());
	Cout() << "[3/6] Rows inserted (count=3)\n";

	// 4. Cell retrieval verification
	ASSERT(grid.Get(0, 0) == 101);
	ASSERT(grid.Get(0, 1) == "Alpha");
	ASSERT(grid.Get(0, 2) == "Cat-A");

	ASSERT(grid.Get(1, 0) == 102);
	ASSERT(grid.Get(1, 1) == "Beta");
	ASSERT(grid.Get(1, 2) == "Cat-B");

	ASSERT(grid.Get(2, 0) == 103);
	ASSERT(grid.Get(2, 1) == "Gamma");
	ASSERT(grid.Get(2, 2) == "Cat-G");
	Cout() << "[4/6] Cell values verified\n";

	// 5. Data binding, Find & FindValue
	grid.SetData(102);
	ASSERT(grid.GetData() == 102);
	ASSERT(grid.GetValue() == "Beta");
	ASSERT(grid.GetKey() == 102);

	int idx = grid.Find(103, 0);
	ASSERT(idx == 2);
	ASSERT(grid.Find(999, 0) == -1);

	ASSERT(grid.FindValue(101) == "Alpha");
	ASSERT(grid.FindValue(103) == "Gamma");

	bool moved = grid.FindMove(103);
	ASSERT(moved);
	bool not_found = grid.FindMove(999);
	ASSERT(!not_found);

	grid.SetData(103);
	ASSERT(grid.GetData() == 103);
	ASSERT(grid.GetValue() == "Gamma");
	ASSERT(grid.GetKey() == 103);
	Cout() << "[5/6] Key/Data mapping and lookup verified\n";

	// 6. Clear lifecycle
	grid.Clear();
	ASSERT(grid.GetCount() == 0);
	ASSERT(grid.IsEmpty());
	Cout() << "[6/6] Clear lifecycle verified (count=0, empty=true)\n";

	Cout() << "=== DropGridCheck: All 6 Test Phases PASSED ===\n";
	_exit(0);
}
