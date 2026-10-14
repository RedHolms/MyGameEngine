#include <Private/Win32/Win32OpenGLContext.hpp>

#include <Private/Win32/WGLBootstrapper.hpp>

Win32OpenGLContext::Win32OpenGLContext(HGLRC glRc, HDC dc)
  : m_glResource(glRc),
    m_deviceContext(dc)
{
  // NOTE: Order is important

  // This function will select us as the current context and all every command it can
  LoadCommands();

  // This function will try to call glGetString(GL_VERSION) do determine feature level we're working with
  LoadFeatureLevel();

  if (!has_GL_VERSION_1_0)
    return; // Possible if LoadFeatureLevel() fails

  // Load extension lists

  LoadExtensionList();
  LoadWGLSpecificExtensionList();
}

Win32OpenGLContext::~Win32OpenGLContext() {
  // TODO
}

std::expected<void, String> Win32OpenGLContext::MakeCurrent() {
  auto& bootstrapper = WGLBootstrapper::GetLazyInstance();

  auto result = bootstrapper.wglMakeCurrentPtr(m_deviceContext, m_glResource);

  if (!result)
    return std::unexpected { U"wglMakeCurrent failed"_s }; // TODO: Format win32 error

  return {}; // success
}

std::expected<void, String> Win32OpenGLContext::SwapBuffers() {
  auto& bootstrapper = WGLBootstrapper::GetLazyInstance();

  auto result = bootstrapper.SwapBuffersPtr(m_deviceContext);

  if (!result)
    return std::unexpected { U"SwapBuffers failed"_s }; // TODO: Format win32 error

  return {}; // success
}

bool Win32OpenGLContext::HasExtension(char const* extensionName) {
  for (auto const& availableExtension : m_extensionList) {
    if (availableExtension == extensionName)
      return true;
  }

  for (auto const& availableExtension : m_wglSpecificExtensionList) {
    if (availableExtension == extensionName)
      return true;
  }

  return false;
}

void* Win32OpenGLContext::GetUnknownCommandProcedure(char const* commandName) {
  auto& bootstrapper = WGLBootstrapper::GetLazyInstance();
  return reinterpret_cast<void*>(bootstrapper.wglGetProcAddressPtr(commandName));
}

void Win32OpenGLContext::LoadCommands() {
  auto& bootstrapper = WGLBootstrapper::GetLazyInstance();

  // TODO Check for error
  (void) bootstrapper.wglMakeCurrentPtr(m_deviceContext, m_glResource);

  auto const loaderProc = [&](const char* name) -> void* {
    auto result = bootstrapper.wglGetProcAddressPtr(name);

    if (result != nullptr)
      return reinterpret_cast<void*>(result);

    // OpenGL 1.0 functions are only available as Opengl32.dll symbols
    return bootstrapper.GetOpengl32Symbol(name);
  };

#define _COMMAND(NAME, RETURN_TYPE, ...) \
  this->NAME = reinterpret_cast<NAME ## _t>(loaderProc(#NAME));
#include <MyGameEngine/Generated/OpenGL/Commands.generated.def>
#include <MyGameEngine/Generated/OpenGL/WGL/Commands.generated.def>
#undef _COMMAND
}

static uint32_t parseVersionPart(std::string_view string) {
  uint32_t result = 0;

  for (auto chr : string) {
    result *= 10;
    result += chr - '0';
  }

  return result;
}

