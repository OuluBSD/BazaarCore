#include "Local.h"

#if defined(VIRTUALGUI) && !defined(flagTUI)

NAMESPACE_UPP

void ChSysInit()
{
	ChReset();
}

void ChHostSkin()
{
	ChSysInit();
}

bool IsSystemThemeDark()
{
	return false;
}

END_UPP_NAMESPACE

#endif
