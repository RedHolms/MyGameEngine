#pragma once

#include <MyGameEngine/Graphics/GraphicsAPIFactory.hpp>

class OpenGLFactory final : public GraphicsAPIFactory {
public:
  explicit OpenGLFactory(GraphicsAPIIdentifier id);
  ~OpenGLFactory() override;

public:
  GraphicsAPIInformation const& GetInformation() override;
  std::expected<std::unique_ptr<GraphicsAPI>, String> CreateAPI() override;

private:
  GraphicsAPIInformation m_information;
};
