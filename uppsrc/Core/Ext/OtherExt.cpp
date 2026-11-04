#include "Ext.h"

NAMESPACE_UPP

static Vector<Event<>>& sExitEvents(bool rm = false) {
	static Vector<Event<>>* v = nullptr;
	if (!v)
		v = new Vector<Event<>>();
	if (rm) {
		delete v;
		v = nullptr;
		static Vector<Event<>> empty;
		return empty;
	}
	return *v;
}

EXITBLOCK {
	for (Event<>& cb : sExitEvents())
		cb();
	sExitEvents(true);
}

void CallInExitBlock(Event<> cb) {
	static StaticMutex m;
	m.Enter();
	sExitEvents().Add(cb);
	m.Leave();
}

VectorMap<hash_t, String>& TypeStringHasherIndex::Map() {
	static VectorMap<hash_t, String> m;
	return m;
}

String TypeStringHasherIndex::ToString(hash_t h) {
	int i = Map().Find(h);
	return i >= 0 ? Map()[i] : Format("0x%X", (int64)h);
}

END_UPP_NAMESPACE
