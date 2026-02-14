#include "util/html_util.hpp"
#include "util/curl.hpp"
#include "util/error.hpp"
#include <algorithm>
#include <cctype>
#include <set>
#include <sstream>

namespace inst::util {

    constexpr int MAX_CRAWL_RESULTS = 256;
    constexpr long CRAWL_TIMEOUT_MS = 10000;

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
        size_t len = lower.length();
        
        return (len > 4 && lower.substr(len - 4) == ".nsp") || 
               (len > 4 && lower.substr(len - 4) == ".nsz") ||
               (len > 4 && lower.substr(len - 4) == ".xci") || 
               (len > 4 && lower.substr(len - 4) == ".xcz");
    }

    std::string extractHrefValue(const std::string& htmlChunk) {
        size_t hrefPos = htmlChunk.find("href");
        if (hrefPos == std::string::npos) return "";
        
        size_t quoteStart = htmlChunk.find_first_of("\"'", hrefPos);
        if (quoteStart == std::string::npos) return "";
        
        char quoteChar = htmlChunk[quoteStart];
        size_t quoteEnd = htmlChunk.find(quoteChar, quoteStart + 1);
        if (quoteEnd == std::string::npos) return "";
        
        return htmlChunk.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
    }

    std::string extractLinkText(const std::string& htmlChunk) {
        size_t closingTag = htmlChunk.find('>');
        if (closingTag == std::string::npos) return "";
        
        size_t endTag = htmlChunk.find("</a>", closingTag);
        if (endTag == std::string::npos) return "";
        
        return htmlChunk.substr(closingTag + 1, endTag - closingTag - 1);
    }

    std::vector<LinkInfo> parseHTMLLinks(const std::string& htmlContent, const std::string& baseUrl) {
        std::vector<LinkInfo> links;
        std::set<std::string> seenUrls;
        
        size_t pos = 0;
        while (pos < htmlContent.length()) {
            size_t aTagStart = htmlContent.find("<a", pos);
            if (aTagStart == std::string::npos) break;
            
            size_t aTagEnd = htmlContent.find("</a>", aTagStart);
            if (aTagEnd == std::string::npos) break;
            
            std::string aTag = htmlContent.substr(aTagStart, aTagEnd - aTagStart + 4);
            std::string href = extractHrefValue(aTag);
            std::string text = extractLinkText(aTag);
            
            pos = aTagEnd + 4;
            
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
            info.isDirectory = (!href.empty() && href.back() == '/') || 
                              (text.find("[DIR]") != std::string::npos) ||
                              (text.find("Directory") != std::string::npos);
            
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
        
        while (!toVisit.empty() && installableFiles.size() < MAX_CRAWL_RESULTS) {
            auto current = toVisit.back();
            toVisit.pop_back();
            
            std::string currentUrl = current.first;
            int depth = current.second;
            
            if (visited.find(currentUrl) != visited.end() || depth > maxDepth) {
                continue;
            }
            visited.insert(currentUrl);
            
            try {
                std::string htmlContent = inst::curl::downloadToBuffer(currentUrl, -1, -1, CRAWL_TIMEOUT_MS);
                
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
            } catch (const std::exception& e) {
                LOG_DEBUG("Failed to crawl URL %s: %s\n", currentUrl.c_str(), e.what());
                continue;
            } catch (...) {
                LOG_DEBUG("Unknown error while crawling URL %s\n", currentUrl.c_str());
                continue;
            }
        }
        
        return installableFiles;
    }
}
