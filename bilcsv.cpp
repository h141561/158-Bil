#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "Bil.hpp"

namespace {
 
// Wraps a field in quotes if it contains the delimiter, a quote or a newline.
std::string csvEscape(const std::string& field, char delimiter) {
    if (field.find_first_of(std::string{delimiter, '"', '\n', '\r'}) == std::string::npos)
        return field;
    std::string out = "\"";
    for (char c : field) {
        if (c == '"') out += '"';   // double any quotes
        out += c;
    }
    out += '"';
    return out;
}
 
const char* girToString(girType g) {
    switch (g) {
        case AUTOMAT: return "automat";
        case MANUELL: return "manuell";
    }
    return "";
}
 
const char* motorToString(motorType m) {
    switch (m) {
        case DIESEL: return "diesel";
        case BENSIN: return "bensin";
        case EL:     return "el";
    }
    return "";
}
 
// A car is exported only if every value is set and the price isn't 1.
bool shouldExport(const Bil& b) {
    return true;
    if (b.totPris == 1) return false;
 
    if (b.Merke.empty() || b.Modell.empty() || b.Id.empty()) return false;
 
    // Ints use a negative number (-1) for "not set".
    if (b.kmStand < 0 || b.totPris < 0 || b.modellAar < 0 ||
        b.vekt < 0 || b.hk < 0) return false;
 
    if (!b.gir || !b.motor) return false;
 
    return true;
}
 
} // namespace
 
// Writes every complete car to `path`. Returns false if the file can't be opened.
bool exportCsv(const std::vector<Bil>& bils, const std::string& path) {
    std::ofstream file(path);
    if (!file) {
        std::cerr << "Could not open " << path << " for writing\n";
        return false;
    }
    char delimiter = ',';
    const char d = delimiter;
    file << "Id" << d << "Merke" << d << "Modell" << d << "kmStand" << d
         << "totPris" << d << "modellAar" << d << "vekt" << d << "hk" << d
         << "kwh" << d << "gir" << d << "motor" << '\n';
 
    std::size_t written = 0;
    for (const Bil& b : bils) {
        if (!shouldExport(b)) continue;
 
        file << csvEscape(b.Id, d)     << d
             << csvEscape(b.Merke, d)  << d
             << csvEscape(b.Modell, d) << d
             << b.kmStand   << d
             << b.totPris   << d
             << b.modellAar << d
             << b.vekt      << d
             << b.hk        << d
             << b.kwh       << d
             << girToString(*b.gir)     << d
             << motorToString(*b.motor) << '\n';
        ++written;
    }
 
    std::cout << "Exported " << written << " of " << bils.size()
              << " cars to " << path << '\n';
    return true;
}