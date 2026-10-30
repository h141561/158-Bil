
#include "Bil.hpp"


bool DEBUG =  true;

int minKmstand = 2;
int maksKmstand = 1000000;

int minPris = 2;
int maksPris = 10000000;

int minstModellaar = 1985;
std::string totalPrisStreng = R"(Totalpris</p><h2 class="mb-0 mt-8"><span class="t2">)";
std::string modellaarStreng = R"(Modellår</dt><dd class="t4 m-0">)";
std::string kmstandStreng = R"(Kilometerstand</span><p class="m-0 font-bold">)";
std::string drivstoffStreng = R"(Drivstoff</dt><dd class="t4 m-0">)";
std::string merkeStreng = R"(Merke</dt><dd class="t4 m-0">)";
std::string girStreng = R"(Girkasse</dt><dd class="t4 m-0">)";
std::string vektStreng = R"(Vekt</dt><dd class="t4 m-0">)";
std::string modellStreng = R"(Modell</dt><dd class="t4 m-0">)";
std::string seteStreng = R"(Seter</dt><dd class="t4 m-0">)";
std::string doerStreng = R"(Dører</dt><dd class="t4 m-0")";
std::string effektStreng = R"(Effekt</dt><dd class="t4 m-0">)";
std::string batteriKapasitet = R"(Batterikapasitet</dt><dd class="t4 m-0">)";

std::string eksStreng = R"(Pris ekskl. omreg.</dt><dd class="t4 m-0">)";
std::string omregStreng = R"(Omregistrering</dt><dd class="t4 m-0">)";
std::string trimmedString(std::string input, std::string search_string);

Bil::Bil(){    
}
Bil::Bil(std::string ID, std::string streng ){
    if(false){
        std::cout << "merke " << trimmedString(streng, merkeStreng) <<std::endl;
        std::cout << "modell " << trimmedString(streng, modellStreng) << std::endl;
        std::cout << "modellår " << trimmedString(streng, modellaarStreng) << std::endl;
        std::cout << "kmstand " << trimmedString(streng, kmstandStreng) << std::endl;
        std::cout << "bilpris " << trimmedString(streng, totalPrisStreng) <<std::endl;
        std::cout << "drivstoff " << trimmedString(streng, drivstoffStreng) << std::endl;
        std::cout << "girkasse " << trimmedString(streng, girStreng)<< std::endl;
        std::cout << "vekt " << trimmedString(streng, vektStreng) << std::endl;
    }
    
    this->Merke = trimmedString(streng, merkeStreng);
    this->Modell = trimmedString(streng, modellStreng);
    this->Id = std::move(ID);
    
    this->kmStand = stoiIgnore(trimmedString(streng, kmstandStreng));
    this->totPris = stoiIgnore(trimmedString(streng, eksStreng)) + stoiIgnore(trimmedString(streng, omregStreng));
    this->modellAar = stoiIgnore(trimmedString(streng, modellaarStreng));
    this->vekt = stoiIgnore(trimmedString(streng, vektStreng));
    this->hk = stoiIgnore(trimmedString(streng, effektStreng ));
    this->kwh = stoiIgnore(trimmedString(streng, batteriKapasitet));
    this->seter = stoiIgnore(trimmedString(streng, seteStreng));
    this->gir = girFromString(trimmedString(streng, girStreng));
    this->motor = motorFromString(trimmedString(streng, drivstoffStreng));

}

std::string trimmedString(std::string input, std::string search_string){
    const std::size_t found = input.find(search_string);
    if (found == std::string::npos) return "";
 
    const std::size_t start = found + search_string.size();
    std::size_t end = input.find('<', start);
    if (end == std::string::npos) end = input.size();
 
    return input.substr(start, end - start);
}

int stoiIgnore(const std::string& input) {
    int result = 0;
    for (unsigned char c : input) {
        if (std::isdigit(c)) {
            result = result * 10 + (c - '0');
        }
    }
    return result;
}

std::string orNA(const std::string& s) {
    return s.empty() ? "n/a" : s;
}
 
std::string orNA(int value) {
    return value < 0 ? "n/a" : std::to_string(value);
}
 
std::string orNA(const std::optional<girType>& g) {
    if (!g) return "n/a";
    switch (*g) {
        case AUTOMAT: return "automat";
        case MANUELL: return "manuell";
    }
    return "n/a";
}
 
std::string orNA(const std::optional<motorType>& m) {
    if (!m) return "n/a";
    switch (*m) {
        case DIESEL: return "diesel";
        case BENSIN: return "bensin";
        case EL:     return "el";
        case HYBRID : return "hybrid";
    }
    return "n/a";
}
 
 
void Bil::printOut() const {
    std::cout << "Merke:     " << orNA(Merke)     << '\n'
              << "Modell:    " << orNA(Modell)    << '\n'
              << "Id:        " << orNA(Id)        << '\n'
              << "kmStand:   " << orNA(kmStand)   << '\n'
              << "totPris:   " << orNA(totPris)   << '\n'
              << "modellAar: " << orNA(modellAar) << '\n'
              << "vekt:      " << orNA(vekt)      << '\n'
              << "hk:        " << orNA(hk)        << '\n'
              << "kwh:       " << orNA(kwh)       << '\n'
              << "gir:       " << orNA(gir)   << '\n'
              << "motor:     " << orNA(motor) << '\n';
}
std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return s;
}

// Case-insensitive. Returns std::nullopt if the string isn't recognised.
std::optional<girType> girFromString(const std::string& s) {
    const std::string v = toLower(s);
    if (v == "automat" || v == "automatisk") return AUTOMAT;
    if (v == "manuell" || v == "manual")     return MANUELL;
    std::cout << "girFromString forstod ikkje " << v << std::endl;
    return std::nullopt;
}
 
std::optional<motorType> motorFromString(const std::string& s) {
    const std::string v = toLower(s);
    if (v == "diesel")                      return DIESEL;
    if (v == "bensin")                      return BENSIN;
    if (v == "el" || v == "elektrisk")      return EL;
    if (v == "Plug-in hybrid" ||
        v == "plug-in bensin" ||
        v == "plug-in diesel" ||
        v == "hybrid bensin" ||
        v == "hybrid diesel"
    
        )              return HYBRID;
    
    std::cout << "motorFromString forstod ikkje : " << v  << std::endl;
    return std::nullopt;
}