#include "Target.h"
#include "math.h"

#include <QFile>
#include <QTextStream>

Target::Target() {}

Target::Target(int sizeX,int sizeY,int sizeZ){
    this->sizeX = sizeX;
    this->sizeY = sizeY;
    this->sizeZ = sizeZ;

    tSize = sizeX*sizeY*sizeZ;
    // Создаю пустую матрицу
    target = TVector(tSize,0.0);

    QString Changes = "Target matrix:\n";
    Changes +=        "sizeX = "+QString::number(sizeX) +'\n';
    Changes +=        "sizeY = "+QString::number(sizeY) +'\n';
    Changes +=        "sizeZ = "+QString::number(sizeZ) +'\n';
    Changes +=        "======================="   ;
    addToChangeLog(Changes);

}
Target::Target(int sizeX,int sizeY,int sizeZ, double XShift ,double YShift,double ZShift){
    this->sizeX = sizeX;
    this->sizeY = sizeY;
    this->sizeZ = sizeZ;

    tSize = sizeX*sizeY*sizeZ;
    // Создаю пустую матрицу
    target = TVector(tSize,0.0);

    QString Changes = "Target matrix:\n";
    Changes +=        "sizeX = "+QString::number(sizeX) +'\n';
    Changes +=        "sizeY = "+QString::number(sizeY) +'\n';
    Changes +=        "sizeZ = "+QString::number(sizeZ) +'\n';
    Changes +=        "======================="   ;
    addToChangeLog(Changes);

    setShifts(XShift,YShift,ZShift);

}

Target::~Target() {}

Target& Target::operator=(const Target& other){
    //this->sizeX = other.getSizeX


    this->target = other.target;
    this->sizeX = other.sizeX;
    this->sizeY = other.sizeY;
    this->sizeZ = other.sizeZ;
    this->tSize = other.tSize;
    this->xShift = other.xShift;
    this->yShift = other.yShift;
    this->zShift = other.zShift;
    this->changeLog = other.changeLog;

    return *this;
}

///Coordinate correspondence to vector index
int Target::index(int X, int Y, int Z){
    return X + Y * sizeX + Z * sizeX * sizeY;
}

///Check element
int Target::element(int X, int Y, int Z){
    int indx=index(X,Y,Z);
    return target[indx];
}

void Target::setShifts(double XShift,double YShift,double ZShift){

    if ((this->xShift == XShift) &&
        (this->yShift == YShift) &&
        (this->zShift == ZShift)){
        return;
    }

    this->xShift = XShift;
    this->yShift = YShift;
    this->zShift = ZShift;

    QString Changes = "------------------------";
    Changes +=        "Shifts changes:\n";
    Changes +=        "XShift = "+QString::number(xShift) +'\n';
    Changes +=        "YShift = "+QString::number(yShift) +'\n';
    Changes +=        "ZShift = "+QString::number(zShift) +'\n';
    Changes +=        "------------------------"   ;
    addToChangeLog(Changes);
}

///Action = "Add" | "Remove"
void Target::setPoint(int X, int Y, int Z, QString Action){
    QString Changes = "Point ";
    if ((Action == "Add")||(Action == "add")||(Action == "ADD")){
        keyVal = 1;
        Changes +="added; ";
    }else if ((Action == "Remove")||(Action == "remove")||(Action == "REMOVE")){
        keyVal = 0;
        Changes +="removed; ";
    }

    Changes +="Coordinates =(" + QString::number(X) + ", "
                               + QString::number(Y) + ", "
                               + QString::number(Z) + ")";

    X += floor(xShift);
    Y += floor(yShift);
    Z += floor(zShift);

    X < 0 ? X = 0 : false;
    X >= sizeX ? X = sizeX-1 : false;

    Y < 0 ? Y = 0 : false;
    Y >= sizeX ? Y = sizeY-1 : false;

    Z < 0 ? Z = 0 : false;
    Z >= sizeX ? Z = sizeZ-1 : false;

    target[index(X,Y,Z)] = keyVal;
    addToChangeLog(Changes);


}

