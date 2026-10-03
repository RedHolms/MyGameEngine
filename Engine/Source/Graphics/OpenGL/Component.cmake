set(COMPONENT_PRIVATE_SOURCES
  "Private/OpenGLFactory.cpp"
  "Private/OpenGLFactory.hpp"
  "Private/OpenGLRegistrar.cpp"
)

set(COMPONENT_PRIVATE_DEPENDENCIES
  Graphics/.DiscoveryInterface
  Graphics/Base
)

set(COMPONENT_REQUIRE_FULL_LINKAGE TRUE)
