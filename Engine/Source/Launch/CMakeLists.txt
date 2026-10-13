MyGameEngine_DefineEngineComponent(
  TYPE
    COMPILABLE_STATIC
  DEPENDS_ON
    Core
)

target_sources(${TARGET} PRIVATE
  "Private/WinMain.cpp"
)
