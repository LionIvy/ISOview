#include "mooretracing.h"

#include <iostream>
#include <vector>
MooreTracing::MooreTracing(){}
MooreTracing::MooreTracing(double** inpMatrix, int inpWidth, int inpHeight, double searchingLvl)
{
    Map_width = inpWidth + 2;
    Map_height = inpHeight + 2;

    initMap();
    setNewMap(inpMatrix, inpWidth, inpHeight, searchingLvl);

    //printMtrx();
}
MooreTracing::MooreTracing(std::vector<std::vector<double>> inpMatrix, int inpWidth, int inpHeight, double searchingLvl)
{
    Map_width = inpWidth + 2;
    Map_height = inpHeight + 2;

    initMap();
    setNewMap(inpMatrix, inpWidth, inpHeight, searchingLvl);

}


MooreTracing::~MooreTracing()
{
   destroyMap();
}


MooreTracing& MooreTracing::operator=(const MooreTracing &other)
{
    Map_width = other.Map_width;
    Map_height = other.Map_height;

    Map_mtrx = other.Map_mtrx;
    Map_mtrx_const = other.Map_mtrx_const;

    return *this;
}

//=====================================================================
///==================0.Задание исходной матрицы========================
//=====================================================================

void MooreTracing::initMap()
{
    Map_mtrx.resize(Map_width);
    for (int i=0; i<Map_width; i++)
    {
        Map_mtrx[i].resize(Map_height);
        std::fill(Map_mtrx[i].begin(),Map_mtrx[i].end(),false);
    }
    Map_mtrx_const = Map_mtrx;
}

void MooreTracing::destroyMap()
{
    Map_mtrx.clear();
    Map_mtrx_const.clear();
}

///Есть проблема, которая приводит к появлению спиралей
int MooreTracing::setNewMap(double** inpMatrix, int inpWidth, int inpHeight, double searchingLvl)
{
    // Проверка размерностей
    if ((inpWidth!=(Map_width)-2) || (inpHeight!=(Map_height)-2))
    {
        return 1; // ошибка, размеры матриц не соответствуют
    }


    // Присвоение элементов, соответствующих исследуемой матрице
    for (int i=0; i<inpWidth; i++)
    {
        for (int j=0; j<inpHeight; j++)
        {
            if (inpMatrix[i][j]>=searchingLvl)
            {
                Map_mtrx[i+1][j+1]=true;
            }else{
                Map_mtrx[i+1][j+1]=false;
            }
        }
    }

    Map_mtrx_const = Map_mtrx;
    return 0; //код отработал исправно
}
int MooreTracing::setNewMap(std::vector<std::vector<double>> inpMatrix, int inpWidth, int inpHeight, double searchingLvl)
{
    // Проверка размерностей
    if ((inpWidth!=(Map_width)-2) || (inpHeight!=(Map_height)-2))
    {
        return 1; // ошибка, размеры матриц не соответствуют
    }


    // Присвоение элементов, соответствующих исследуемой матрице
    for (int i=0; i<inpWidth; i++)
    {
        for (int j=0; j<inpHeight; j++)
        {
            if (inpMatrix[i][j]>=searchingLvl)
            {
                Map_mtrx[i+1][j+1]=true;
            }else{
                Map_mtrx[i+1][j+1]=false;
            }
        }
    }

    Map_mtrx_const = Map_mtrx;
    return 0; //код отработал исправно
}

/*
int MooreTracing::updateSearchingLvl(double searchingLvl)
{
    //destroyMap();
    //initMap();
    for (int i=0; i<Map_width; i++)
    {
        for (int j=0; j<Map_width; j++)
        {
                Map_mtrx[i][j]=false;
        }
    }
    // Присвоение элементов, соответствующих исследуемой матрице
    for (int i=0; i<Map_width-2; i++)
    {
        for (int j=0; j<Map_height-2; j++)
        {
            if (baseMtrx[i][j]>=searchingLvl)
            {
                Map_mtrx[i+1][j+1]=true;
            }else{
                Map_mtrx[i+1][j+1]=false;
            }
        }
    }

    return 0; //код отработал исправно
}
*/


//=====================================================================
///=================1.Поиск новой стартовой точки======================
//=====================================================================

