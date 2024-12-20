#include "utils.h"
#include <iomanip>
#include <numeric>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <chrono>

using namespace std;

double calculateMedian(std::vector<double>& grades) {
    size_t size = grades.size();
    if (size == 0) return 0.0;

    size_t mid = size / 2;

    std::nth_element(grades.begin(), grades.begin() + mid, grades.end());

    if (size % 2 == 0) {
        double midValue1 = grades[mid];
        std::nth_element(grades.begin(), grades.begin() + mid - 1, grades.end());
        double midValue2 = grades[mid - 1];
        return (midValue1 + midValue2) / 2.0;
    }

    return grades[mid];
}

double calculateFinalGrade(const Studentas& studentas, int pasirinkimas) {
    if (pasirinkimas == 1) {
        if (studentas.rezultatai.namuDarbuRezultatai.empty()) {
            return 0.0;
        }
        double namuDarbuVidurkis = std::accumulate(studentas.rezultatai.namuDarbuRezultatai.begin(), studentas.rezultatai.namuDarbuRezultatai.end(), 0.0) / studentas.rezultatai.namuDarbuRezultatai.size();
        return 0.4 * namuDarbuVidurkis + 0.6 * studentas.rezultatai.egzaminoRezultatas;
    }
    else {
        std::vector<double> namuDarbuCopy = studentas.rezultatai.namuDarbuRezultatai;
        double namuDarbuMediana = calculateMedian(namuDarbuCopy);
        return 0.4 * namuDarbuMediana + 0.6 * studentas.rezultatai.egzaminoRezultatas;
    }
}

bool compareByResultsAscending(const Studentas& a, const Studentas& b) {
    return calculateFinalGrade(a, 1) < calculateFinalGrade(b, 1);
}

bool compareByResultsDescending(const Studentas& a, const Studentas& b) {
    return calculateFinalGrade(a, 1) > calculateFinalGrade(b, 1);
}

void writeResultsToFile(std::vector<Studentas>& studentai, const std::string& filename, int pasirinkimas, int sortOption) {

    if (sortOption == 1) {
        std::sort(studentai.begin(), studentai.end(), [](const Studentas& a, const Studentas& b) {
            return a.finalGrade < b.finalGrade;
            });
    }
    else {
        std::sort(studentai.begin(), studentai.end(), [](const Studentas& a, const Studentas& b) {
            return a.finalGrade > b.finalGrade;
            });
    }

    std::ofstream outFile(filename);

    outFile << "\n" << std::setw(15) << std::left << "Vardas"
        << std::setw(15) << std::left << "Pavarde"
        << std::setw(20) << std::left << "Vieta atmintyje"
        << (pasirinkimas == 1 ? "Galutinis (Vid.)" : "Galutinis (Med.)") << std::endl;
    outFile << std::string(70, '-') << std::endl;

    for (const auto& studentas : studentai) {
        outFile << std::setw(15) << std::left << studentas.vardas
            << std::setw(15) << std::left << studentas.pavarde
            << std::setw(20) << std::left << &studentas
            << std::fixed << std::setprecision(2) << studentas.finalGrade << std::endl;
    }

    outFile.close();
}

void printOrSaveResults(std::vector<Studentas>& studentai, int pasirinkimas, int outputOption) {
    if (pasirinkimas == 1) {
        std::sort(studentai.begin(), studentai.end(), compareByResultsAscending);
    }
    else {
        std::sort(studentai.begin(), studentai.end(), compareByResultsDescending);
    }

    if (outputOption == 1 || outputOption == 3) {
        std::cout << "\n" << std::setw(15) << std::left << "Vardas"
            << std::setw(15) << std::left << "Pavarde"
            << std::setw(20) << std::left << "Vieta atmintyje"
            << (pasirinkimas == 1 ? "Galutinis (Vid.)" : "Galutinis (Med.)") << std::endl;
        std::cout << std::string(70, '-') << std::endl;

        for (const auto& studentas : studentai) {
            double galutinisVid = calculateFinalGrade(studentas, pasirinkimas);
            std::cout << std::setw(15) << std::left << studentas.vardas
                << std::setw(15) << std::left << studentas.pavarde
                << std::setw(20) << std::left << &studentas
                << std::fixed << std::setprecision(2) << galutinisVid << std::endl;
        }
    }
}

void createStudentFile(int studentCount, int gradeCount, const std::string& fileName) {
    using namespace std::chrono;

    auto start = high_resolution_clock::now();

    ofstream outFile(fileName);
    if (!outFile) {
        cout << "Nepavyko sukurti failo: " << fileName << endl;
        return;
    }

    outFile << left << setw(20) << "Vardas" << setw(20) << "Pavarde";

    for (int i = 1; i <= gradeCount; ++i) {
        outFile << setw(10) << "ND" + to_string(i);
    }
    outFile << setw(10) << "Egz." << endl;

    srand(static_cast<unsigned int>(time(0)));

    for (int i = 1; i <= studentCount; ++i) {
        outFile << left << setw(20) << ("Vardas" + to_string(i)) << setw(20) << ("Pavarde" + to_string(i));

        for (int j = 0; j < gradeCount; ++j) {
            outFile << setw(10) << (rand() % 10 + 1);
        }

        outFile << setw(10) << (rand() % 10 + 1);
        outFile << endl;
    }

    outFile.close();

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start).count();

    cout << "Failas sukurtas: " << fileName << endl;
    cout << "Failo kurimo laikas: " << duration << " ms" << endl;
}

void processStudentData(vector<Studentas>& studentai) {
    int gradeCalculationOption;
    do {
        cout << "Pasirinkite, kaip skaiciuoti galutini bala:\n";
        cout << "1 - Vidurkis\n";
        cout << "2 - Mediana\n";
        cout << "Iveskite pasirinkima: ";
        cin >> gradeCalculationOption;

        if (cin.fail() || (gradeCalculationOption < 1 || gradeCalculationOption > 2)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Neteisingas pasirinkimas. Bandykite dar karta.\n";
        }
        else {
            break;
        }
    } while (true);

    int sortOption;
    do {
        cout << "Pasirinkite rezultatu rikiavimo tvarka:\n";
        cout << "1 - Rikiuoti didejimo tvarka\n";
        cout << "2 - Rikiuoti mazejimo tvarka\n";
        cout << "Iveskite pasirinkima: ";
        cin >> sortOption;

        if (cin.fail() || (sortOption < 1 || sortOption > 2)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Neteisingas pasirinkimas. Bandykite dar karta.\n";
        }
        else {
            break;
        }
    } while (true);

    int outputOption;
    do {
        cout << "Pasirinkite isvedimo buda:\n";
        cout << "1 - Spausdinti i ekrana\n";
        cout << "2 - Issaugoti i failus\n";
        cout << "3 - Spausdinti i ekrana ir issaugoti i failus\n";
        cout << "Iveskite pasirinkima: ";
        cin >> outputOption;

        if (cin.fail() || (outputOption < 1 || outputOption > 3)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Neteisingas pasirinkimas. Bandykite dar karta.\n";
        }
        else {
            if (outputOption == 2 || outputOption == 3) {
                cout << "Rezultatai issaugoti failuose islaike.txt ir neislaike.txt.\n";
            }
            break;
        }
    } while (true);

    if (sortOption == 1) {
        sort(studentai.begin(), studentai.end(), compareByResultsDescending);
    }
    else {
        sort(studentai.begin(), studentai.end(), compareByResultsAscending);
    }
    printOrSaveResults(studentai, gradeCalculationOption, outputOption);
}
