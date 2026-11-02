#include "Ext.h"

NAMESPACE_UPP

VectorMap<hash_t, String>& TypeStringHasherIndex::Map() {
	static VectorMap<hash_t, String> m;
	return m;
}

String TypeStringHasherIndex::ToString(hash_t h) {
	int i = Map().Find(h);
	return i >= 0 ? Map()[i] : HexStr64(h);
}

END_UPP_NAMESPACE
