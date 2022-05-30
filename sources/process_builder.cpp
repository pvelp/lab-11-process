//
// Created by pvelp on 09.05.22.
//

#include "process_build.hpp"

process::process() {}

void process::out_help()
{
  std::string out_info;
  out_info = R"(Usage: builder [options]
Allowed options:
  --help                    : выводим вспомогательное сообщение
  --config <Release|Debug>  : указываем конфигурацию сборки (по умолчанию Debug)
  --install                 : добавляем этап установки (в директорию _install)
  --pack                    : добавляем этап упаковки (в архив формата tar.gz)
  --timeout <count>         : указываем время ожидания (в секундах))";

  std::cout << out_info << std::endl;
}

void process::start_process(bool Install, bool Pack, std::string Config,
                            int Time)
{
  std::thread{std::bind(&process::timer, this, Time)}.detach();
  bool success;
  auto task = async::spawn([this, &success, &Config] {
    success = my_task("-H. -B_build -DCMAKE_INSTALL_PREFIX=_install "
        "-DCMAKE_BUILD_TYPE=" + Config);
  });
  task.wait();
  if (!success)
    return;
  task = async::spawn([this, &success]{
    success = my_task("--build _build");
  });
  task.wait();
  if (!success)
    return;
  if (Install&&Pack)
  {
    task = async::spawn([this, &success]{
      success = my_task("--build _build --target install");
    });
    task.wait();
    if (!success)
      return;
    task = async::spawn([this, &success]{
      success = my_task("--build _build --target package");
    });
    task.wait();
    if (!success)
      return;
  } else if (Install) {
    task = async::spawn([this, &success]{
      success = my_task("--build _build --target install");
    });
    task.wait();
    if (!success)
      return;
  } else if (Pack) {
    task = async::spawn([this, &success] {
      success = my_task("--build _build --target package");
    });
    task.wait();
    if (!success)
      return;
  }
}

bool process::my_task(std::string task)
{
  auto cmake_path = boost::process::search_path("cmake");

  boost::process::ipstream stream;
  boost::process::child child(cmake_path.string() + " " + task,
                              boost::process::std_out > stream);
  new_child = std::move(child);

  for (std::string line; new_child.running() &&
                         std::getline(stream, line);)
  {
    std::cout << line << std::endl;
  }

  new_child.wait();

  auto exit_code = new_child.exit_code();

  if (exit_code != 0) {
    std::cout << "Non zero exit code. Exiting..." << std::endl;
    return false;
  } else {
    return true;
  }
}

void process::timer(int time)
{
  sleep(time);
  new_child.terminate();
}

process::~process() {}

