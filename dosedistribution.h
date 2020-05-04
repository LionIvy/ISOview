#ifndef DOSEDISTRIBUTION_H
#define DOSEDISTRIBUTION_H

//#include "matrix3d.h"
#include <string>

#include <QString>

class DoseDistribution
{
public:
    DoseDistribution();
    DoseDistribution(std::string FileName);
    DoseDistribution(QString FileName, QString FileFormat);
    ~DoseDistribution();

    double element(int X, int Y, int Z);
    void setElement(int X, int Y, int Z, double Val);
    void setWholeMatrix(int sizeX,int sizeY,int sizeZ, double*** matrix);

    int getXSize();
    int getYSize();
    int getZSize();
    double getDoseMaxValue();

    DoseDistribution& operator=(const DoseDistribution& other);





private:
    int sizeX, sizeY, sizeZ;  // Размерность матрицы
    double*** matrix = nullptr;
    double doseMaxVal=1.e-8;

    void initMatrix(int sizeX,int sizeY,int sizeZ);
    void destroyMatrix();

    void getSizeFromFile(std::string FileName);
    void getSizeFromFile(QString FileName);
    void loadDoseFile(std::string FileName);
    void loadDoseFile(QString FileName);

    void getSizeFromGEANT4File(QString FileName);
    void loadGEANT4DoseFile(QString FileName);

    void findAndSetDoseMaximum();

};

#endif // DOSEDISTRIBUTION_H
