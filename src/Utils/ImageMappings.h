#pragma once

#include <string>
#include <unordered_map>

namespace Easy2Read {

/**
 * Manages image-to-text mappings loaded from config files.
 * Supports multiple mapping files so mod authors can add their own.
 */
class ImageMappings {
public:
  [[nodiscard]] static ImageMappings *GetSingleton();

  /**
   * Load all mapping files from the ImageMappings folder.
   * Each .ini file in the folder is parsed for path=replacement pairs.
   */
  void LoadMappings();

  /**
   * Get the text replacement for an image path.
   * @param imagePath The image path from the img tag (e.g.,
   * "Textures/Interface/Books/...")
   * @return The replacement text, or empty string if no mapping exists
   */
  [[nodiscard]] std::string GetReplacement(const std::string &imagePath) const;

  /**
   * Check if a mapping exists for the given path.
   */
  [[nodiscard]] bool HasMapping(const std::string &imagePath) const;

  /**
   * Get the number of loaded mappings.
   */
  [[nodiscard]] size_t GetMappingCount() const { return mappings.size(); }

private:
  ImageMappings() = default;
  ImageMappings(const ImageMappings &) = delete;
  ImageMappings(ImageMappings &&) = delete;
  ~ImageMappings() = default;

  ImageMappings &operator=(const ImageMappings &) = delete;
  ImageMappings &operator=(ImageMappings &&) = delete;

  void LoadMappingFile(const std::wstring &filePath);

  // Map of image path -> replacement text
  std::unordered_map<std::string, std::string> mappings;
};

} // namespace Easy2Read
