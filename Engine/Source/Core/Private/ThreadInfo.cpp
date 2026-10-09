#include <MyGameEngine/ThreadInfo.hpp>

#include <MyGameEngine/Bytes.hpp>
#include <MyGameEngine/EncodedString.hpp>
#include <MyGameEngine/Guards.hpp>
#include <MyGameEngine/Win32Inc.hpp>

static thread_local ThreadInfo s_CurrentThreadInfo {};

ThreadInfo const& MyGameEngine::GetCurrentThreadInformation() {
  s_CurrentThreadInfo.id = ::GetCurrentThreadId();

  wchar_t* threadName = nullptr;
  CallOnExit threadNameCleaner {
    [&] {
      if (threadName != nullptr)
        ::LocalFree(threadName);
    }
  };

  HRESULT result = ::GetThreadDescription(::GetCurrentThread(), &threadName);

  if (FAILED(result)) {
    s_CurrentThreadInfo.name = U"Unnamed"_s;
    s_CurrentThreadInfo.nameUtf8 = "Unnamed";
    return s_CurrentThreadInfo;
  }

  // TODO: Can we either make it faster or just cache it? Second one can be an issue if thread name changes at run time

  s_CurrentThreadInfo.name = String::Decode(
    Bytes { threadName, wcslen(threadName) * sizeof(wchar_t) },
    Encoding::WCHAR);

  auto nameEncoded = s_CurrentThreadInfo.name.Encode(Encoding::UTF8);

  // FIXME: Double allocation. Really bad
  s_CurrentThreadInfo.nameUtf8 = std::string { nameEncoded.GetCString<char>(), nameEncoded.GetLengthInBytes() };

  return s_CurrentThreadInfo;
}
