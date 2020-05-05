#ifndef MOORETRACING_H
#define MOORETRACING_H

#include <vector>
#include <utility>

class MooreTracing
{
public:
    MooreTracing();
    MooreTracing(double** inpMatrix, int inpWidth, int inpHeight, double searchingLvl); //иниц + присвоение
    MooreTracing(std::vector<std::vector<double>> inpMatrix, int inpWidth, int inpHeight, double searchingLvl); //иниц + присвоение

    ~MooreTracing();    

    MooreTracing& operator=(const MooreTracing& other);

    // Задание рабочей матрицы
    int setNewMap(double** inpMatrix, int inpWidth, int inpHeight, double searchingLvl);
    int setNewMap(std::vector<std::vector<double>> inpMatrix, int inpWidth, int inpHeight, double searchingLvl);

    int updateSearchingLvl(double searchingLvl);

    // Поиск новой границы
    bool traceNewObj(bool clearCavities);

    int getTraceXmin();
    int getTraceXmax();
    int getTraceYmin();
    int getTraceYmax();


    typedef std::vector<int> intVector;

    // Вывод координат границ
    intVector getNewTraceX();
    intVector getNewTraceY();


    void printMtrx();


private:

    //=====================================================================
    ///==================0.Задание исходной матрицы========================
    //=====================================================================

    std::vector<std::vector<bool>> Map_mtrx;
    std::vector<std::vector<bool>> Map_mtrx_const;
    int Map_width, Map_height;

    void initMap();    // инициализация массива рабочей матрицы
    void destroyMap(); // уничтожение массива рабочей матрицы

    //=====================================================================
    ///==============================Алгоритм==============================
    /// 1. Найти точку старта поиска S, отметив предыдущую точку проверки P.
    ///    Присвоить текущей точки поиска C, значение точки S
    /// 2. Начать поиск контура:
    /// 2.1. Для точки поиска задать соседей Mc
    /// 2.2. Обойти соседей пр.часовой стрелки, начиная с точки P, пока
    ///      не найдется след.элемент, сохранить его как C. Сохранить
    ///      предыдущую проверенную точку, не явл. частью контура, как P
    /// 2.3. Повторять процедуру поиска для новой точки C, до возвращения в точку S
    ///      дважды, исключить из истории повторное прохождение
    /// 3. Исключить из исходной матрицы область внутри контура, сохранив или удалив полости
    //=====================================================================

    //=====================================================================
    ///=================1.Поиск новой стартовой точки======================
    //=====================================================================
    // Координаты точки с которых начинался предыдущий поиск
    int prevX0=0, prevY0=1;

    // Поиск новой стартовой точки
    bool locateNewStartForScanning();

    //=====================================================================
    ///========================2.Поиск контура=============================
    //=====================================================================

    //Внутренний контур
    intVector newTraceX;
    intVector newTraceY;

    //Внешний контур
   // intVector newTraceXout;
   // intVector newTraceYout;

    // Поиск контура
    void startTracing(std::vector<std::vector<bool>>& SearchingMap_mtrx,int startX, int startY,int inp_prevX, int inp_prevY);

    // Задание координат точек окрестности
    int MooreNeibours_X[8]{}, MooreNeibours_Y[8]{};
    void setNeighborhood(int consX,int consY,int prevX,int prevY);

    //Проверить есть в окрестности хоть что-то (на случай локальной точки)
    bool testNeighborhood();

    //Проверка окрестности и поиск след точки
    void traceNeighborhood(std::vector<std::vector<bool>>& SearchingMap_mtrx);
    int consX, consY; // координаты точки, которая рассматривается
    int prevX, prevY; // подтвержденные координаты

    //=====================================================================
    ///=======================3.Очиска контура=============================
    //=====================================================================

    // Крайние значения координат контура области
    int TraceXmin = 0;
    int TraceXmax = 0;
    int TraceYmin = 0;
    int TraceYmax = 0;
    void updateLimits();

    void clearArea(bool clearCavities);  // Очистка области

    void clearTrace(); // Очистка вектора контура

    //=====================================================================
    ///======================4.Служебные функции===========================
    //=====================================================================










};

#endif // MOORETRACING_H