void Win32OpenGLContext::LoadFeatureLevel() {
  if (!glGetString)
    return; // Under 1.0?

  std::string_view fullVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));

  // By specification:
  //   The GL_VERSION and GL_SHADING_LANGUAGE_VERSION strings are laid out as follows:
  //      <version number><space><vendor-specific information>
  //   The version number is either of the form major_number.minor_number or major_number.minor_number.release_number,
  //   where the numbers all have one or more digits. The minor number for SHADING_LANGUAGE_VERSION is always two
  //   digits, matching the OpenGL ES Shading Language Specification release number.

  auto spaceIndex = fullVersion.find(' ');
  std::string_view version = fullVersion.substr(0, spaceIndex);

  auto majorEnd = version.find('.');
  if (majorEnd == std::string_view::npos)
    return; // Invalid version format

  auto minorEnd = version.find('.', majorEnd+1);

  auto majorStr = version.substr(0, majorEnd);
  auto minorStr = version.substr(majorEnd+1, minorEnd-majorEnd-1);

  auto major = parseVersionPart(majorStr);
  auto minor = parseVersionPart(minorStr);

  has_GL_VERSION_4_6 = ( major > 4 )                 || ( major == 4 && minor >= 6 );
  has_GL_VERSION_4_5 = has_GL_VERSION_4_6 == GL_TRUE || ( major == 4 && minor >= 5 );
  has_GL_VERSION_4_4 = has_GL_VERSION_4_5 == GL_TRUE || ( major == 4 && minor >= 4 );
  has_GL_VERSION_4_3 = has_GL_VERSION_4_4 == GL_TRUE || ( major == 4 && minor >= 3 );
  has_GL_VERSION_4_2 = has_GL_VERSION_4_3 == GL_TRUE || ( major == 4 && minor >= 2 );
  has_GL_VERSION_4_1 = has_GL_VERSION_4_2 == GL_TRUE || ( major == 4 && minor >= 1 );
  has_GL_VERSION_4_0 = has_GL_VERSION_4_1 == GL_TRUE || ( major == 4               );
  has_GL_VERSION_3_3 = has_GL_VERSION_4_0 == GL_TRUE || ( major == 3 && minor >= 3 );
  has_GL_VERSION_3_2 = has_GL_VERSION_3_3 == GL_TRUE || ( major == 3 && minor >= 2 );
  has_GL_VERSION_3_1 = has_GL_VERSION_3_2 == GL_TRUE || ( major == 3 && minor >= 1 );
  has_GL_VERSION_3_0 = has_GL_VERSION_3_1 == GL_TRUE || ( major == 3               );
  has_GL_VERSION_2_1 = has_GL_VERSION_3_0 == GL_TRUE || ( major == 2 && minor >= 1 );
  has_GL_VERSION_2_0 = has_GL_VERSION_2_1 == GL_TRUE || ( major == 2               );
  has_GL_VERSION_1_5 = has_GL_VERSION_2_0 == GL_TRUE || ( major == 1 && minor >= 5 );
  has_GL_VERSION_1_4 = has_GL_VERSION_1_5 == GL_TRUE || ( major == 1 && minor >= 4 );
  has_GL_VERSION_1_3 = has_GL_VERSION_1_4 == GL_TRUE || ( major == 1 && minor >= 3 );
  has_GL_VERSION_1_2 = has_GL_VERSION_1_3 == GL_TRUE || ( major == 1 && minor >= 2 );
  has_GL_VERSION_1_1 = has_GL_VERSION_1_2 == GL_TRUE || ( major == 1 && minor >= 1 );
  has_GL_VERSION_1_0 = true;
}

static void parseSpaceSeparatedExtensionList(std::string_view stringList, std::vector<std::string_view>* outList) {
  size_t i = 0;
  while (i != std::string_view::npos) {
    size_t end = stringList.find(' ', i);
    outList->push_back(stringList.substr(i, end-i));
    i = stringList.find_first_not_of(' ', end);
  }
}

void Win32OpenGLContext::LoadExtensionList() {
  if (has_GL_VERSION_3_0) {
    // OpenGL 3.0 deprecates glGetString(GL_EXTENSIONS) in favor of getIntegerv (for GL_NUM_EXTENSIONS) and getStringi

    if (!glGetIntegerv || !glGetStringi)
      return; // Can't get any?

    GLint extensionsCount;
    glGetIntegerv(GL_NUM_EXTENSIONS, &extensionsCount);

    for (size_t i = 0; i < static_cast<GLuint>(extensionsCount); ++i) {
      auto extensionName = glGetStringi(GL_EXTENSIONS, i);
      m_extensionList.emplace_back(reinterpret_cast<const char*>(extensionName));
    }
  }
  else {
    if (!glGetString)
      return; // Can't get any?

    // This function returns space-separated list of extension names
    std::string_view extensionList = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));

    parseSpaceSeparatedExtensionList(extensionList, &m_extensionList);
  }
}

void Win32OpenGLContext::LoadWGLSpecificExtensionList() {
  std::string_view extensionList;

  if (wglGetExtensionsStringARB) {
    extensionList = wglGetExtensionsStringARB(m_deviceContext);
  }
  else if (wglGetExtensionsStringEXT) {
    extensionList = wglGetExtensionsStringEXT();
  }

  if (extensionList.empty())
    return;

  parseSpaceSeparatedExtensionList(extensionList, &m_wglSpecificExtensionList);
}