/// Поиск новой точки входа в цикл поиска, возвращает значение нашел/не нашел
bool MooreTracing::locateNewStartForScanning()
{
    //printMtrx();

    int X = prevX0;
    int Y = prevY0;
    if (X >= (Map_width - 2) )  // -_______0-
    {
        X = 0;
        Y++  ;
    }

    bool pointIsNotFound = true;

    //сначала закончим сканирование по координате X, при этом Y фиксирован
    do{
        X++;
        //if(Map_mtrx_upd[X][Y])
        if(Map_mtrx[X][Y])
        {
            pointIsNotFound = false;
            prevX0 = X;
            prevY0 = Y;
        }
    }while(pointIsNotFound && (X < (Map_width - 1)));

    //
    if(pointIsNotFound)
    {
      do{ //сканирование по Y
          X = 0;
          Y++  ;
          do{ //сканирование по X
              X++;
              if(Map_mtrx[X][Y])
              {
                  pointIsNotFound = false;
                  prevX0 = X;
                  prevY0 = Y;
              }
          }while(pointIsNotFound && (X < (Map_width - 1)) );//&& (Y < (Map_height - 1))требуется правка
      }while(pointIsNotFound &&  (Y < (Map_height - 1)));//(X < (Map_width - 1)) &&
    }
    return !pointIsNotFound;
}

//=====================================================================
///========================2.Поиск контура=============================
//=====================================================================

/// Поиск нового контура, входной параметр - удалить или сохранить полость из зоны поиска,
///  возвращает значение нашел контур или нет
bool MooreTracing::traceNewObj(bool clearCavities)
{
    bool NewTraceFound;
    if (locateNewStartForScanning())  //Просканировать матрицу на наличие объектов
    {
        //Найден объект
        NewTraceFound = true;

        //Поиск контура объекта


        startTracing(Map_mtrx,prevX0,prevY0,prevX0-1,prevY0);

       // При необходимости поиска полостей, нужно доконтурить объект
       if (!clearCavities){
           int startX = newTraceX[1];
           int startY = newTraceY[1];
           int inp_prevX = newTraceX[0];
           int inp_prevY = newTraceY[0];

           setNeighborhood(startX,startY,inp_prevX,inp_prevY);
           traceNeighborhood(Map_mtrx_const);
          // setNeighborhood(startX,startY,inp_prevX,inp_prevY);
          // traceNeighborhood(Map_mtrx_const);

           startX = consX;
           startY = consY;
           inp_prevX = prevX;
           inp_prevY = prevY;

           startTracing(Map_mtrx_const,startX,startY,inp_prevX,inp_prevY);
       }

        clearArea(clearCavities);     //Удаление объекта из матрицы

       /* std::cout << '\n';
        std::cout << "Cleared mtrx:\n";
        std::cout << '\n';
        printMtrx();*/
    }else{
        NewTraceFound = false;//новая точка не найдена
    }
    return NewTraceFound;
}

/// Поиск внешнего контура
void MooreTracing::startTracing(std::vector<std::vector<bool>>& SearchingMap_mtrx,int startX, int startY,int inp_prevX, int inp_prevY)
{
    clearTrace(); // при повторном считывании следует обнулить контур

    consX = startX;
    consY = startY;
    newTraceX.push_back(consX);
    newTraceY.push_back(consY);

    TraceXmin=startX;
    TraceXmax=startX;
    TraceYmin=startY;
    TraceYmax=startY;

   // int traceSize = 0;
   // std::cout<<newTraceX.size() << '\n';
   // std::cout<<"X,Y = " << newTraceX[traceSize] << "  " << newTraceY[traceSize] << std::endl;

    setNeighborhood(startX,startY,inp_prevX, inp_prevY);

    if (testNeighborhood()) return; // Если точка одна в котуре - возврат

    traceNeighborhood(SearchingMap_mtrx);
    newTraceX.push_back(consX);
    newTraceY.push_back(consY);
    updateLimits();

   // traceSize++;
   // std::cout<<newTraceX.size() << '\n';
   // std::cout<<"X,Y = " << newTraceX[traceSize] << "  " << newTraceY[traceSize] << std::endl;
    int attempt=1;
    int I[3]{};
    while(attempt<=2)
    {
        setNeighborhood(consX,consY,prevX,prevY);
        traceNeighborhood(SearchingMap_mtrx);
        newTraceX.push_back(consX);
        newTraceY.push_back(consY);
        updateLimits();

        if(((newTraceX[0] == consX) && (newTraceY[0] == consY)))
        {
            I[attempt]=newTraceX.size()-1;
            attempt++;
        }
    //    traceSize++;
    //    std::cout<<newTraceX.size() << '\n';
    //    std::cout<<"X,Y = " << newTraceX.at(traceSize) << "  " << newTraceY.at(traceSize) << std::endl;
    }

    if((newTraceX[I[0]+1] == newTraceX[I[1]+1]) &&
       (newTraceY[I[0]+1] == newTraceY[I[1]+1]) &&
       (newTraceX[I[0]+2] == newTraceX[I[1]+2]) &&
       (newTraceY[I[0]+2] == newTraceY[I[1]+2]) )
    {
        newTraceX.resize(I[1]+1);
        newTraceY.resize(I[1]+1);
    }
}

