#ifndef BIL_H
#define BIL_H

#include <iostream>
#include <string>
#include <cstdio>
#include <cctype>
#include <optional>
#include <algorithm>
#include <vector>
std::string toLower(std::string s);


int stoiIgnore(const std::string& input);
enum girType{
    AUTOMAT,
    MANUELL,
};

enum motorType{
    DIESEL,
    BENSIN,
    EL,
    HYBRID


};

std::optional<girType> girFromString(const std::string& s);
std::optional<motorType> motorFromString(const std::string& s);

class Bil{
    public:
    
    std::string Merke;
    std::string Modell;
    std::string Id;
    int kmStand;
    int totPris;
    int modellAar;
    int vekt;
    int hk;
    int kwh;
    int seter;
    std::optional<girType> gir;
    std::optional<motorType> motor;
    Bil(std::string ID,std::string streng);
    Bil();
    void printOut() const;
};

bool exportCsv(const std::vector<Bil>& bils, const std::string& path) ;



#endif
