#include "dosedistribution.h"
#include <iostream>
#include <string>
#include <istream>
#include <fstream>

#include <QString>
#include <QFile>
#include <QTextStream>

DoseDistribution::DoseDistribution(){
    sizeX = sizeY = sizeZ = 0;  // Размерность матрицы
    matrix = nullptr;
    doseMaxVal=0;
}

DoseDistribution::DoseDistribution(std::string FileName)
{    
    getSizeFromFile(FileName);
    initMatrix(this->sizeX,this->sizeY,this->sizeZ);
    loadDoseFile(FileName);
    //findAndSetDoseMaximum();
}
DoseDistribution::DoseDistribution(QString FileName, QString FileFormat)
{
    if (FileFormat == "SRNA")
    {
        getSizeFromFile(FileName);
        initMatrix(this->sizeX,this->sizeY,this->sizeZ);
        loadDoseFile(FileName);
    }
    else if(FileFormat == "GEANT4")
    {
        getSizeFromGEANT4File(FileName);
        initMatrix(this->sizeX,this->sizeY,this->sizeZ);
        loadGEANT4DoseFile(FileName);
    }
    //findAndSetDoseMaximum();
}

DoseDistribution& DoseDistribution::operator=(const DoseDistribution& other){
    this->sizeX = other.sizeX;
    this->sizeY = other.sizeY;
    this->sizeZ = other.sizeZ;
    this->doseMaxVal = other.doseMaxVal;

    if (matrix!=nullptr) destroyMatrix();

    initMatrix(other.sizeX,other.sizeY,other.sizeZ);
    for (int i=0; i < this->sizeX; i++)
    {
        for (int j=0; j < this->sizeY; j++)
        {
            for (int k=0; k < this->sizeZ; k++)
            {
                this->matrix[i][j][k]= other.matrix[i][j][k];
            }
        }
    }
    return *this;
}


DoseDistribution::~DoseDistribution()
{
    std::cout << "DoseDistribution destroyed\n";
    if (matrix!=nullptr)
    destroyMatrix();
}

void DoseDistribution::initMatrix(int sizeX,int sizeY,int sizeZ)
{
    this->sizeX = sizeX;
    this->sizeY = sizeY;
    this->sizeZ = sizeZ;

    this->matrix = new double**[this->sizeX]{};
    for (int i=0; i < this->sizeX; i++)
    {
        this->matrix[i]= new double*[this->sizeY]{};
        for (int j=0; j < this->sizeY; j++)
        {
            this->matrix[i][j]= new double[this->sizeZ]{};
        }
    }
}

void DoseDistribution::destroyMatrix()
{
    for (int i = 0; i< this->sizeX ; i++)
    {
        for (int j = 0; j< this->sizeY ; j++)
        {
           delete [] this->matrix[i][j];
        }
        delete [] this->matrix[i];
    }

    delete [] this->matrix;
}

double DoseDistribution::element(int X, int Y, int Z)
{
    if((X<this->sizeX)&&(Y<this->sizeY)&&(Z<this->sizeZ))
    {
      return this->matrix[X][Y][Z];
    }
    else
    {
        std::cout << "Wrong matrix index\n";
        return 0.0;
    }
}

void DoseDistribution::setElement(int X, int Y, int Z, double Val)
{
    if((X<this->sizeX)&&(Y<this->sizeY)&&(Z<this->sizeZ))
    {
      this->matrix[X][Y][Z] = Val;
    }
    else
    {
        std::cout << "Wrong matrix index\n";
    }

}

void DoseDistribution::setWholeMatrix(int sizeX,int sizeY,int sizeZ, double*** matrix)
{

    if ((this->sizeX != sizeX) || (this->sizeY != sizeY) || (this->sizeZ != sizeZ))
    {
        destroyMatrix();
        initMatrix(sizeX,sizeY,sizeZ);
    }

    for (int i=0; i < this->sizeX; i++)
    {
        for (int j=0; j < this->sizeY; j++)
        {
            for (int k=0; k < this->sizeZ; k++)
            {
                this->matrix[i][j][k]= matrix[i][j][k];
            }
        }
    }
}


void DoseDistribution::getSizeFromFile(std::string FileName)
{
    setlocale(LC_ALL, "Russian");

    std::ifstream newData;
    newData.open(FileName);
    newData >> this->sizeZ >> this->sizeX >> this->sizeY;
    newData.close();
}

