/* SPDX-License-Identifier: MIT */
//
// Copyright (c) 2024 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

//----------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------

#include <fcntl.h>
#include <getopt.h>

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#include <cppsl/log/multiSinkWizard.hpp>
#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "appContext.hpp"
#include "daemon.hpp"
#include "daemonConfig.hpp"
#include "version.hpp"

using namespace std::chrono_literals;

//----------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Typedefs, enums, unions, variables
//----------------------------------------------------------------------------
enum class handleConsoleType {
  none,
  reload,
  exit,
};

struct DaemonEvent {
  std::mutex event_mutex;
  std::condition_variable event_condition;
};

static DaemonEvent daemon_event;

/**
 * @brief The options for the program.
 */
static const std::array<std::string_view, 8> OPTIONS = {
    "  -D, --background         start as daemon\n", "  -C, --cfgfile            specified configuration file\n",
    "  -P, --pidfile            create pid file\n", "  -v, --version            version\n",
    "  -h, --help               this message\n"};

/**
 *  @brief The help options for the program.
 */
static const char* help_options = "h?vDP:C:";
static const struct option long_options[] = {
    {"help", no_argument, nullptr, 0},
    {"version", no_argument, nullptr, 'v'},
    {"background", no_argument, nullptr, 'D'},
    {"pidfile", required_argument, nullptr, 'P'},
    {"cfgfile", required_argument, nullptr, 'C'},
    {nullptr, 0, nullptr, 0},
};

/**
 * @brief The sample command lines for the program.
 */
static const std::array<std::string_view, 3> SAMPLE_COMMANDS = {
    " -F\n", " -D -P /var/run/some.pid\n", " -D -C /app/config/config.yaml -P /var/run/some.pid\n"};

//----------------------------------------------------------------------------
// Prototypes
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Declarations
//----------------------------------------------------------------------------

/*************************************************************************/ /**
 * @brief Prints the sample command lines for the program.
 * @param programName The name of the program.
 * @return None
 *****************************************************************************/
static void print_sample_commands(const char* programName) {
  std::cout << "\nSample command lines:" << std::endl;
  for (const auto& cmd : SAMPLE_COMMANDS) {
    std::cout << programName << cmd;
  }
}

/***************************************************************************/ /**
 * @brief Displays the version information of the program.
 * @param prog The name of the program.
 * @return None
 ******************************************************************************/
static void show_version(const char* prog) {
  auto verString = version::taskController::getVersion(true);
  // info
  std::cout << prog << " v." << verString << std::endl;
}

/*************************************************************************/ /**
 * Displays the help message for the program.
 * @param programName The name of the program.
 * @param var The character representing the option with an error. Defaults to 0.
 *****************************************************************************/
static void display_help(const char* programName, std::string_view errorOption = "") {
  if (!errorOption.empty()) {
    std::cerr << "Error in option: " << errorOption << "\n";
  }
  std::cout << "\nUsage: " << programName << " [OPTIONS]\n" << std::endl;
  for (const auto& option : OPTIONS) {
    std::cout << option;
  }
  // output sample commands
  print_sample_commands(programName);

  if (!errorOption.empty()) {
    exit(EXIT_FAILURE);
  }
}

/*************************************************************************/ /**
 * @brief Handles the argument for a given option.
 * @param option The option for which the argument is provided.
 * @param argument The argument provided for the option.
 * @param argv0 The name of the program.
 * @return None
 ****************************************************************************/
void handle_option_argument(const char* option, const char* argument, const char* argv0) {
  if (!strlen(argument)) {
    std::cerr << "Missing " << option << " argument for option\n";
    display_help(argv0);
  }
}

/*************************************************************************/ /**
 * @brief Processes the command line options passed to the program.
 *
 * This function parses the command line options and sets the corresponding
 * variables based on the provided values. It uses getopt_long function to
 * handle both short and long options.
 *
 * @param argc The number of command line arguments.
 * @param argv The array of command line argument strings.
 * @param config
 *****************************************************************************/
static void process_command_line(int argc, char* argv[], app::DaemonConfig& config) {
  int option_index = 0;
  for (;;) {
    int current_option = getopt_long(argc, argv, help_options, long_options, &option_index);
    if (current_option == -1) {
      break;
    }

    switch (current_option) {
      case 0:
        display_help(argv[0]);
        break;

      case 'h':
      case '?':
        display_help(argv[0]);
        exit(EXIT_SUCCESS);

      case 'v':
        show_version(argv[0]);
        exit(EXIT_SUCCESS);

      case 'D':
        config.isDaemon = true;
        break;

      case 'P':
        handle_option_argument("pid-file", optarg, argv[0]);
        config.pidFile.assign(optarg);
        break;

      case 'C':
        handle_option_argument("configuration file", optarg, argv[0]);
        config.configFile.assign(optarg);
        break;

      default:
        std::cerr << "Unknown option: " << std::to_string(current_option) << std::endl;
        display_help(argv[0]);
    }
  }
}

/*************************************************************************/ /**
 * @brief handle console input
 *****************************************************************************/
handleConsoleType handle_console() {
  auto key = getchar();
  switch (key) {
    case 'q':
      return handleConsoleType::exit;
    case 'R':
      return handleConsoleType::reload;
    case 'v':
      // info
      std::cout << " v." << version::taskController::getVersion(true) << std::endl;
      break;
    case '?':
    case 'h':
      fprintf(stderr, "Application test console:\n");
      fprintf(stderr, " R   -  execute reload functions\n");
      fprintf(stderr, " q   -  quit from application.\n");
      fprintf(stderr, " v   -  version\n");
      fprintf(stderr, " h|? -  this information.\n");
      break;
    default:;
  }
  return handleConsoleType::none;
}

