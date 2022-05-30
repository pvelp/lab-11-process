//
// Created by pvelp on 09.05.22.
//

#ifndef INCLUDE_PROCESS_BUILDER_HPP_
#define INCLUDE_PROCESS_BUILDER_HPP_
#include <iostream>
#include <string>

#include <thread>
#include <async++.h>
#include <boost/process.hpp>
#include <boost/program_options.hpp>

class process
{
 public:
  process();
  ~process();
  void start_process(bool Install, bool Pack, std::string Config,
                     int Time);
  void timer(int time);
  bool my_task(std::string task);
  void out_help();
 private:
  boost::process::child new_child;
};
#endif // INCLUDE_PROCESS_BUILDER_HPP_

