#include "app.h"
#include "fmt/core.h"

#include <string>

static const std::string kGameTitle{"Turn Base RPG"};
static const int kScreenWidth = 912;
static const int kScreenHeight = 480;

int main(int argc, char const* argv[])
{
  fmt::print("Hello world!\n");
  App app{kGameTitle, kScreenWidth, kScreenHeight};

  try
  {
    app.Init();
    app.Run();
  }
  catch (const std::exception& e)
  {
    fmt::print("{}\n", e.what());
  }
  app.End();

  return 0;
}