void DoseDistribution::getSizeFromFile(QString FileName)
{
    setlocale(LC_ALL, "Russian");
    QFile newFile(FileName);
    newFile.open(QIODevice::ReadOnly);
    QTextStream newData(&newFile);

    newData >> this->sizeZ >> this->sizeX >> this->sizeY;

    newFile.close();


}

void DoseDistribution::loadDoseFile(std::string FileName)
{
    setlocale(LC_ALL, "Russian");
    std::ifstream newData;
    int dummy;

    newData.open(FileName);
    newData >> dummy >> dummy >> dummy ;

    double  Maximum=0;//val,
     // Считывание массива
    for (int k=0; k < sizeZ; k++)
    {
        for (int j=0; j < sizeY; j++)
        {
            for (int i=0; i < sizeX; i++)
            {
//                newData >> val;
//                this->matrix[i][j][k] = val;
                newData >>this->matrix[i][j][k];
                if (this->matrix[i][j][k] > Maximum)
                {
                    Maximum=this->matrix[i][j][k];
                }
            }
        }
    }
    this->doseMaxVal=Maximum;
    newData.close();
}

void DoseDistribution::loadDoseFile(QString FileName)
{
    setlocale(LC_ALL, "Russian");

    QFile newFile(FileName);
    newFile.open(QIODevice::ReadOnly);
    QTextStream newData(&newFile);

    int dummy;
    newData >> dummy >> dummy >> dummy ;

    double  Maximum=0;//val,
     // Считывание массива
    for (int k=0; k < sizeZ; k++)
    {
        for (int j=0; j < sizeY; j++)
        {
            for (int i=0; i < sizeX; i++)
            {
               // newData >> val;
               // this->matrix[i][j][k] = val;
                newData >>this->matrix[i][j][k];
                if (this->matrix[i][j][k] > Maximum)
                {
                    Maximum=this->matrix[i][j][k];
                }
            }
        }
    }
    this->doseMaxVal=Maximum;


    newFile.close();
}

void DoseDistribution::getSizeFromGEANT4File(QString FileName)
{
    setlocale(LC_ALL, "Russian");

    QFile newFile(FileName);
    newFile.open(QIODevice::ReadOnly);
    QTextStream newData(&newFile);

    QString dummy;
    int sizeZ, sizeX, sizeY;
    newData >> dummy >> dummy >> sizeZ;
    newData >> dummy >> dummy >> sizeX;
    newData >> dummy >> dummy >> sizeY;

    this->sizeZ = sizeZ;
    this->sizeX = sizeX;
    this->sizeY = sizeY;
    newFile.close();
}
void DoseDistribution::loadGEANT4DoseFile(QString FileName)
{
    setlocale(LC_ALL, "Russian");

    QFile newFile(FileName);
    newFile.open(QIODevice::ReadOnly);
    QTextStream newData(&newFile);

    QString dummy;
    int sizeZ, sizeX, sizeY;
    newData >> dummy >> dummy >> sizeZ;
    newData >> dummy >> dummy >> sizeX;
    newData >> dummy >> dummy >> sizeY;
    //          i         j        k        Dose(Gy)
    newData >> dummy >> dummy >> dummy >> dummy;

    int x, y, z;
    double Dose, Maximum=0;

    while(!newFile.atEnd()) {
      newData >> z;
      newData >> x;
      newData >> y;
      newData >> Dose;
      if(newData.status() != QTextStream::Ok) break; // check status before using the data
      this->matrix[x][y][z]=Dose;
      if (Dose>Maximum) {
          Maximum = Dose;
      }
    }
    this ->doseMaxVal = Maximum;

    newFile.close();
}


int DoseDistribution::getXSize()
{
   return this->sizeX;
}
int DoseDistribution::getYSize()
{
   return this->sizeY;
}
int DoseDistribution::getZSize()
{
   return this->sizeZ;
}

void DoseDistribution::findAndSetDoseMaximum()
{
    double Maximum=0;
    for(int k=0;k<this->sizeZ;k++)
    {
        for(int j=0;j<this->sizeY;j++)
        {
            for(int i=0;i<this->sizeX;i++)
            {
                if(this->matrix[i][j][k]>Maximum)
                {
                    Maximum=matrix[i][j][k];
                }
            }
        }
    }
    this->doseMaxVal=Maximum;
}

double DoseDistribution::getDoseMaxValue()
{
    return this->doseMaxVal;
}
