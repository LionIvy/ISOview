#ifndef TARGET_H
#define TARGET_H

#include <vector>
#include <QString>

class Target
{
public:
    Target();
    Target(int sizeX,int sizeY,int sizeZ);
    Target(int sizeX,int sizeY,int sizeZ, double XShift,double YShift,double ZShift);
    ~Target();

    Target& operator=(const Target& other);

    void setShifts(double XShift,double YShift,double ZShift);

    int getSizeX(){return sizeX;}
    int getSizeY(){return sizeY;}
    int getSizeZ(){return sizeZ;}

    QString getChangeLog(){return changeLog;}

    ///Check element
    int element(int X, int Y, int Z);

    ///Action = "Add" | "Remove"
    void setPoint(int X, int Y, int Z, QString Action);

    ///Action = "Add" | "Remove"
    void setSphere(double Cx, double Cy, double Cz, double Rx, double Ry, double Rz, QString Action);

    ///Action = "Add" | "Remove"
    void setCube(double Cx, double Cy, double Cz, double sideX, double sideY, double sideZ, QString Action);

    ///Action = "Add" | "Remove", Direction = "X"|"Y"|"Z"
    void setTube(double Cx, double Cy, double Cz, double R1, double R2, double H, QString Direction, QString Action);


    void saveTarget2File(QString filename);
    bool loadTargetFromFile(QString filename);


    typedef std::vector<unsigned int> TVector;

private:
    TVector target;
    int sizeX = 0, sizeY = 0, sizeZ = 0;
    int tSize = sizeX*sizeY*sizeZ;
    double xShift = 0, yShift = 0, zShift = 0;
    QString changeLog = "Target change log";
    unsigned int keyVal;

    int index(int X, int Y, int Z);

    void addToChangeLog(QString Changes){
        changeLog+='\n'+Changes;
    }


};

#endif // TARGET_H
