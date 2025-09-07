#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <algorithm>

struct StudentFull {
    int id;
    int group;
    char lastName[30];
    char firstName[30];
    char patronymic[30];
    int math;
    int geo;
    int prog;
    double average;
};

void calculateAverage(const std::string&, const std::string&);
void mergeStudentGrades(const std::string&, const std::string&, const std::string&);
void createBinaryGrades(const std::string&, const std::string&);
void createBinaryStudents(const std::string&, const std::string&);
void createFailingList(const std::string&, const std::string&);
bool compare(const StudentFull&, const StudentFull&);
void sortFailingList(const std::string&);
void printFailingList(const std::string&);
void printGroupSortedBySurnames(const std::string&, int);
void printGroupSortedByAverage(const std::string&, int);
void createGoodList(const std::string&, const std::string&);
void binaryToText(const std::string& binaryFile, const std::string& textFile);

int main() {
    // a) Create binary files from text files
    createBinaryStudents("students.txt", "students.bin");
    createBinaryGrades("grades.txt", "grades.bin");

    // b) Merge student names with grades
    mergeStudentGrades("students.bin", "grades.bin", "merged.bin");

    // c) Calculate average grades
    calculateAverage("merged.bin", "averaged.bin");
    
    // d) Create list of failing students
    createFailingList("averaged.bin", "failing.bin");
    // f) Print failing list before sorting
    std::cout << "\n--- Failing Students Before Sorting ---\n";
    printFailingList("failing.bin");

    // e) Sort failing list
    sortFailingList("failing.bin");
    binaryToText("failing.bin", "failing.txt");

    // f) Print failing list after sorting
    std::cout << "\n--- Failing Students After Sorting ---\n";
    printFailingList("failing.bin");
    // g) Print grade sheet for a specific group sorted by surnames
    int targetGroup = 2; // Example group number
    printGroupSortedBySurnames("averaged.bin", targetGroup);
    binaryToText("averaged.bin", "averaged.txt");

    // h) Print grade sheet for a specific group sorted by average descending

    printGroupSortedByAverage("averaged.bin" ,targetGroup);
    // i) Create list of excellent students
    createGoodList("averaged.bin", "excellent.bin");
    binaryToText("excellent.bin", "excellent.txt");
    std::cout << "\n--- Program completed successfully ---\n";
    return 0;
}

void createBinaryStudents(const std::string& inputFile, const std::string& outputFile) {
    std::ifstream in(inputFile);
    std::ofstream out(outputFile, std::ios::binary);

    std::string line;
    while (std::getline(in, line)) {
        std::stringstream ss(line);
        std::string inlexStr, lastNameStr, firstNameStr, patronymicStr;

        if (std::getline(ss, inlexStr, ';') &&
            std::getline(ss, lastNameStr, ';') &&
            std::getline(ss, firstNameStr, ';') &&
            std::getline(ss, patronymicStr, ';')) {

            StudentFull s{};


            s.id = stoi(inlexStr);

            std::strncpy(s.lastName, lastNameStr.c_str(), sizeof(s.lastName));

            std::strncpy(s.firstName, firstNameStr.c_str(), sizeof(s.firstName));

            std::strncpy(s.patronymic, patronymicStr.c_str(), sizeof(s.patronymic));

            s.group = 0;
            s.math = s.geo = s.prog = 0;
            s.average = 0.0;

            out.write(reinterpret_cast<char*>(&s), sizeof(StudentFull));
        }
    }

    in.close();
    out.close();
}

void createBinaryGrades(const std::string& inputFile, const std::string& outputFile) {
    std::ifstream in(inputFile);
    std::ofstream out(outputFile, std::ios::binary);
    std::string line;
    while (std::getline(in, line)) {
        std::stringstream ss(line);
        std::string group, index;
        std::string subj1, grade1, subj2, grade2, subj3, grade3;

        if (std::getline(ss, group, ';') &&
            std::getline(ss, index, ';') &&
            std::getline(ss, subj1, ';') &&
            std::getline(ss, grade1, ';') &&
            std::getline(ss, subj2, ';') &&
            std::getline(ss, grade2, ';') &&
            std::getline(ss, subj3, ';') &&
            std::getline(ss, grade3, ';')) {

            StudentFull s{};
            s.group = stoi(group);
            s.id = stoi(index);
            s.math = stoi(grade1);
            s.geo = stoi(grade2);
            s.prog = stoi(grade3);

            out.write(reinterpret_cast<char*>(&s), sizeof(StudentFull));
        }
    }

    in.close();
    out.close();
}

