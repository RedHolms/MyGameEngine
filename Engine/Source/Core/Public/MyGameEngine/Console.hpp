#pragma once

namespace Console {

/**
 * On Win32:
 *    Will try to attach to parent's console (e.g. if game was run from terminal). If it fails to attach, new console
 *    will be allocated but its window will be hidden initially.
 */
void MakeSureConsolePresent();

} // namespace Console
