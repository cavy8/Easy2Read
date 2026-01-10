#include "ImageMappings.h"
#include "PCH.h"
#include <SimpleIni.h>
#include <algorithm>
#include <filesystem>


namespace Easy2Read {

ImageMappings *ImageMappings::GetSingleton() {
  static ImageMappings singleton;
  return &singleton;
}

void ImageMappings::LoadMappings() {
  mappings.clear();

  const std::filesystem::path mappingsDir =
      L"Data/SKSE/Plugins/Easy2Read/ImageMappings";

  if (!std::filesystem::exists(mappingsDir)) {
    SKSE::log::info("ImageMappings folder not found, creating: {}",
                    mappingsDir.string());
    std::filesystem::create_directories(mappingsDir);
    return;
  }

  SKSE::log::info("Loading image mappings from: {}", mappingsDir.string());

  int fileCount = 0;
  for (const auto &entry : std::filesystem::directory_iterator(mappingsDir)) {
    if (entry.is_regular_file()) {
      auto ext = entry.path().extension().string();
      // Convert to lowercase for comparison
      std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

      if (ext == ".ini" || ext == ".txt") {
        LoadMappingFile(entry.path().wstring());
        fileCount++;
      }
    }
  }

  SKSE::log::info("Loaded {} mappings from {} files", mappings.size(),
                  fileCount);
}

void ImageMappings::LoadMappingFile(const std::wstring &filePath) {
  CSimpleIniA ini;
  ini.SetUnicode();
  ini.SetMultiKey(false);

  const auto rc = ini.LoadFile(filePath.c_str());
  if (rc < 0) {
    SKSE::log::warn("Failed to load mapping file: {}",
                    std::filesystem::path(filePath).string());
    return;
  }

  SKSE::log::debug("Loading mapping file: {}",
                   std::filesystem::path(filePath).filename().string());

  // Get all keys from [Mappings] section
  CSimpleIniA::TNamesDepend keys;
  ini.GetAllKeys("Mappings", keys);

  for (const auto &key : keys) {
    const char *value = ini.GetValue("Mappings", key.pItem, "");
    if (value && *value) {
      std::string imagePath = key.pItem;
      std::string replacement = value;

      // Normalize the path (forward slashes, lowercase)
      std::replace(imagePath.begin(), imagePath.end(), '\\', '/');

      mappings[imagePath] = replacement;
      SKSE::log::trace("  {} -> {}", imagePath, replacement);
    }
  }
}

std::string ImageMappings::GetReplacement(const std::string &imagePath) const {
  // Normalize the input path
  std::string normalized = imagePath;
  std::replace(normalized.begin(), normalized.end(), '\\', '/');

  // Try direct lookup first
  auto it = mappings.find(normalized);
  if (it != mappings.end()) {
    return it->second;
  }

  // Try case-insensitive lookup
  std::string lowerPath = normalized;
  std::transform(lowerPath.begin(), lowerPath.end(), lowerPath.begin(),
                 ::tolower);

  for (const auto &[key, value] : mappings) {
    std::string lowerKey = key;
    std::transform(lowerKey.begin(), lowerKey.end(), lowerKey.begin(),
                   ::tolower);
    if (lowerKey == lowerPath) {
      return value;
    }
  }

  return "";
}

bool ImageMappings::HasMapping(const std::string &imagePath) const {
  return !GetReplacement(imagePath).empty();
}

} // namespace Easy2Read