void mergeStudentGrades(const std::string& studentsBin, const std::string& gradesBin, const std::string& outputBin) {

    std::ifstream students(studentsBin, std::ios::binary);
    std::ifstream grades(gradesBin, std::ios::binary);
    std::ofstream out(outputBin, std::ios::binary);

    StudentFull grades_s;
    while (grades.read(reinterpret_cast<char*>(&grades_s), sizeof(StudentFull))) {

        students.clear();
        students.seekg(0);

        StudentFull students_s{};

        while (students.read(reinterpret_cast<char*>(&students_s), sizeof(StudentFull))) {
            if (students_s.id == grades_s.id) {

                StudentFull out_s{};

                out_s.id = grades_s.id;
                out_s.group = grades_s.group;
                out_s.math = grades_s.math;
                out_s.geo = grades_s.geo;
                out_s.prog = grades_s.prog;
                strncpy(out_s.lastName, students_s.lastName, sizeof(out_s.lastName));
                out_s.average = 0.0;


                out.write(reinterpret_cast<char*>(&out_s), sizeof(StudentFull));

                break;
            }
        }
    }
}

void calculateAverage(const std::string& inputBin, const std::string& outputBin) {
    std::ifstream in(inputBin, std::ios::binary);
    std::ofstream out(outputBin, std::ios::binary);

    StudentFull s{};
    while (in.read(reinterpret_cast<char*>(&s), sizeof(StudentFull))) {

        StudentFull avg{};

        avg.id = s.id;
        avg.group = s.group;
        strncpy(avg.lastName, s.lastName, sizeof(avg.lastName));

        avg.math = s.math;
        avg.geo = s.geo;
        avg.prog = s.prog;
        avg.average = (s.math + s.geo + s.prog) / 3.0;
        out.write(reinterpret_cast<char*>(&avg), sizeof(StudentFull));
    }
}

void createFailingList(const std::string& averageBin, const std::string& failingBin) {
    std::ifstream in(averageBin, std::ios::binary);
    std::ofstream out(failingBin, std::ios::binary);

    StudentFull s{};

    while (in.read(reinterpret_cast<char*>(&s), sizeof(StudentFull))) {
        if (s.average < 4.0) {

            StudentFull fs{};

            fs.id = s.id;
            fs.group = s.group;
            strncpy(fs.lastName, s.lastName, sizeof(fs.lastName));
            fs.average = s.average;
            fs.math = s.math;
            fs.geo = s.geo;
            fs.prog = s.prog;

            out.write(reinterpret_cast<char*>(&fs), sizeof(StudentFull));
        }
    }
}

bool compareGroupAndSurnames(const StudentFull& a, const StudentFull& b) {

    if (a.group != b.group)
        return a.group < b.group;

    return std::strcmp(a.lastName, b.lastName) < 0;
}


bool compareSurnames(const StudentFull& a, const StudentFull& b) {

    return std::strcmp(a.lastName, b.lastName) < 0;
}

bool compareAverage(const StudentFull& a, const StudentFull& b) {

    return a.average > b.average;
}


template<class T>
void copyArray(T* arr, T*& newArr, int oldSize) {
    newArr = new T[oldSize + 1];
    for (int i = 0; i < oldSize; ++i) {
        newArr[i] = arr[i];
    }
}

template<class T>
void addElementToArray(T*& arr, int& size, T element) {
    T* tempArr = nullptr;
    copyArray(arr, tempArr, size);
    tempArr[size] = element;
    delete[] arr;
    arr = tempArr;
    ++size;
}

