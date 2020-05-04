#ifndef DoseVector_H
#define DoseVector_H


#include <vector>
#include <QString>

class DoseVector
{
public:
    DoseVector();
    DoseVector(QString fileName, QString fileFormat);
   ~DoseVector();
    DoseVector& operator=(const DoseVector &other);

    double getDoseMaxValue();
    double getXSize();
    double getYSize();
    double getZSize();

    double element(int X, int Y, int Z);
    bool wrongFileFormat = false;

    typedef std::vector<double> DVector;

private:
    DVector doseV;
    int sizeX = 0, sizeY = 0, sizeZ = 0;
    int index(int X,int Y,int Z);

    double DoseMaximum;

    void getSizeFromSRNAFile(QString fileName);
    void getSizeFromGEANT4File(QString fileName);

    void loadMatrixFromSRNAFile(QString fileName);
    void loadMatrixFromGEANT4File(QString fileName);





};

#endif // DoseVector_H
