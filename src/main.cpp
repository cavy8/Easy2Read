#include "Config/Settings.h"
#include "Hooks/D3D11Hook.h"
#include "Hooks/InputHandler.h"
#include "Hooks/MenuWatcher.h"
#include "Hooks/TextHooks.h"
#include "PCH.h"
#include "TextSanitization/TextSanitizer.h"
#include "UI/Overlay.h"
#include "Utils/ImageMappings.h"

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
    logger::info("Data loaded - initializing");

    // Load image-to-text mappings
    Easy2Read::ImageMappings::GetSingleton()->LoadMappings();

    // Configure text sanitizer from settings
    {
      auto *settings = Easy2Read::Settings::GetSingleton();
      auto *sanitizer = Easy2Read::TextSanitizer::GetSingleton();
      sanitizer->SetEnabled(settings->sanitizationEnabled);
      sanitizer->SetLogReplacements(settings->sanitizationLogReplacements);
      sanitizer->SetMaxExpansionRatio(settings->sanitizationMaxExpansionRatio);

      // Parse mode string
      if (settings->sanitizationMode == "Off") {
        sanitizer->SetMode(Easy2Read::SanitizationMode::Off);
      } else if (settings->sanitizationMode == "DetectOnly") {
        sanitizer->SetMode(Easy2Read::SanitizationMode::DetectOnly);
      } else {
        sanitizer->SetMode(Easy2Read::SanitizationMode::AnyASCII);
      }
    }

    // Install text sanitization hooks for global coverage
    if (Easy2Read::Settings::GetSingleton()->sanitizationEnabled) {
      // Allocate trampoline space for hooks (256 bytes should be plenty)
      SKSE::AllocTrampoline(256);
      Easy2Read::TextHooks::Install();
    }

    // Register event handlers
    Easy2Read::MenuWatcher::GetSingleton()->Register();
    Easy2Read::InputHandler::GetSingleton()->Register();

    // Install D3D11 hook and set up overlay render callback
    if (Easy2Read::D3D11Hook::GetSingleton()->Install()) {
      Easy2Read::D3D11Hook::GetSingleton()->SetRenderCallback(
          []() { Easy2Read::Overlay::GetSingleton()->Render(); });
    }
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
