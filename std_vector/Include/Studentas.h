#ifndef STUDENTAS_H
#define STUDENTAS_H

#include <string>
#include <vector>
#include "Rezultatai.h"

struct Studentas {
    std::string vardas;
    std::string pavarde;
    Rezultatai rezultatai;
    double finalGrade;
};

void correctInvalidData(int& rezultatas, const std::string& fieldName);
void skaitytiIsFailo(std::vector<Studentas>& studentai, const std::string& fileName);

#endif // STUDENTAS_H
