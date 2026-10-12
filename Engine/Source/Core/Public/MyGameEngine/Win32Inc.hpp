#pragma once

// Make Windows.h as light as possible

#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS
#define NOSYSMETRICS
#define NOICONS
#define NOSYSCOMMANDS
#define NORASTEROPS
#define OEMRESOURCE
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NORPC
#define NOPROXYSTUB
#define NOIMAGE
#define NOTAPE
#define NOATOM
#define NOCLIPBOARD
#define NODRAWTEXT
#define NOKERNEL
#define NOMEMMGR
#define NOMETAFILE
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOMINMAX
#define NOMENUS
#define NOCOLOR
#define NOCTLMGR
#define NONLS
#define STRICT

#include <Windows.h>

// Should be used instead of NULL/nullptr for handles
constexpr auto nullhandle = nullptr;

// Here we must undefine everything that breaks compilation
#undef CreateWindow
#undef ERROR
