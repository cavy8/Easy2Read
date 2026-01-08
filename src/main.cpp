#include "Config/Settings.h"
#include "PCH.h"


namespace {
/**
 * Setup logging using SKSE's logging system.
 * Log files are created in Documents/My Games/Skyrim Special Edition/SKSE/
 */
void SetupLog() {
  auto logsFolder = SKSE::log::log_directory();
  if (!logsFolder) {
    SKSE::stl::report_and_fail(
        "SKSE log_directory not provided, logs disabled.");
  }

  auto logPath = *logsFolder / "Easy2Read.log";
  auto fileLoggerPtr = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
      logPath.string(), true);
  auto loggerPtr =
      std::make_shared<spdlog::logger>("log", std::move(fileLoggerPtr));

  spdlog::set_default_logger(std::move(loggerPtr));
  spdlog::set_level(spdlog::level::info);
  spdlog::flush_on(spdlog::level::info);

  logger::info("Easy2Read v{} loaded",
               SKSE::PluginDeclaration::GetSingleton()->GetVersion().string());
}

/**
 * Handle SKSE messages (game lifecycle events).
 */
void MessageHandler(SKSE::MessagingInterface::Message *a_msg) {
  switch (a_msg->type) {
  case SKSE::MessagingInterface::kDataLoaded:
    logger::info("Data loaded - registering event handlers");
    // TODO: Register MenuWatcher here
    // TODO: Register InputHandler here
    break;
  case SKSE::MessagingInterface::kPostLoad:
    logger::info("Post load complete");
    break;
  case SKSE::MessagingInterface::kPostPostLoad:
    logger::info("Post-post load complete");
    break;
  default:
    break;
  }
}
} // namespace

/**
 * Plugin entry point - called by SKSE when the plugin is loaded.
 */
SKSEPluginLoad(const SKSE::LoadInterface *a_skse) {
  SKSE::Init(a_skse);

  SetupLog();
  logger::info("Easy2Read plugin initializing...");

  // Load configuration from INI
  Easy2Read::Settings::GetSingleton()->Load();

  // Get SKSE messaging interface for lifecycle events
  auto messaging = SKSE::GetMessagingInterface();
  if (!messaging) {
    logger::error("Failed to get SKSE messaging interface");
    return false;
  }

  messaging->RegisterListener(MessageHandler);

  logger::info("Easy2Read initialization complete");
  return true;
}