void sortFailingList(const std::string& failingBin) {
    std::ifstream in(failingBin, std::ios::binary);
    StudentFull s{};
    int size{};

    StudentFull* arr = nullptr;

    while(in.read(reinterpret_cast<char*>(&s), sizeof(StudentFull))) {
        addElementToArray(arr, size, s);
    }

    std::sort(arr, arr + size, compareGroupAndSurnames);

    std::ofstream out(failingBin, std::ios::binary | std::ios::trunc);


    for (int i = 0; i < size; ++i) {
        out.write(reinterpret_cast<char*>(&arr[i]), sizeof(StudentFull));
    }

    delete[] arr;
}

void printFailingList(const std::string& failingBin) {
    std::ifstream in(failingBin, std::ios::binary);
    StudentFull fs{};

    while (in.read(reinterpret_cast<char*>(&fs), sizeof(StudentFull))) {
        std::cout << "Last Name: " << fs.lastName
            << ", Group: " << fs.group
            << ", ID: " << fs.id
            << ", Average Grade: " << std::fixed << std::setprecision(4) << fs.average
            << std::endl;
    }
}


void printGroupSortedBySurnames(const std::string& mergedBin, int targetGroup) {
    std::ifstream in(mergedBin, std::ios::binary);

    StudentFull* arr = nullptr;
    int size{};
    StudentFull s{};


    while (in.read(reinterpret_cast<char*>(&s), sizeof(StudentFull))) {
        if (s.group == targetGroup) {
            addElementToArray(arr, size, s);
        }
    }

    in.close();

    std::sort(arr, arr + size, compareSurnames);

    std::ofstream out(mergedBin, std::ios::binary);

    for (int i = 0; i < size; ++i) {
        out.write(reinterpret_cast<char*>(&arr[i]), sizeof(StudentFull));
    }

    out.close();

    delete[] arr;
}

void printGroupSortedByAverage(const std::string& mergedBin, int targetGroup) {
    std::ifstream in(mergedBin, std::ios::binary);

    StudentFull* arr = nullptr;
    int size{};
    StudentFull s{};


    while (in.read(reinterpret_cast<char*>(&s), sizeof(StudentFull))) {
        if (s.group == targetGroup) {
            addElementToArray(arr, size, s);
        }
    }

    in.close();

    std::sort(arr, arr + size, compareAverage);

    std::ofstream out(mergedBin, std::ios::binary);

    for (int i = 0; i < size; ++i) {
        out.write(reinterpret_cast<char*>(&arr[i]), sizeof(StudentFull));
    }

    out.close();

    delete[] arr;
}

void createGoodList(const std::string& inBin, const std::string& outBin) {
    std::ifstream in(inBin, std::ios::binary);
    std::ofstream out(outBin, std::ios::binary);

    StudentFull s{};
    while (in.read(reinterpret_cast<char*>(&s), sizeof(StudentFull))) {
        if (s.average >= 8.0) {

            StudentFull gs{};

            strncpy(gs.lastName, s.lastName, sizeof(gs.lastName));
            gs.id = s.id;
            gs.group = s.group;
            gs.average = 0.0;
            gs.math = 0;
            gs.geo = 0;
            gs.prog = 0;


            out.write(reinterpret_cast<char*>(&gs), sizeof(StudentFull));
        }
    }
}

void binaryToText(const std::string& binaryFile, const std::string& textFile) {
    std::ifstream in(binaryFile, std::ios::binary);

    StudentFull* arr = nullptr;
    int size{};

    StudentFull s{};


    while (in.read(reinterpret_cast<char*>(&s), sizeof(StudentFull))) {
        addElementToArray(arr, size, s);
    }


    std::sort(arr, arr + size, compareGroupAndSurnames);


    std::ofstream outBin(binaryFile, std::ios::binary);

    for (int i = 0; i < size; ++i) {
        outBin.write(reinterpret_cast<char*>(&arr[i]), sizeof(StudentFull));
    }

    outBin.close();

    in.clear();
    in.seekg(0);

    std::ofstream out(textFile);
    
    while (in.read(reinterpret_cast<char*>(&s), sizeof(s))) {
        out << s.id << ";"
            << s.group << ";"
            << s.lastName << ";"
            << s.firstName << ";"
            << s.patronymic << ";"
            << s.math << ";"
            << s.geo << ";"
            << s.prog << ";"
            << s.average << "\n";
    }

    in.close();
    out.close();
}