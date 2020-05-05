#ifndef DoseVector_H
#define DoseVector_H


#include <vector>
#include <QString>
#include <tuple>

class DoseVector
{
public:
    DoseVector();
    DoseVector(QString fileName, QString fileFormat);
   ~DoseVector();
    DoseVector& operator=(const DoseVector &other);

    double getDoseMaxValue();
    std::tuple<int,int,int> getMaxValPosition();

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

    double doseMaxValue;
    //std::tuple<int,int,int> maxValPos;
    int maxValPosX = 0, maxValPosY = 0, maxValPosZ = 0;

    void getSizeFromSRNAFile(QString fileName);
    void getSizeFromGEANT4File(QString fileName);

    void loadMatrixFromSRNAFile(QString fileName);
    void loadMatrixFromGEANT4File(QString fileName);





};

#endif // DoseVector_H
