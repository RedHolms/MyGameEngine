extern uint OsSpecificInit();
extern void OsSpecificTerminate();

extern uint GameMain();

uint Main() {
  uint exitCode;

  if ((exitCode = OsSpecificInit()) != 0)
    return exitCode;

  exitCode = GameMain();

  OsSpecificTerminate();

  return exitCode;
}