/*************************************************************************/ /**
 * @brief Subscriber main function
 * @desc Threads cannot always actively monitor a stop token.
 * @param lptr - log appender
 * @param cfg_converter - configuration
 * @param stopToken - stop task token
 *****************************************************************************/
void AppContextWorker(app::AppContext& app_context, app::DaemonConfig& daemon_config, std::stop_token stopToken) {
  auto sooner = 1000ms;

  // Register a stop callback
  std::stop_callback stop_cb(stopToken, [&]() {
    // Wake thread on stop request
    daemon_event.event_condition.notify_all();
  });

  spdlog::info("application task started");

  while (!stopToken.stop_requested()) {
    spdlog::info("application task ticks {} ms", sooner.count());
    // observe serves states
    sooner = app_context.ProcessExecuting(sooner);
    if (sooner.count() > 0) {
      // Start of locked block
      std::unique_lock lck(daemon_event.event_mutex);
      daemon_event.event_condition.wait_for(lck, std::chrono::milliseconds(sooner));
    }
  }  // End of while loop

  spdlog::info("application context got stop token. Application task completed");
}

/*************************************************************************/ /**
 * @file main.c
 * @brief This is the main entry point for the application.
 *****************************************************************************/
int main(int argc, char** argv) {
  // The Daemon class is a singleton to avoid be instantiated more than once
  app::Daemon& daemon = app::Daemon::instance();
  app::DaemonConfig appConfig;  ///< The configuration of the daemon
  app::AppContext appContext;   ///< The application context
  cppsl::log::LogManagerPtr logManPtr{cppsl::log::CreateLoggingManager("main")};

  if (!logManPtr->add_console_sink(cppsl::log::MultiSinkWizard::OutputLog::err,
                                   cppsl::log::MultiSinkWizard::Colored::color, spdlog::level::debug)) {
    spdlog::warn("cannot add console log");
  }

  if (!logManPtr->openLogger(spdlog::level::debug)) {
    spdlog::warn("cannot open log manager");
  }

  logManPtr->critical("Log manager is okay? Empty: {}?", logManPtr->empty());

  //----------------------------------------------------------
  // parse parameters
  //----------------------------------------------------------
  process_command_line(argc, argv, appConfig);

  //----------------------------------------------------------
  // set in daemon all handlers
  //----------------------------------------------------------
  daemon.SetStartFunction([&]() {
    spdlog::info("Start all function called.");
    return appContext.ProcessStart();
  });

  // Set the stop all function
  daemon.set_close_function([&]() {
    spdlog::info("Close all function called.");
    return appContext.ProcessShutdown();
  });

  daemon.set_reload_function([&]() {
    spdlog::info("Reload function called.");
    return appContext.ProcessReconfigure();
  });

  daemon.set_user1_function([&]() {
    spdlog::info("User1 function called.");
    return appContext.ProcessSignalUser1();
  });

  daemon.set_user2_function([&]() {
    spdlog::info("User2 function called.");
    return appContext.ProcessSignalUser2();
  });

  //----------------------------------------------------------
  // Check integrity this configuration
  //----------------------------------------------------------
  if (auto result = appContext.ValidateConfig(appConfig); result.has_value() && !result.value()) {
    spdlog::warn("AppContext configuration mismatch. Exit");
    exit(EXIT_FAILURE);
  }

  //----------------------------------------------------------
  // Start all
  //----------------------------------------------------------
  if (auto result = daemon.StartAll(); result.has_value() && !result.value()) {
    spdlog::warn("Error starting the daemon.");
    return EXIT_FAILURE;
  }

  if (appConfig.isDaemon) {
    if (auto result = daemon.make_daemon(appConfig.pidFile); result.has_value() && !result.value()) {
      spdlog::warn("Error starting the daemon.");
      return EXIT_FAILURE;
    }
  }

  //----------------------------------------------------------
  // start application task
  //----------------------------------------------------------
  // Create a stop_source
  std::stop_source stopSource;
  // Get a stop_token from the stop_source
  std::stop_token stopToken = stopSource.get_token();
  // Create all workers and pass stop tokens
std::thread taskAppCtx =
      std::move(std::thread(AppContextWorker, std::ref(appContext), std::ref(appConfig), stopSource.get_token()));

  //----------------------------------------------------------
  // Main loop

  if (!appConfig.isDaemon) {
    std::cout << "Press the h key to display the Console Menu..." << std::endl;
  }

  // Daemon main loop
  while (daemon.is_running()) {
    if (!appConfig.isDaemon) {
      auto result = handle_console();
      switch (result) {
        case handleConsoleType::exit:
          daemon.set_state(app::Daemon::State::stop);
          break;
        case handleConsoleType::reload:
          daemon.set_state(app::Daemon::State::reload);
          break;
        case handleConsoleType::none:
        default:
          break;
      }
    } else {
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }

  // set token to stop all worker
  stopSource.request_stop();

  spdlog::info("The daemon process is stopping");

  // wakeup all tasks
  {
    std::unique_lock lck(daemon_event.event_mutex);
    daemon_event.event_condition.notify_all();
  }

  spdlog::info("Waiting for the application task to complete");

  // Join threads
  taskAppCtx.join();

  if (auto result = daemon.CloseAll(); result.has_value() && !result.value()) {
    spdlog::error("Error closing the daemon.");
    return EXIT_FAILURE;
  }

  spdlog::info("The daemon process ended successfully");

  return EXIT_SUCCESS;
}