#include "DoseVector.h"
#include <QString>
#include <QFile>
#include <QTextStream>

DoseVector::DoseVector(){
    this->sizeX = 0;
    this->sizeY = 0;
    this->sizeZ = 0;
    this->doseV.push_back(0);
}
DoseVector::DoseVector(QString fileName, QString fileFormat)
{
    //location.resize(3);
    if (fileFormat == "SRNA")
    {
        getSizeFromSRNAFile(fileName);
        if (wrongFileFormat) return;
        //initNewMap();
        //doseV.resize(sizeX*sizeY*sizeZ);
        doseV = DVector(sizeX*sizeY*sizeZ,0);

        loadMatrixFromSRNAFile(fileName);
    }
    else if(fileFormat == "GEANT4")
    {
        getSizeFromGEANT4File(fileName);
        if (wrongFileFormat) return;
        //initNewMap();
        //doseV.resize(sizeX*sizeY*sizeZ);
        doseV = DVector(sizeX*sizeY*sizeZ,0);
        loadMatrixFromGEANT4File(fileName);
    }
}

DoseVector::~DoseVector(){
}


DoseVector& DoseVector::operator=(const DoseVector &other){
    this->sizeX = other.sizeX;
    this->sizeY = other.sizeY;
    this->sizeZ = other.sizeZ;
    this->doseMaxValue = other.doseMaxValue;
    this->doseV = other.doseV;
    this->maxValPosX = other.maxValPosX;
    this->maxValPosY = other.maxValPosY;
    this->maxValPosZ = other.maxValPosZ;

    return *this;
}

int DoseVector::index(int X,int Y,int Z)
{
    return X + Y * sizeX + Z * sizeX * sizeY;
}

void DoseVector::getSizeFromSRNAFile(QString fileName){

    setlocale(LC_ALL, "Russian");
    QFile newFile(fileName);
    newFile.open(QIODevice::ReadOnly);
    QTextStream newData(&newFile);

    newData >> sizeZ >> sizeX >> sizeY;

    newFile.close();
    if (( sizeX == 0 )&&( sizeY == 0 )&&( sizeZ == 0 )){
        wrongFileFormat = true;
    }
}

void DoseVector::getSizeFromGEANT4File(QString FileName)
{
    setlocale(LC_ALL, "Russian");

    QFile newFile(FileName);
    newFile.open(QIODevice::ReadOnly);
    QTextStream newData(&newFile);

    QString dummy;
    newData >> dummy >> dummy >> sizeZ;
    newData >> dummy >> dummy >> sizeX;
    newData >> dummy >> dummy >> sizeY;

    newFile.close();
    if ((this->sizeX == 0)&&(this->sizeY == 0)&&(this->sizeZ == 0)){
        wrongFileFormat = true;
    }
}

void DoseVector::loadMatrixFromSRNAFile(QString fileName){
    setlocale(LC_ALL, "Russian");
    QFile newFile(fileName);
    newFile.open(QIODevice::ReadOnly);
    QTextStream newData(&newFile);

    int dummy;
    newData >> dummy >> dummy >> dummy ;

    double Dose, Maximum=0;
    int indx;
    // Считывание массива
    for (int k=0; k < sizeZ; k++){
        for (int j=0; j < sizeY; j++){
            for (int i=0; i < sizeX; i++){
                newData >> Dose;
                indx=index(i,j,k);
                doseV[indx] = Dose;
                if (Dose > Maximum){
                    Maximum = Dose;
                    //maxValPos = (i,j,k);
                    maxValPosX = i;
                    maxValPosY = j;
                    maxValPosZ = k;
                }
            }
        }
    }
    doseMaxValue=Maximum;
    newFile.close();
}

void DoseVector::loadMatrixFromGEANT4File(QString fileName){
    setlocale(LC_ALL, "Russian");

    QFile newFile(fileName);
    newFile.open(QIODevice::ReadOnly);
    QTextStream newData(&newFile);

    QString dummy;
    int sizeZ, sizeX, sizeY;
    newData >> dummy >> dummy >> sizeZ;
    newData >> dummy >> dummy >> sizeX;
    newData >> dummy >> dummy >> sizeY;
    //          i         j        k        Dose(Gy)
    newData >> dummy >> dummy >> dummy >> dummy;

    int x, y, z, indx;
    double Dose, Maximum=0;
    //int location[3]{};
    //std::map<int[3], double>::iterator it;

    while(!newFile.atEnd()) {
        newData >> z;
        newData >> x;
        newData >> y;
        newData >> Dose;
        if(newData.status() != QTextStream::Ok) break; // check status before using the data

        indx=index(x,y,z);
        doseV[indx] = Dose;
        if (Dose > Maximum){
            Maximum = Dose;
            //maxValPos = std::make_tuple (x,y,z);
            maxValPosX = x;
            maxValPosY = y;
            maxValPosZ = z;
        }
    }
    doseMaxValue=Maximum;

    newFile.close();
}

double DoseVector::getDoseMaxValue(){
    return doseMaxValue;
}

double DoseVector::getXSize(){
    return sizeX;
}

double DoseVector::getYSize(){
    return sizeY;
}

double DoseVector::getZSize(){
    return sizeZ;
}

double DoseVector::element(int X, int Y, int Z){
    int indx=index(X,Y,Z);
    return doseV[indx];
}

std::tuple<int,int,int> DoseVector::getMaxValPosition()
{
    std::tuple<int,int,int> maxValPos = std::make_tuple(maxValPosX,maxValPosY,maxValPosZ);
    return maxValPos;
}