/// Задать точки соседства, начиная с предыдущей исследованной
void MooreTracing::setNeighborhood(int consX,int consY,int prevX,int prevY)
{
    int N = consY + 1; // СЕВЕР
    int S = consY - 1; // ЮГ
    int keepY = consY;

    int W = consX + 1; // ЗАПАД
    int E = consX - 1; // ВОСТОК
    int keepX = consX;

    if (consX > prevX) // E
    {
        if(consY == prevY)       //prev  E, next NE
        {
            MooreNeibours_Y[0] = keepY;
            MooreNeibours_X[0] = E;

            MooreNeibours_Y[1] = N;
            MooreNeibours_X[1] = E;

            MooreNeibours_Y[2] = N;
            MooreNeibours_X[2] = keepX;

            MooreNeibours_Y[3] = N;
            MooreNeibours_X[3] = W;

            MooreNeibours_Y[4] = keepY;
            MooreNeibours_X[4] = W;

            MooreNeibours_Y[5] = S;
            MooreNeibours_X[5] = W;

            MooreNeibours_Y[6] = S;
            MooreNeibours_X[6] = keepX;

            MooreNeibours_Y[7] = S;
            MooreNeibours_X[7] = E;


        }else if (consY > prevY) //prev SE, next  E
        {
            MooreNeibours_Y[0] = S;
            MooreNeibours_X[0] = E;

            MooreNeibours_Y[1] = keepY;
            MooreNeibours_X[1] = E;

            MooreNeibours_Y[2] = N;
            MooreNeibours_X[2] = E;

            MooreNeibours_Y[3] = N;
            MooreNeibours_X[3] = keepX;

            MooreNeibours_Y[4] = N;
            MooreNeibours_X[4] = W;

            MooreNeibours_Y[5] = keepY;
            MooreNeibours_X[5] = W;

            MooreNeibours_Y[6] = S;
            MooreNeibours_X[6] = W;

            MooreNeibours_Y[7] = S;
            MooreNeibours_X[7] = keepX;


        }else if (consY < prevY) //prev NE, next N
        {
            MooreNeibours_Y[0] = N;
            MooreNeibours_X[0] = E;

            MooreNeibours_Y[1] = N;
            MooreNeibours_X[1] = keepX;

            MooreNeibours_Y[2] = N;
            MooreNeibours_X[2] = W;

            MooreNeibours_Y[3] = keepY;
            MooreNeibours_X[3] = W;

            MooreNeibours_Y[4] = S;
            MooreNeibours_X[4] = W;

            MooreNeibours_Y[5] = S;
            MooreNeibours_X[5] = keepX;

            MooreNeibours_Y[6] = S;
            MooreNeibours_X[6] = E;

            MooreNeibours_Y[7] = keepY;
            MooreNeibours_X[7] = E;
        }
    }else if (consX == prevX) //0
    {
        if (consY > prevY)       //prev S , next SE
        {
            MooreNeibours_Y[0] = S;
            MooreNeibours_X[0] = keepX;

            MooreNeibours_Y[1] = S;
            MooreNeibours_X[1] = E;

            MooreNeibours_Y[2] = keepY;
            MooreNeibours_X[2] = E;

            MooreNeibours_Y[3] = N;
            MooreNeibours_X[3] = E;

            MooreNeibours_Y[4] = N;
            MooreNeibours_X[4] = keepX;

            MooreNeibours_Y[5] = N;
            MooreNeibours_X[5] = W;

            MooreNeibours_Y[6] = keepY;
            MooreNeibours_X[6] = W;

            MooreNeibours_Y[7] = S;
            MooreNeibours_X[7] = W;

        }else if (consY < prevY) //prev N , next NW
        {
            MooreNeibours_Y[0] = N;
            MooreNeibours_X[0] = keepX;

            MooreNeibours_Y[1] = N;
            MooreNeibours_X[1] = W;

            MooreNeibours_Y[2] = keepY;
            MooreNeibours_X[2] = W;

            MooreNeibours_Y[3] = S;
            MooreNeibours_X[3] = W;

            MooreNeibours_Y[4] = S;
            MooreNeibours_X[4] = keepX;

            MooreNeibours_Y[5] = S;
            MooreNeibours_X[5] = E;

            MooreNeibours_Y[6] = keepY;
            MooreNeibours_X[6] = E;

            MooreNeibours_Y[7] = N;
            MooreNeibours_X[7] = E;
        }
    }else if (consX < prevX) //W
    {
        if ( consY == prevY)     //prev W , next SW
        {
            MooreNeibours_Y[0] = keepY;
            MooreNeibours_X[0] = W;

            MooreNeibours_Y[1] = S;
            MooreNeibours_X[1] = W;

            MooreNeibours_Y[2] = S;
            MooreNeibours_X[2] = keepX;

            MooreNeibours_Y[3] = S;
            MooreNeibours_X[3] = E;

            MooreNeibours_Y[4] = keepY;
            MooreNeibours_X[4] = E;

            MooreNeibours_Y[5] = N;
            MooreNeibours_X[5] = E;

            MooreNeibours_Y[6] = N;
            MooreNeibours_X[6] = keepX;

            MooreNeibours_Y[7] = N;
            MooreNeibours_X[7] = W;

        }else if (consY > prevY) //prev SW, next S
        {
            MooreNeibours_Y[0] = S;
            MooreNeibours_X[0] = W;

            MooreNeibours_Y[1] = S;
            MooreNeibours_X[1] = keepX;

            MooreNeibours_Y[2] = S;
            MooreNeibours_X[2] = E;

            MooreNeibours_Y[3] = keepY;
            MooreNeibours_X[3] = E;

            MooreNeibours_Y[4] = N;
            MooreNeibours_X[4] = E;

            MooreNeibours_Y[5] = N;
            MooreNeibours_X[5] = keepX;

            MooreNeibours_Y[6] = N;
            MooreNeibours_X[6] = W;

            MooreNeibours_Y[7] = keepY;
            MooreNeibours_X[7] = W;

        }else if (consY < prevY) //prev NW, next  W
        {
            MooreNeibours_Y[0] = N;
            MooreNeibours_X[0] = W;

            MooreNeibours_Y[1] = keepY;
            MooreNeibours_X[1] = W;

            MooreNeibours_Y[2] = S;
            MooreNeibours_X[2] = W;

            MooreNeibours_Y[3] = S;
            MooreNeibours_X[3] = keepX;

            MooreNeibours_Y[4] = S;
            MooreNeibours_X[4] = E;

            MooreNeibours_Y[5] = keepY;
            MooreNeibours_X[5] = E;

            MooreNeibours_Y[6] = N;
            MooreNeibours_X[6] = E;

            MooreNeibours_Y[7] = N;
            MooreNeibours_X[7] = keepX;
        }
    }
}

