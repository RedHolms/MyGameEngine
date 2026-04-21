#pragma once

/**
 * Base exception interface
 * You should not extend it (extend RuntimeException instead)
 */
class Exception {
public:
  virtual ~Exception() = default;

public:
  // Returns real type of exception being thrown
  // You should always override it
  virtual TypeInfo const& GetExceptionType() const noexcept = 0;

  // Returns basic information about the exception
  virtual String const& GetInfo() const noexcept = 0;
};

/**
 * Base class for all runtime exceptions
 */
class RuntimeException : public Exception {
public:
  explicit inline RuntimeException(String const& info) : m_info(info) {}
  inline ~RuntimeException() override = default;

public:
  inline TypeInfo const& GetExceptionType() const noexcept override {
    return typeid(RuntimeException);
  }

  inline String const& GetInfo() const noexcept override {
    return m_info;
  }

private:
  String const m_info;
};
