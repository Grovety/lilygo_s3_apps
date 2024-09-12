#include "App.hpp"
#include "git_version.h"

extern "C" void app_main(void) {
  printf("VERSION: %s\n", VERSION_STRING);
  App app;
  app.run();
}