/// Проверка окрестностей, р-т 1 = точка-одиночка / 0 = точка не однаж
bool MooreTracing::testNeighborhood()
{
    bool isASinglePoint = true;
    int i=-1;

    while(isASinglePoint && (i<7)) //i=-1 0 1 2 3 4 5 6
    {
        i++;                       //i= 0 1 2 3 4 5 6 7
        if(Map_mtrx[MooreNeibours_X[i]][MooreNeibours_Y[i]])
        {
            isASinglePoint = false;
        }
    }

    return isASinglePoint;
}

/// Поиск новой точки в окрестностях известной
void MooreTracing::traceNeighborhood(std::vector<std::vector<bool>>& SearchingMap_mtrx)
{
    bool isASinglePoint = true;
    int i=0;

    while(isASinglePoint && (i<7)) //i= 0 1 2 3 4 5 6
    {
        i++;                       //i= 1 2 3 4 5 6 7
        if(SearchingMap_mtrx[MooreNeibours_X[i]][MooreNeibours_Y[i]])
        {
            isASinglePoint = false;
            prevX =MooreNeibours_X[i-1];// consX;
            prevY =MooreNeibours_Y[i-1];// consY;
            consX=MooreNeibours_X[i];
            consY=MooreNeibours_Y[i];
        }
    }
}