///Action = "Add" | "Remove"
void Target::setSphere(double Cx, double Cy, double Cz, double Rx, double Ry, double Rz, QString Action){
    QString Changes = "Spheroid ";
    if ((Action == "Add")||(Action == "add")||(Action == "ADD")){
        keyVal = 1; Changes +="added; ";
    }else if ((Action == "Remove")||(Action == "remove")||(Action == "REMOVE")){
        keyVal = 0; Changes +="removed; ";
    }

    Changes +=  "Center =(" + QString::number(Cx) + ", "
                            + QString::number(Cy) + ", "
                            + QString::number(Cz) + ")";
    Changes +="; Radii =("  + QString::number(Rx) + ", "
                            + QString::number(Ry) + ", "
                            + QString::number(Rz) + ")";

    double rcX = xShift + Cx-0;
    double rcY = yShift + Cy-0;
    double rcZ = zShift + Cz-0;


    //Определить края области

    int Xmin = floor(rcX - Rx);
    int Xmax =  ceil(rcX + Rx);

    int Ymin = floor(rcY - Ry);
    int Ymax =  ceil(rcY + Ry);

    int Zmin = floor(rcZ - Rz);
    int Zmax =  ceil(rcZ + Rz);

    // Проверка на выход за границы массива target
    Xmin < 0 ? Xmin = 0 : false;
    Ymin < 0 ? Ymin = 0 : false;
    Zmin < 0 ? Zmin = 0 : false;

    Xmax > sizeX ? Xmax = sizeX-1 : false;
    Ymax > sizeY ? Ymax = sizeY-1 : false;
    Zmax > sizeZ ? Zmax = sizeZ-1 : false;

    double pointX, pointY, pointZ;

    double side;
    for (int x=Xmin; x<=Xmax; x++){
        for (int y=Ymin; y<=Ymax; y++){
            for (int z=Zmin; z<=Zmax; z++){

                pointX=double(x - rcX);
                pointY=double(y - rcY);
                pointZ=double(z - rcZ);

                side = pow(pointX/Rx,2.0)+pow(pointY/Ry,2.0)+pow(pointZ/Rz,2.0);

                if (side <= 1){
                    target[index(x,y,z)] = keyVal;
                }
            }
        }
    }

    addToChangeLog(Changes);
}

///Action = "Add" | "Remove"
void Target::setCube(double Cx, double Cy, double Cz, double sideX, double sideY, double sideZ, QString Action){
    QString Changes = "Cube ";
    if ((Action == "Add")||(Action == "add")||(Action == "ADD")){
        keyVal = 1; Changes +="added; ";
    }else if ((Action == "Remove")||(Action == "remove")||(Action == "REMOVE")){
        keyVal = 0; Changes +="removed; ";
    }

    Changes +=  "Center =(" + QString::number(Cx) + ", "
                            + QString::number(Cy) + ", "
                            + QString::number(Cz) + ")";
    Changes +="; Sides =("  + QString::number(sideX) + ", "
                            + QString::number(sideY) + ", "
                            + QString::number(sideZ) + ")";

    //Определить края области и проверить на пренадлежность области массива
    int Xmin, Xmax, Ymin, Ymax, Zmin, Zmax;

    double rcX = xShift + Cx+0.5;
    double rcY = yShift + Cy+0.5;
    double rcZ = zShift + Cz+0.5;

    if (sideX == 1){
        Xmin = floor(rcX);
        Xmax = Xmin + 1;
    }else{
        Xmin = floor(rcX - 0.5*sideX);
        Xmax =  ceil(rcX + 0.5*sideX);
        Xmin < 0 ? Xmin = 0 : false;
        Xmax > sizeX ? Xmax = sizeX : false;
    }

    if (sideY == 1){
        Ymin = floor(yShift + Cy);
        Ymax = Ymin + 1;
    }else{
        Ymin = floor(rcY - 0.5*sideY);
        Ymax =  ceil(rcY + 0.5*sideY);
        Ymin < 0 ? Ymin = 0 : false;
        Ymax > sizeY ? Ymax = sizeY : false;
    }

    if (sideZ == 1){
        Zmin = floor(rcZ);
        Zmax = Zmin + 1;
    }else{
        Zmin = floor(rcZ - 0.5*sideZ);
        Zmax =  ceil(rcZ + 0.5*sideZ);
        Zmin < 0 ? Zmin = 0 : false;
        Zmax > sizeZ ? Zmax = sizeZ : false;
    }

    for (int x = Xmin; x < Xmax; x++){
        for (int y = Ymin; y < Ymax; y++){
            for (int z = Zmin; z < Zmax; z++){
                    target[index(x,y,z)] = keyVal;
            }
        }
    }
    addToChangeLog(Changes);
}

