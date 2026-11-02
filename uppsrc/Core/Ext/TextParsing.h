#ifndef _Core_Ext_TextParsing_h_
#define _Core_Ext_TextParsing_h_

#include <Core/Core.h>

NAMESPACE_UPP

#include "Process.h"
#include "ExtUtil.h"
#include "TextString.h"
#include "Tokenizer.h"
#include "TokenParser.h"
#include "Html.h"
#include "Url.h"

String NormalizePathSeparators(const String& path);

END_UPP_NAMESPACE

#endif
