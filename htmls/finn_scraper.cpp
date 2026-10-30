// finn_scraper.cpp
// Walks https://www.finn.no/mobility/search/car?page=N (N = start..end),
// finds links of the form /mobility/item/<id>, downloads each item page once
// and saves it as <id>.html in the output directory.
//
// Build (Linux/macOS):
//   g++ -std=c++17 -O2 finn_scraper.cpp -o finn_scraper -lcurl
// Build (Windows/MSYS2): g++ -std=c++17 -O2 finn_scraper.cpp -o finn_scraper -lcurl
//
// Usage:
//   ./finn_scraper [start_page=6] [end_page=50] [output_dir=finn_pages]

#include <curl/curl.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <set>
#include <string>
#include <thread>

namespace fs = std::filesystem;

static const std::string kBaseUrl = "https://www.finn.no";
static const int kDelayMs = 10;  // pause between requests, be polite to the server

static size_t writeCallback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    auto* out = static_cast<std::string*>(userdata);
    out->append(ptr, size * nmemb);
    return size * nmemb;
}

// Downloads url into 'body'. Returns true on HTTP 200.
static bool httpGet(CURL* curl, const std::string& url, std::string& body) {
    body.clear();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");  // allow gzip/br, auto-decoded
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT,
                     "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 "
                     "(KHTML, like Gecko) Chrome/120.0 Safari/537.36");

    CURLcode rc = curl_easy_perform(curl);
    if (rc != CURLE_OK) {
        std::cerr << "  curl error: " << curl_easy_strerror(rc) << "\n";
        return false;
    }
    long code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
    if (code != 200) {
        std::cerr << "  HTTP " << code << " for " << url << "\n";
        return false;
    }
    return true;
}

static void sleepMs(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

int main(int argc, char** argv) {
    int startPage = argc > 1 ? std::stoi(argv[1]) : 6;
    int endPage   = argc > 2 ? std::stoi(argv[2]) : 50;
    fs::path outDir = argc > 3 ? argv[3] : "finn_pages";

    fs::create_directories(outDir);

    // Ids already saved in earlier runs count as "already downloaded".
    std::set<std::string> seen;
    for (const auto& entry : fs::directory_iterator(outDir)) {
        
        seen.insert(entry.path().stem().string());
    }
    std::cout << "Found " << seen.size() << " previously saved pages.\n";

    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to init curl\n";
        return 1;
    }

    // Matches .../mobility/item/479379121 (absolute or relative links)
    const std::regex itemRe(R"(mobility/item/(\d+))");

    int downloaded = 0;
    std::string var = "?location=0.20016";

    for (int page = startPage; page <= endPage; ++page) {
    
        std::string searchUrl = kBaseUrl + "/mobility/search/car" + var;
        if(page >= 1) searchUrl += "&page=" + std::to_string(page);
        
        std::cout << "\n=== Search page " << page << "=== Pagueurl == " << searchUrl <<" ===\n";

        std::string html;
        if (!httpGet(curl, searchUrl, html)) {
            std::cerr << "  Skipping search page " << page << "\n";
            sleepMs(kDelayMs);
            continue;
        }

        // Collect unique item ids on this page, in order of appearance.
        std::set<std::string> pageIds;
        for (auto it = std::sregex_iterator(html.begin(), html.end(), itemRe);
             it != std::sregex_iterator(); ++it) {
            pageIds.insert((*it)[1].str());
        }
        std::cout << "  Found " << pageIds.size() << " item links.\n";

        for (const auto& id : pageIds) {
            if (seen.count(id)) {
                std::cout << "  " << id << " already downloaded, skipping.\n";
                continue;
            }

            sleepMs(kDelayMs);
            std::string itemUrl = kBaseUrl + "/mobility/item/" + id;
            std::string itemHtml;
            if (!httpGet(curl, itemUrl, itemHtml)) continue;

            fs::path outFile = outDir / (id);
            std::ofstream out(outFile, std::ios::binary);
            if (!out) {
                std::cerr << "  Could not write " << outFile << "\n";
                continue;
            }
            out << itemHtml;
            out.close();

            seen.insert(id);
            ++downloaded;
            std::cout << "  Saved " << outFile.string() << " (" << itemHtml.size() << " bytes)\n";
        }

        sleepMs(kDelayMs);
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();

    std::cout << "\nDone. Downloaded " << downloaded << " new pages.\n";
    return 0;
}