//=====================================================================
///=======================3.Очиска контура=============================
//=====================================================================

/// Поиск диапазона значений координат контура
void MooreTracing::updateLimits()
{
    // поиск диапазона значений X
    if(consX < TraceXmin)
    {
        TraceXmin = consX;
    }else if (consX > TraceXmax)
    {
         TraceXmax = consX;
    }

    // поиск диапазона значений Y
    if(consY < TraceYmin)
    {
        TraceYmin = consY;
    }else if (consY > TraceYmax)
    {
         TraceYmax = consY;
    }
}

///Очистить матрицу от исследованной области
void MooreTracing::clearArea(bool clearCavities)
{
    unsigned int len = newTraceX.size();

    int Xmin = TraceXmin;
    int Xmax = TraceXmax;

    //избавление матрицы от самого контура, чтобы избавиться от одиночек и поиск диапазона значений X
    for (unsigned int i=0; i< len; i++)
    {
        //избавление матрицы от самого контура, чтобы избавиться от одиночек и горизонтальных линий
        Map_mtrx[newTraceX[i]][newTraceY[i]]=false;
    }

    //на случай, если точек меньше 3 - выход из функции
    if (len<=2){
        return;
    }

    //если область шире:

    //задаем массив узлов области по списку координат (двумерный)
    std::vector<std::vector<int>> IndexList((Xmax-Xmin+1), std::vector<int>());
    int sgnNext, sgnPrev; //знаки справа и слева от точки (позволяет отделить ветикальные линии и точки

    sgnNext = newTraceX[1] - newTraceX[0];
    sgnPrev = newTraceX[len-2] - newTraceX[0];

    if ((sgnNext != sgnPrev)||((sgnNext == 0) && (sgnPrev == 0)))  // РАЗНЫЙ ЗНАК ОЗНАЧАЕТ ТОЧКУ ПОВОРОТА. ОДИНАКОВЫЙ - ВЕРТИКАЛЬ И ТОЧКИ. ТОЧКИ УЖЕ ИСКЛЮЧИЛИ
    {
        IndexList[newTraceX[0] - Xmin].push_back(0);
    }
    for (unsigned int i=1; i< len-1; i++)
    {
        sgnNext = newTraceX[i+1] - newTraceX[i];
        sgnPrev = newTraceX[i-1] - newTraceX[i];

        if ((sgnNext != sgnPrev)||((sgnNext == 0) && (sgnPrev == 0)))  // РАЗНЫЙ ЗНАК ОЗНАЧАЕТ ТОЧКУ ПОВОРОТА. ОДИНАКОВЫЙ - ВЕРТИКАЛЬ И ТОЧКИ. ТОЧКИ УЖЕ ИСКЛЮЧИЛИ
        {
            IndexList[newTraceX[i] - Xmin].push_back(i);
        }
    }

    int cXval, cYval, cYval2; //текущее зн-е Х и Y
    int YvalSize;     //количество узлов для заданного Х
    int buffY=0;
    bool notSorted;

    //Процедура удаления точек
    if(clearCavities){
        for (int I=0; I < (Xmax-Xmin+1);I++)
        {
            if (IndexList[I].empty())
            {
                continue;//на случай если узел был одинокой точкой
            }

            cXval = newTraceX[IndexList[I][0]]; //текущая кордината Х
            YvalSize=IndexList[I].size();             //количество узлов для заданного Х

            do{
                notSorted = false;
                for(int j = 0; j < YvalSize - 1; j++)
                {
                    if(newTraceY[IndexList[I][j]] > newTraceY[IndexList[I][j + 1]])
                    {
                        // меняем элементы местами
                        buffY = IndexList[I][j];
                        IndexList[I][j] = IndexList[I][j + 1];
                        IndexList[I][j + 1] = buffY;
                        notSorted=true;
                    }else if (newTraceY[IndexList[I][j]] == newTraceY[IndexList[I][j + 1]])
                    {
                        //iter=IndexList[I].end();
                        IndexList[I][j + 1] = IndexList[I][YvalSize-1];
                        IndexList[I].pop_back();
                        YvalSize--;
                        notSorted = true;
                    }
                }
            }while(notSorted);

            //удаление точек
            for(int J=0;J<YvalSize-1;J++)
            {
                cYval = newTraceY[IndexList[I][J]];
                cYval2 = newTraceY[IndexList[I][J+1]];

                if(( (cYval+1) < cYval2) && !(Map_mtrx[cXval][cYval-1])) // внутри
                {
                    for (int K=cYval+1;K < cYval2; K++)
                    {
                        //Map_mtrx_upd[cXval][K]=false;
                        Map_mtrx[cXval][K]=false;
                    }
                }
            }
        }
    }else{
        for (int I=0; I < (Xmax-Xmin+1);I++)
        {
            if (IndexList[I].empty()) continue; //на случай если узел был одинокой точкой

            cXval = newTraceX[IndexList[I][0]]; //текущая кордината Х
            YvalSize=IndexList[I].size();             //количество узлов для заданного Х


            do{
                notSorted = false;
                for(int j = 0; j < YvalSize - 1; j++)
                {
                    if(newTraceY[IndexList[I][j]] > newTraceY[IndexList[I][j + 1]])
                    {
                        // меняем элементы местами
                        buffY = IndexList[I][j];
                        IndexList[I][j] = IndexList[I][j + 1];
                        IndexList[I][j + 1] = buffY;
                        notSorted=true;
                    }else if (newTraceY[IndexList[I][j]] == newTraceY[IndexList[I][j + 1]])
                    {
                        //iter=IndexList[I].end();
                        IndexList[I][j + 1] = IndexList[I][YvalSize-1];
                        IndexList[I].pop_back();
                        YvalSize--;
                        notSorted = true;
                    }
                }
            }while(notSorted);

            // Удаление точек
            for(int J=0;J<YvalSize-1;J++) {
                cYval = newTraceY[IndexList[I][J]];
                cYval2 = newTraceY[IndexList[I][J+1]];

                if(( (cYval+1) < cYval2) && !(Map_mtrx[cXval][cYval-1])) // внутри
                {
                    for (int K=cYval+1;K < cYval2; K++)
                    {
                            //Map_mtrx_upd[cXval][K]=!(Map_mtrx[cXval][K]);
                            Map_mtrx[cXval][K]=!(Map_mtrx[cXval][K]);
                    }
                }
            }
        }
    }

        //Повторное избавление матрицы от самого контура
        for (unsigned int i=0; i< len; i++) {
            //избавление матрицы от самого контура, чтобы избавиться от одиночек и горизонтальных линий
            Map_mtrx[newTraceX[i]][newTraceY[i]]=false;
        }
}

