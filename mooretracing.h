#ifndef MOORETRACING_H
#define MOORETRACING_H

#include <vector>
#include <utility>

class MooreTracing
{
public:
     MooreTracing(double** inpMatrix, int inpWidth, int inpHeight, double searchingLvl); //иниц + присвоение
    ~MooreTracing();

  // Задание рабочей матрицы
     int setNewMap(double** inpMatrix, int inpWidth, int inpHeight, double searchingLvl);
     int updateSearchingLvl(double searchingLvl);
  // Поиск новой границы
     bool traceNewObj(bool clearCavities);
     int getTraceXmin();
     int getTraceXmax();
     int getTraceYmin();
     int getTraceYmax();


  // Вывод координат границ
     std::vector<int> getNewTraceX();
     std::vector<int> getNewTraceY();


     void printMtrx();
private:

    bool** Map_mtrx;
    int Map_width, Map_height;
    double** baseMtrx;

    void initMap();    // инициализация динамического массива рабочей матрицы
    void destroyMap(); // уничтожение   динамического массива рабочей матрицы

    std::vector<int> newTraceX;
    std::vector<int> newTraceY;

    int prevX0=0, prevY0=1; // координаты точки с которых начинался предыдущий поиск
    //void locateForNewEntryPoint();
    bool locateNewStartForScanning(); // Поиск новой стартовой точки



    void startTracing();              // Поиск контура
    void clearTrace(); // очистка вектора контура

    int MooreNeibours_X[8]{}, MooreNeibours_Y[8]{};
    void setNeighborhood(int consX,int consY,int prevX,int prevY); // Задание координат точек окрестности

    bool testNeighborhood(); //Проверить есть в окрестности хоть что-то (на случай локальной точки)
    void traceNeighborhood(); //Проверка окрестности и поиск след точки
    int consX, consY; // координаты точки, которая рассматривается
    int prevX, prevY; // подтвержденные координаты

    void clearArea(bool clearCavities);  // Очистка области
    int TraceXmin = 0;
    int TraceXmax = 0;
    int TraceYmin = 0;
    int TraceYmax = 0;
    void updateLimits();










};

#endif // MOORETRACING_H
