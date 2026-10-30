
#include <iostream>
#include <vector>
#include <filesystem>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <map>
#include "Bil.hpp"
#include "trekant.hpp"
using std::filesystem::path;
namespace fs = std::filesystem;

std::string searchstring = R"("o"><div class="flex items-center"><w-icon name="Speedometer" size="large" locale="nb"></w-icon></div><div><span class="s-text-subtle">Kilometerstand</span><p class="m-0 font-bold">191 200 km</p></div></div><div class="flex gap-16 hyphens-auto"><div class="flex items-center"><w-icon name="GearAutomatic" size="large" locale="nb"></w-icon></div><div><span class="s-text-subtle">Girkasse</span><p class="m-0 font-bold">Automat</p></div></div><div class="flex gap-16 hyphens-auto"><div class="flex items-center"><w-icon name="GasDiesel" size="large" locale="nb"></w-icon></div><div><span class="s-text-subtle">Drivstoff</span><p class="m-0 font-bold">Bensin</p></div></div></div><div class="border-t pt-40 mt-40"><div class="flex flex-col"><p class="s-text-subtle mb-0">Totalpris</p><h2 class="mb-0 mt-8"><span class="t2">119 900 kr</span></h2></div></div><div class=""></div></div><div class="md:col-span-1 md:row-span-2"><div class="flex flex-col gap-16 pt-8"><w-button role="button" tabIndex="0" variant="primary" data-testid="contact" class="whitespace-nowrap md:min-w-[170px]" full-width="">Skriv til selger</w-button><div class="w-full"><w-button role="button" tabIndex="0" variant="secondary" data-testid="phone-number" class="whitespace-nowrap md:min-w-[170px]" full-width=""><span class="flex items-center">Se telefonnummer</span></w-button></div></div><w-box bordered="" class="my-16"><div class="mb-16"><h2 class="sr-only">Selgerens infokort</h2><h3 class="mb-4">MK Auto</h3><a class="flex items-center gap-4" target="_blank" rel="noopener noreferrer" href=";)";
std::string readFile(const fs::path& path);
std::map<std::string, std::string> readFolder(const fs::path& folder);

int main(int argc, char** argv){

    Bil c =  Bil("", readFile("./htmls/479217966"));
    std::cout << c.kmStand << std::endl;
    c.printOut();
    std::vector<Bil> bilListe;
    auto filer = readFolder("./htmls");
    for(const auto& [id, streng] : filer){
        auto kjerre = Bil(id,streng);
        bilListe.push_back(kjerre);
        kjerre.printOut();
        
    }


    std::map<std::string, int> tellar;
    for(auto kjerre : bilListe){
        tellar[kjerre.Merke]++;
    }
    
    for(const auto& [key, value] : tellar){
        std::cout << key << "\t" << value <<std::endl;
    }
    exportCsv(bilListe, "bilar.csv");
}

std::string readFile(const fs::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::cerr << "Could not open " << path << '\n';
        return "";
    }
    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::map<std::string, std::string> readFolder(const fs::path& folder) {
    std::map<std::string, std::string> files;
 
    if (!fs::is_directory(folder)) {
        std::cerr << "Not a directory: " << folder << '\n';
        return files;
    }
 
    for (const auto& entry : fs::directory_iterator(folder)) {
        if (!entry.is_regular_file()) continue;
        files[entry.path().filename().string()] = readFile(entry.path());
    }
    return files;
}

