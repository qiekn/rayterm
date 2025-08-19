#pragma once

#define MAKE_SINGLETON(ClassName)                   \
public:                                             \
  ClassName(const ClassName &) = delete;            \
  ClassName &operator=(const ClassName &) = delete; \
  ClassName(ClassName &&other) = delete;            \
  ClassName &operator=(ClassName &&) = delete;      \
  static ClassName &Get() {                         \
    static ClassName instance;                      \
    return instance;                                \
  }

#define CONSTRUCT_SINGLETON(ClassName) ClassName ::Get();
#define GET_SINGLETON(ClassName) ClassName ::Get();
