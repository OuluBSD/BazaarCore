#ifndef _Core_Ext_Ext_h_
#define _Core_Ext_Ext_h_

#include <random>
#include <chrono>
#include <signal.h>
#include <Core/Core.h>
#include "OtherExt.h"

namespace Upp {

template <class C, class A>
inline C* CastPtr(A* a) { return dynamic_cast<C*>(a); }

template <class C, class A>
inline const C* CastConstPtr(const A* a) { return dynamic_cast<const C*>(a); }

inline bool IsAllDigit(const String& s) {
	if (s.IsEmpty()) return false;
	for (int c : s) if (!IsDigit(c)) return false;
	return true;
}

#include "Shared.h"
#include "Recycler.h"
#include "LinkedList.h"
#include "CritBitIndex.h"
#include "Timing.h"
#include "YAML.h"
#include "TOON.h"
#include "VCS.h"
#include "Visitor.h"
#include "Mlui.h"
#include "St.h"
#include "AppLog.h"
#include "CoreLog.h"
#include "AppRegistry.h"
#include "DiskBlobCache.h"
#include "CommandRegistry.h"
#include "Daemon.h"

#ifdef PLATFORM_UWP
#include "Uwp.h"
#endif

} // namespace Upp

#include "MetaTraits.h"
#include "DataStructures.h"
#include "ProcessUtil.h"
#include "MathNumeric.h"
#include "ColorGeom.h"
#include "CryptoExt.h"
#include "Cuboid.h"
#include "CompatExt.h"
#include "TextParsing.h"
#include "MluiScript.h"
#include "CmdArgs.h"
#include "Repl.h"
#include "InternalShell.h"
#include "AiCommon.h"
#include "VfsValueLegacy.h"

#endif
