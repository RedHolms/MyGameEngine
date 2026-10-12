#pragma once

#include <MyGameEngine/Logging/IAppender.hpp>
#include <MyGameEngine/Logging/IFormatter.hpp>
#include <functional>
#include <map>
#include <string_view>

namespace Logging {

struct Configuration {
  struct Group {
    std::function<bool(LoggingRecord const&)> condition;
    size_t formatterIndex;
    std::vector<size_t> appendersIndices;
  };

  std::vector<std::unique_ptr<IAppender>> appenders;
  std::vector<std::unique_ptr<IFormatter>> formatters;
  std::vector<Group> groups;
};

void Configure(Configuration&& configuration);

class ConfigurationBuilder final {
private:
  class GroupConfigurationBuilder {
  public:
    constexpr explicit GroupConfigurationBuilder(ConfigurationBuilder* parent) : m_parent(parent) {}

  public:
    GroupConfigurationBuilder& SetCondition(std::function<bool(LoggingRecord const&)>&& pred) {
      m_condition = std::move(pred);
      return *this;
    }

    GroupConfigurationBuilder& SetFormatter(std::string_view id) {
      auto it = m_parent->m_formattersIndicesByNameMap.find(std::string { id });
      if (it == m_parent->m_formattersIndicesByNameMap.end())
        throw std::runtime_error("Invalid formatter ID passed to group configuration");
      m_formatterIndex = it->second;
      return *this;
    }

    GroupConfigurationBuilder& AppendTo(std::string_view appenderId) {
      auto it = m_parent->m_appendersIndicesByNameMap.find(std::string { appenderId });
      if (it == m_parent->m_appendersIndicesByNameMap.end())
        throw std::runtime_error("Invalid appender ID passed to group configuration");
      m_appendersIndices.push_back(it->second);
      return *this;
    }

    ConfigurationBuilder& EndGroup() {
      m_parent->m_groups.push_back(Configuration::Group {
        .condition = std::move(m_condition),
        .formatterIndex = m_formatterIndex,
        .appendersIndices = std::move(m_appendersIndices)
      });
      return *m_parent;
    }

  private:
    ConfigurationBuilder* m_parent;
    std::function<bool(LoggingRecord const&)> m_condition;
    size_t m_formatterIndex = -1;
    std::vector<size_t> m_appendersIndices;
  };

public:
  constexpr ConfigurationBuilder() = default;

public:
  constexpr operator Configuration() const noexcept {
    return Configuration {
      .appenders = std::move(m_appenders),
      .formatters = std::move(m_formatters),
      .groups = std::move(m_groups)
    };
  }

public:
  /**
   * Registers new appender and attaches string ID to it.
   */
  ConfigurationBuilder& AddAppender(std::string_view id, std::unique_ptr<IAppender>&& appender) {
    size_t index = m_appenders.size();
    m_appenders.push_back(std::move(appender));
    m_appendersIndicesByNameMap.emplace(std::string { id }, index);
    return *this;
  }

  /**
   * Builds new appender and registers it and attaches string ID to it.
   */
  template <class BuilderT>
    requires requires(BuilderT appenderBuilder) {
      { appenderBuilder.Build() } -> std::same_as<std::unique_ptr<IAppender>>;
    }
  ConfigurationBuilder& BuildAppender(std::string_view id, BuilderT&& appenderBuilder) {
    return AddAppender(id, appenderBuilder.Build());
  }

  /**
   * Registers new formatter and attaches string ID to it.
   */
  ConfigurationBuilder& AddFormatter(std::string_view id, std::unique_ptr<IFormatter>&& formatter) {
    size_t index = m_formatters.size();
    m_formatters.push_back(std::move(formatter));
    m_formattersIndicesByNameMap.emplace(std::string { id }, index);
    return *this;
  }

  /**
   * Builds new formatter and registers it and attaches string ID to it.
   */
  template <class BuilderT>
    requires requires(BuilderT formatterBuilder) {
      { formatterBuilder.Build() } -> std::same_as<std::unique_ptr<IFormatter>>;
    }
  ConfigurationBuilder& BuildFormatter(std::string_view id, BuilderT&& formatterBuilder) {
    return AddFormatter(id, formatterBuilder.Build());
  }

  GroupConfigurationBuilder BeginGroup() {
    return GroupConfigurationBuilder { this };
  }

private:
  std::map<std::string, size_t> m_appendersIndicesByNameMap;
  std::map<std::string, size_t> m_formattersIndicesByNameMap;
  std::vector<std::unique_ptr<IAppender>> m_appenders;
  std::vector<std::unique_ptr<IFormatter>> m_formatters;
  std::vector<Configuration::Group> m_groups;
};

} // namespace Logging