///Action = "Add" | "Remove", Direction = "X"|"Y"|"Z"
void Target::setTube(double Cx, double Cy, double Cz, double R1, double R2, double H, QString Direction, QString Action){

    QString Changes = "Cyllinder ";
    if ((Action == "Add")||(Action == "add")||(Action == "ADD")){
        keyVal = 1; Changes +="added; ";
    }else if ((Action == "Remove")||(Action == "remove")||(Action == "REMOVE")){
        keyVal = 0; Changes +="removed; ";
    }

    Changes +=  "Direction " + Direction;
    Changes +="; Center =(" + QString::number(Cx) + ", "
                            + QString::number(Cy) + ", "
                            + QString::number(Cz) + ")";
    Changes +="; R = "  + QString::number(R1) + ", "  + QString::number(R2);
    Changes +="; H = "  + QString::number(H);

    //Определить края области и проверить на пренадлежность области массива
    int Xmin, Xmax, Ymin, Ymax, Zmin, Zmax;
    double pointX, pointY, pointZ;
    double side;

    if ((Direction == "Z")||(Direction == "z")){
        double rcX = xShift + Cx;
        double rcY = yShift + Cy;
        double rcZ = zShift + Cz + 0.5;

        if (H == 1){
            Zmin = floor(rcZ);
            Zmax = Zmin + 1;
        }else{
            Zmin = floor(rcZ - 0.5*H);
            Zmax =  ceil(rcZ + 0.5*H);
            Zmin < 0 ? Zmin = 0 : false;
            Zmax > sizeZ ? Zmax = sizeZ-1 : false;
        }

        Xmin = floor(rcX - R1);
        Xmin < 0 ? Xmin = 0 : false;
        Xmax =  ceil(rcX + R1);
        Xmax > sizeX ? Xmax = sizeX-1 : false;

        Ymin = floor(rcY - R2);
        Ymin < 0 ? Ymin = 0 : false;
        Ymax =  ceil(rcY + R2);
        Ymax > sizeY ? Ymax = sizeY-1 : false;

        for (int z = Zmin; z < Zmax; z++){
            for (int x=Xmin; x<=Xmax; x++){
                for (int y=Ymin; y<=Ymax; y++){
                    pointX=double(x - rcX);
                    pointY=double(y - rcY);
                    //pointZ=double(z - rcZ);

                    side = pow(pointX/R1,2.0)+pow(pointY/R2,2.0);

                    if (side <= 1) target[index(x,y,z)] = keyVal;
                }
            }
        }


    }else if ((Direction == "X")||(Direction == "x")){
        double rcX = xShift + Cx + 0.5;
        double rcY = yShift + Cy;
        double rcZ = zShift + Cz;

        if (H == 1){
            Xmin = floor(rcX);
            Xmax = Xmin + 1;
        }else{
            Xmin = floor(rcX - 0.5*H);
            Xmax =  ceil(rcX + 0.5*H);
            Xmin < 0 ? Xmin = 0 : false;
            Xmax > sizeX ? Xmax = sizeX-1 : false;
        }

        Zmin = floor(rcZ - R1);
        Zmin < 0 ? Zmin = 0 : false;
        Zmax =  ceil(rcZ + R1);
        Zmax > sizeZ ? Zmax = sizeZ-1 : false;

        Ymin = floor(rcY - R2);
        Ymin < 0 ? Ymin = 0 : false;
        Ymax =  ceil(rcY + R2);
        Ymax > sizeY ? Ymax = sizeY-1 : false;


        for (int x = Xmin; x < Xmax; x++){
            for (int z=Zmin; z<=Zmax; z++){
                for (int y=Ymin; y<=Ymax; y++){
                    //pointX=double(x - rcX);
                    pointY=double(y - rcY);
                    pointZ=double(z - rcZ);

                    side = pow(pointZ/R1,2.0)+pow(pointY/R2,2.0);

                    if (side <= 1) target[index(x,y,z)] = keyVal;
                }
            }
        }

    }else if ((Direction == "Y")||(Direction == "y")){
        double rcX = xShift + Cx;
        double rcY = yShift + Cy + 0.5;
        double rcZ = zShift + Cz;

        if (H == 1){
            Ymin = floor(rcY);
            Ymax = Ymin + 1;
        }else{
            Ymin = floor(rcY - 0.5*H);
            Ymax =  ceil(rcY + 0.5*H);
            Ymin < 0 ? Ymin = 0 : false;
            Ymax > sizeY ? Ymax = sizeY-1 : false;
        }

        Zmin = floor(rcZ - R1);
        Zmin < 0 ? Zmin = 0 : false;
        Zmax =  ceil(rcZ + R1);
        Zmax > sizeZ ? Zmax = sizeZ-1 : false;

        Xmin = floor(rcX - R2);
        Xmin < 0 ? Xmin = 0 : false;
        Xmax =  ceil(rcX + R2);
        Xmax > sizeX ? Xmax = sizeX-1 : false;


        for (int y = Ymin; y < Ymax; y++){
            for (int z=Zmin; z<=Zmax; z++){
                for (int x=Xmin; x<=Xmax; x++){
                    pointX=double(x - rcX);
                    //pointY=double(y - rcY);
                    pointZ=double(z - rcZ);

                    side = pow(pointZ/R1,2.0)+pow(pointX/R2,2.0);

                    if (side <= 1) target[index(x,y,z)] = keyVal;
                }
            }
        }


    }else return;


    addToChangeLog(Changes);
}

