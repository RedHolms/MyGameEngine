#include "OpenGLFactory.hpp"

OpenGLFactory::OpenGLFactory(GraphicsAPIIdentifier id)
  : m_information({
      .id = id,
      .displayName = U"OpenGL"_s
    })
{}

OpenGLFactory::~OpenGLFactory() = default;

GraphicsAPIInformation const& OpenGLFactory::GetInformation() {
  return m_information;
}

std::expected<std::unique_ptr<GraphicsAPI>, String> OpenGLFactory::CreateAPI() {
  return std::unexpected { U"Unimplemented"_s };
}