/// Очистка векторов от данных
void MooreTracing::clearTrace()
{
   // std::cout << "TraceSize = " << newTraceX.size() << std::endl;

    newTraceX.clear();
    newTraceY.clear();

   // std::cout << "TraceSize = " << newTraceX.size() << std::endl;
}


//=====================================================================
///======================4.Служебные функции===========================
//=====================================================================
void MooreTracing::printMtrx()
{
    int v;
    for (int j = 0; j < Map_height; j++)
    {
        std::cout << std::endl;
        for (int i = 0; i < Map_width; i++)
        {
            if (Map_mtrx[i][j] == true)
            {
                v=1;
            }else{
                v=0;
            }
            std::cout << v << "\t";
        }
    }
}

//=====================================================================
///===========================5.Геттеры================================
//=====================================================================

std::vector<int> MooreTracing::getNewTraceX()
{
    int size =(newTraceX).size();
    std::vector<int> res (size);
    for (int i=0; i<size; i++)
    {
      res[i]=  (newTraceX)[i]-1;
    }
    return res;
}
std::vector<int> MooreTracing::getNewTraceY()
{
    int size =(newTraceY).size();
     std::vector<int> res (size);
     for (int i=0; i<size; i++)
     {
       res[i]=  (newTraceY)[i]-1;
     }
    return res;
}


int MooreTracing::getTraceXmin()
{
    return TraceXmin;
}
int MooreTracing::getTraceXmax()
{
    return TraceXmax;
}
int MooreTracing::getTraceYmin()
{
    return TraceYmin;
}
int MooreTracing::getTraceYmax()
{
    return TraceYmax;
}


