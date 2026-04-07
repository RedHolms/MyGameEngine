extern bool TestBody();

uint MyGameEngineMain() {
  // Run the test body
  return TestBody() ? 0 : 1;
}