///save file to .dat file, SLIKA style file model
void Target::saveTarget2File(QString filename)
{
    QFile newFile(filename);
    newFile.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text);
    QTextStream newData(&newFile);
    newData << sizeZ << '\t' << sizeX << '\t' << sizeY;

    for (int z=0; z < sizeZ; z++){
        newData << '\n';
        for (int y=0; y < sizeY; y++){
            for (int x=0; x < sizeX; x++){
                newData << target[index(x,y,z)] << "  ";
            }
            newData << '\n';
        }
    }
    newFile.close();
}


///init target by loading from file | true if failed
bool Target::loadTargetFromFile(QString filename)
{
    bool errorAcured = false;
    int nSizeX, nSizeY, nSizeZ, nTSize;
    TVector nTarget;

    setlocale(LC_ALL, "Russian");
    QFile newFile(filename);
    newFile.open(QIODevice::ReadOnly);
    QTextStream newData(&newFile);


    newData >> nSizeZ >> nSizeX >> nSizeY;

    nTSize= nSizeX*nSizeY*nSizeZ;
    nTarget.resize(nTSize);

    unsigned int objPresence;
    int indx;
    // Считывание массива
    for (int k=0; k < sizeZ; k++){
        for (int j=0; j < sizeY; j++){
            for (int i=0; i < sizeX; i++){
                newData >> objPresence;
                indx=index(i,j,k);
                nTarget[indx] = objPresence;
            }
        }
    }

    newFile.close();

    if(!errorAcured){
        sizeX = nSizeX;
        sizeY = nSizeY;
        sizeZ = nSizeZ;
        tSize = nTSize;

        xShift = yShift = zShift = 0;

        target.clear();
        target = nTarget;

        QString changeLog = "Target change log";
    }

    return errorAcured;
}
