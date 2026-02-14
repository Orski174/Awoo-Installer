#pragma once
#include <string>
#include <vector>

namespace inst::util {
    struct LinkInfo {
        std::string url;
        std::string displayName;
        bool isDirectory;
    };

    std::vector<LinkInfo> parseHTMLLinks(const std::string& htmlContent, const std::string& baseUrl);
    std::vector<std::string> crawlForInstallableFiles(const std::string& url, int maxDepth = 2);
    bool isInstallableFile(const std::string& filename);
}
