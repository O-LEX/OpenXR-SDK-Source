#pragma once

#include <memory>
#include <string>
#include <vector>

struct Options;
struct PlatformData;

// Platform-specific plugin interface
struct IPlatformPlugin {
    virtual ~IPlatformPlugin() = default;

    virtual XrBaseInStructure* GetInstanceCreateExtension() const = 0;
    virtual std::vector<std::string> GetInstanceExtensions() const = 0;
    virtual void UpdateOptions(const std::shared_ptr<Options>& options) = 0;
};

// Linux-specific implementation (in-header)
inline std::shared_ptr<IPlatformPlugin> CreatePlatformPlugin(const std::shared_ptr<Options>& options,
                                                             const std::shared_ptr<PlatformData>& data) {
    (void)data;

    struct PosixPlatformPlugin : public IPlatformPlugin {
        PosixPlatformPlugin(const std::shared_ptr<Options>& /*unused*/) {}

        XrBaseInStructure* GetInstanceCreateExtension() const override { return nullptr; }
        std::vector<std::string> GetInstanceExtensions() const override { return {}; }
        void UpdateOptions(const std::shared_ptr<Options>& /*unused*/) override {}
    };

    return std::make_shared<PosixPlatformPlugin>(options);
}
