#include "util/html_util.hpp"
#include "util/curl.hpp"
#include "util/error.hpp"
#include <algorithm>
#include <cctype>
#include <regex>
#include <set>

namespace inst::util {

    std::string urlJoin(const std::string& base, const std::string& relative) {
        if (relative.find("://") != std::string::npos) {
            return relative;
        }
        
        std::string result = base;
        if (!result.empty() && result.back() != '/') {
            result += '/';
        }
        
        std::string rel = relative;
        if (!rel.empty() && rel.front() == '/') {
            size_t protoEnd = result.find("://");
            if (protoEnd != std::string::npos) {
                size_t pathStart = result.find('/', protoEnd + 3);
                if (pathStart != std::string::npos) {
                    result = result.substr(0, pathStart);
                }
            }
        }
        
        if (!rel.empty() && rel.front() == '/') {
            rel = rel.substr(1);
        }
        
        return result + rel;
    }

    std::string toLowerCase(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
                      [](unsigned char c){ return std::tolower(c); });
        return result;
    }

    bool isInstallableFile(const std::string& filename) {
        std::string lower = toLowerCase(filename);
        return lower.find(".nsp") != std::string::npos || 
               lower.find(".nsz") != std::string::npos ||
               lower.find(".xci") != std::string::npos || 
               lower.find(".xcz") != std::string::npos;
    }

    std::vector<LinkInfo> parseHTMLLinks(const std::string& htmlContent, const std::string& baseUrl) {
        std::vector<LinkInfo> links;
        std::set<std::string> seenUrls;
        
        std::regex hrefRegex(R"(<a[^>]+href\s*=\s*['\"]([^'\"]+)['\"][^>]*>([^<]*)</a>)", 
                            std::regex::icase);
        
        auto begin = std::sregex_iterator(htmlContent.begin(), htmlContent.end(), hrefRegex);
        auto end = std::sregex_iterator();
        
        for (std::sregex_iterator i = begin; i != end; ++i) {
            std::smatch match = *i;
            std::string href = match[1].str();
            std::string text = match[2].str();
            
            if (href.empty() || href == "#" || href.find("javascript:") == 0 || 
                href.find("mailto:") == 0 || href == "../" || href == "./") {
                continue;
            }
            
            std::string fullUrl = urlJoin(baseUrl, href);
            
            if (seenUrls.find(fullUrl) != seenUrls.end()) {
                continue;
            }
            seenUrls.insert(fullUrl);
            
            LinkInfo info;
            info.url = fullUrl;
            info.displayName = text.empty() ? href : text;
            info.isDirectory = (href.back() == '/' || 
                              (text.find("[DIR]") != std::string::npos) ||
                              (text.find("Directory") != std::string::npos));
            
            if (!info.isDirectory || isInstallableFile(href)) {
                info.isDirectory = false;
            }
            
            links.push_back(info);
        }
        
        return links;
    }

    std::vector<std::string> crawlForInstallableFiles(const std::string& url, int maxDepth) {
        std::vector<std::string> installableFiles;
        std::set<std::string> visited;
        std::vector<std::pair<std::string, int>> toVisit;
        
        toVisit.push_back({url, 0});
        
        while (!toVisit.empty() && installableFiles.size() < 256) {
            auto current = toVisit.back();
            toVisit.pop_back();
            
            std::string currentUrl = current.first;
            int depth = current.second;
            
            if (visited.find(currentUrl) != visited.end() || depth > maxDepth) {
                continue;
            }
            visited.insert(currentUrl);
            
            try {
                std::string htmlContent = inst::curl::downloadToBuffer(currentUrl, -1, -1, 10000);
                
                if (htmlContent.empty()) {
                    continue;
                }
                
                auto links = parseHTMLLinks(htmlContent, currentUrl);
                
                for (const auto& link : links) {
                    if (isInstallableFile(link.url)) {
                        installableFiles.push_back(link.url);
                    } else if (link.isDirectory && depth < maxDepth) {
                        toVisit.push_back({link.url, depth + 1});
                    }
                }
            } catch (...) {
                continue;
            }
        }
        
        return installableFiles;
    }
}
