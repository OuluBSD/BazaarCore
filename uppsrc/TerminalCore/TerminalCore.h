#ifndef _TerminalCore_TerminalCore_h_
#define _TerminalCore_TerminalCore_h_

// The GUI-free half of the Terminal package: the VT cell model, the page
// buffer and the escape-sequence parser. Split out of uppsrc/Terminal so that
// more than one surface can share VT semantics instead of reimplementing them
// -- uppsrc/Terminal's own widget is one such surface, the TUI backend in
// uppsrc/CtrlCore is another. Nothing here references TerminalCtrl, and
// nothing here needs CtrlLib.
//
// Include this rather than the individual headers: it fixes the dependency
// order (Page needs Cell) in one place.
//
// Note for the input direction: VTInStream parses what a *child process*
// writes when uppsrc/Terminal uses it, and what the *terminal* writes back
// when the TUI backend uses it. The state machine is the same either way; the
// semantic tables are not, and those stay with each user.

#include <Core/Core.h>
#include <Draw/Draw.h>

#include "Parser.h"
#include "Cell.h"
#include "Page.h"
#include "Sixel.h"

#endif
