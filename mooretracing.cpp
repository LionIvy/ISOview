#include "mooretracing.h"

#include <iostream>


MooreTracing::MooreTracing(double** inpMatrix, int inpWidth, int inpHeight, double searchingLvl)
{
    this->Map_width = inpWidth + 2;
    this->Map_height = inpHeight + 2;
    //this->baseMtrx = inpMatrix; так нельзя, это указатель.

    initMap();
    setNewMap(inpMatrix, inpWidth, inpHeight, searchingLvl);

    //printMtrx();
}

MooreTracing::~MooreTracing()
{
   destroyMap();
}

void MooreTracing::initMap()
{
    this->Map_mtrx = new bool* [this->Map_width]{};
    for (int i=0; i<this->Map_width; i++)
    {
        this->Map_mtrx[i]=new bool[this->Map_height]{};
    }
}

void MooreTracing::destroyMap()
{
    for (int i=0;i<this->Map_width;i++)
    {
        delete [] this->Map_mtrx[i];
    }
    delete [] this->Map_mtrx;
    this->Map_mtrx = nullptr;
}

///Есть проблема, которая приводит к появлению спиралей
int MooreTracing::setNewMap(double** inpMatrix, int inpWidth, int inpHeight, double searchingLvl)
{
    // Проверка размерностей
    if ((inpWidth!=(this->Map_width)-2) || (inpHeight!=(this->Map_height)-2))
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
                this->Map_mtrx[i+1][j+1]=true;
            }else{
                this->Map_mtrx[i+1][j+1]=false;
            }
        }
    }

    return 0; //код отработал исправно
}
/*
int MooreTracing::updateSearchingLvl(double searchingLvl)
{
    //destroyMap();
    //initMap();
    for (int i=0; i<this->Map_width; i++)
    {
        for (int j=0; j<this->Map_width; j++)
        {
                this->Map_mtrx[i][j]=false;
        }
    }
    // Присвоение элементов, соответствующих исследуемой матрице
    for (int i=0; i<this->Map_width-2; i++)
    {
        for (int j=0; j<this->Map_height-2; j++)
        {
            if (this->baseMtrx[i][j]>=searchingLvl)
            {
                this->Map_mtrx[i+1][j+1]=true;
            }else{
                this->Map_mtrx[i+1][j+1]=false;
            }
        }
    }

    return 0; //код отработал исправно
}
*/

///Поиск нового контура, входной параметр - удалить или сохранить полость из зоны поиска, возвращает значение нашел контур или нет
bool MooreTracing::traceNewObj(bool clearCavities)
{
    bool NewTraceFound;
    if (locateNewStartForScanning())  //Просканировать матрицу на наличие объектов
    {   NewTraceFound = true;         //Найден объект
        startTracing();               //Поиск контура объекта
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

///Поиск новой точки входа в цикл поиска, возвращает значение нашел/не нашел
bool MooreTracing::locateNewStartForScanning()
{
    //printMtrx();

    int X = this->prevX0;
    int Y = this->prevY0;
    if (X >= (this->Map_width - 2) )  // -_______0-
    {
        X = 0;
        Y++  ;
    }

    bool pointIsNotFound = true;

    //сначала закончим сканирование по координате X, при этом Y фиксирован
    do{
        X++;
        if(this->Map_mtrx[X][Y])
        {
            pointIsNotFound = false;
            this->prevX0 = X;
            this->prevY0 = Y;
        }
    }while(pointIsNotFound && (X < (this->Map_width - 1)));

    //
    if(pointIsNotFound)
    {
      do{ //сканирование по Y
          X = 0;
          Y++  ;
          do{ //сканирование по X
              X++;
              if(this->Map_mtrx[X][Y])
              {
                  pointIsNotFound = false;
                  this->prevX0 = X;
                  this->prevY0 = Y;
              }
          }while(pointIsNotFound && (X < (this->Map_width - 1)) );//&& (Y < (this->Map_height - 1))требуется правка
      }while(pointIsNotFound &&  (Y < (this->Map_height - 1)));//(X < (this->Map_width - 1)) &&
    }
    return !pointIsNotFound;
}

///Задать точки соседства, начиная с предыдущей исследованной
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
            this->MooreNeibours_Y[0] = keepY;
            this->MooreNeibours_X[0] = E;

            this->MooreNeibours_Y[1] = N;
            this->MooreNeibours_X[1] = E;

            this->MooreNeibours_Y[2] = N;
            this->MooreNeibours_X[2] = keepX;

            this->MooreNeibours_Y[3] = N;
            this->MooreNeibours_X[3] = W;

            this->MooreNeibours_Y[4] = keepY;
            this->MooreNeibours_X[4] = W;

            this->MooreNeibours_Y[5] = S;
            this->MooreNeibours_X[5] = W;

            this->MooreNeibours_Y[6] = S;
            this->MooreNeibours_X[6] = keepX;

            this->MooreNeibours_Y[7] = S;
            this->MooreNeibours_X[7] = E;


        }else if (consY > prevY) //prev SE, next  E
        {            
            this->MooreNeibours_Y[0] = S;
            this->MooreNeibours_X[0] = E;

            this->MooreNeibours_Y[1] = keepY;
            this->MooreNeibours_X[1] = E;

            this->MooreNeibours_Y[2] = N;
            this->MooreNeibours_X[2] = E;

            this->MooreNeibours_Y[3] = N;
            this->MooreNeibours_X[3] = keepX;

            this->MooreNeibours_Y[4] = N;
            this->MooreNeibours_X[4] = W;

            this->MooreNeibours_Y[5] = keepY;
            this->MooreNeibours_X[5] = W;

            this->MooreNeibours_Y[6] = S;
            this->MooreNeibours_X[6] = W;

            this->MooreNeibours_Y[7] = S;
            this->MooreNeibours_X[7] = keepX;


        }else if (consY < prevY) //prev NE, next N
        {
            this->MooreNeibours_Y[0] = N;
            this->MooreNeibours_X[0] = E;

            this->MooreNeibours_Y[1] = N;
            this->MooreNeibours_X[1] = keepX;

            this->MooreNeibours_Y[2] = N;
            this->MooreNeibours_X[2] = W;

            this->MooreNeibours_Y[3] = keepY;
            this->MooreNeibours_X[3] = W;

            this->MooreNeibours_Y[4] = S;
            this->MooreNeibours_X[4] = W;

            this->MooreNeibours_Y[5] = S;
            this->MooreNeibours_X[5] = keepX;

            this->MooreNeibours_Y[6] = S;
            this->MooreNeibours_X[6] = E;

            this->MooreNeibours_Y[7] = keepY;
            this->MooreNeibours_X[7] = E;
        }
    }else if (consX == prevX) //0
    {
        if (consY > prevY)       //prev S , next SE
        {
            this->MooreNeibours_Y[0] = S;
            this->MooreNeibours_X[0] = keepX;

            this->MooreNeibours_Y[1] = S;
            this->MooreNeibours_X[1] = E;

            this->MooreNeibours_Y[2] = keepY;
            this->MooreNeibours_X[2] = E;

            this->MooreNeibours_Y[3] = N;
            this->MooreNeibours_X[3] = E;

            this->MooreNeibours_Y[4] = N;
            this->MooreNeibours_X[4] = keepX;

            this->MooreNeibours_Y[5] = N;
            this->MooreNeibours_X[5] = W;

            this->MooreNeibours_Y[6] = keepY;
            this->MooreNeibours_X[6] = W;

            this->MooreNeibours_Y[7] = S;
            this->MooreNeibours_X[7] = W;

        }else if (consY < prevY) //prev N , next NW
        {            
            this->MooreNeibours_Y[0] = N;
            this->MooreNeibours_X[0] = keepX;

            this->MooreNeibours_Y[1] = N;
            this->MooreNeibours_X[1] = W;

            this->MooreNeibours_Y[2] = keepY;
            this->MooreNeibours_X[2] = W;

            this->MooreNeibours_Y[3] = S;
            this->MooreNeibours_X[3] = W;

            this->MooreNeibours_Y[4] = S;
            this->MooreNeibours_X[4] = keepX;

            this->MooreNeibours_Y[5] = S;
            this->MooreNeibours_X[5] = E;

            this->MooreNeibours_Y[6] = keepY;
            this->MooreNeibours_X[6] = E;

            this->MooreNeibours_Y[7] = N;
            this->MooreNeibours_X[7] = E;
        }
    }else if (consX < prevX) //W
    {
        if ( consY == prevY)     //prev W , next SW
        {
            this->MooreNeibours_Y[0] = keepY;
            this->MooreNeibours_X[0] = W;

            this->MooreNeibours_Y[1] = S;
            this->MooreNeibours_X[1] = W;

            this->MooreNeibours_Y[2] = S;
            this->MooreNeibours_X[2] = keepX;

            this->MooreNeibours_Y[3] = S;
            this->MooreNeibours_X[3] = E;

            this->MooreNeibours_Y[4] = keepY;
            this->MooreNeibours_X[4] = E;

            this->MooreNeibours_Y[5] = N;
            this->MooreNeibours_X[5] = E;

            this->MooreNeibours_Y[6] = N;
            this->MooreNeibours_X[6] = keepX;

            this->MooreNeibours_Y[7] = N;
            this->MooreNeibours_X[7] = W;

        }else if (consY > prevY) //prev SW, next S
        {
            this->MooreNeibours_Y[0] = S;
            this->MooreNeibours_X[0] = W;

            this->MooreNeibours_Y[1] = S;
            this->MooreNeibours_X[1] = keepX;

            this->MooreNeibours_Y[2] = S;
            this->MooreNeibours_X[2] = E;

            this->MooreNeibours_Y[3] = keepY;
            this->MooreNeibours_X[3] = E;

            this->MooreNeibours_Y[4] = N;
            this->MooreNeibours_X[4] = E;

            this->MooreNeibours_Y[5] = N;
            this->MooreNeibours_X[5] = keepX;

            this->MooreNeibours_Y[6] = N;
            this->MooreNeibours_X[6] = W;

            this->MooreNeibours_Y[7] = keepY;
            this->MooreNeibours_X[7] = W;

        }else if (consY < prevY) //prev NW, next  W
        {
            this->MooreNeibours_Y[0] = N;
            this->MooreNeibours_X[0] = W;

            this->MooreNeibours_Y[1] = keepY;
            this->MooreNeibours_X[1] = W;

            this->MooreNeibours_Y[2] = S;
            this->MooreNeibours_X[2] = W;

            this->MooreNeibours_Y[3] = S;
            this->MooreNeibours_X[3] = keepX;

            this->MooreNeibours_Y[4] = S;
            this->MooreNeibours_X[4] = E;

            this->MooreNeibours_Y[5] = keepY;
            this->MooreNeibours_X[5] = E;

            this->MooreNeibours_Y[6] = N;
            this->MooreNeibours_X[6] = E;

            this->MooreNeibours_Y[7] = N;
            this->MooreNeibours_X[7] = keepX;
        }
    }

}

///Проверка окрестностей, р-т 1 = точка-одиночка / 0 = точка не однаж
bool MooreTracing::testNeighborhood()
{ bool isASinglePoint = true;
  int i=-1;

  while(isASinglePoint && (i<7)) //i=-1 0 1 2 3 4 5 6
  {
      i++;                       //i= 0 1 2 3 4 5 6 7
      if(this->Map_mtrx[this->MooreNeibours_X[i]][this->MooreNeibours_Y[i]])
      {
          isASinglePoint = false;
      }
  }



  return isASinglePoint;
}

///Поиск новой точки в окрестностях известной
void MooreTracing::traceNeighborhood()
{ bool isASinglePoint = true;
  int i=0;

  while(isASinglePoint && (i<7)) //i= 0 1 2 3 4 5 6
  {
      i++;                       //i= 1 2 3 4 5 6 7
      if(this->Map_mtrx[this->MooreNeibours_X[i]][this->MooreNeibours_Y[i]])
      {
          isASinglePoint = false;
          this->prevX =this->MooreNeibours_X[i-1];// this->consX;
          this->prevY =this->MooreNeibours_Y[i-1];// this->consY;
          this->consX=this->MooreNeibours_X[i];
          this->consY=this->MooreNeibours_Y[i];
      }
  }

//  if (isASinglePoint) // i = 6
//  {
//      i++; // i = 7
//      if(this->Map_mtrx[this->MooreNeibours_X[i]][this->MooreNeibours_Y[i]])
//      {
//          isASinglePoint = false;
//          this->prevX = this->consX;
//          this->prevY = this->consY;
//          this->consX=this->MooreNeibours_X[i];
//          this->consY=this->MooreNeibours_Y[i];
//      }
//  }

}


///Очистить матрицу от исследованной области
void MooreTracing::clearArea(bool clearCavities)
{
    unsigned int len = this->newTraceX.size();

    int Xmin = this->TraceXmin;
    int Xmax = this->TraceXmax;

    //избавление матрицы от самого контура, чтобы избавиться от одиночек и поиск диапазона значений X
    for (unsigned int i=0; i< len; i++)
    {
        //избавление матрицы от самого контура, чтобы избавиться от одиночек и горизонтальных линий
        this->Map_mtrx[this->newTraceX[i]][this->newTraceY[i]]=false;
    }

    //на случай, если точек меньше 3 - выход из функции
    if (len<=2){
        return;
    }

    //если область шире

    //задаем массив узлов области по списку координат (двумерный)
    std::vector<std::vector<int>> IndexList((Xmax-Xmin+1), std::vector<int>());
    int sgnNext, sgnPrev; //знаки справа и слева от точки (позволяет отделить ветикальные линии и точки

    sgnNext = this->newTraceX[1] - this->newTraceX[0];
    sgnPrev = this->newTraceX[len-2] - this->newTraceX[0];

    if ((sgnNext != sgnPrev))  // РАЗНЫЙ ЗНАК ОЗНАЧАЕТ ТОЧКУ ПОВОРОТА. ОДИНАКОВЫЙ - ГОРИЗОНТАЛЬ И ТОЧКИ. ТОЧКИ УЖЕ ИСКЛЮЧИЛИ
    {
        IndexList[this->newTraceX[0] - Xmin].push_back(0);
    }
    for (unsigned int i=1; i< len-1; i++)
    {
        sgnNext = this->newTraceX[i+1] - this->newTraceX[i];
        sgnPrev = this->newTraceX[i-1] - this->newTraceX[i];

        if ((sgnNext != sgnPrev))  // РАЗНЫЙ ЗНАК ОЗНАЧАЕТ ТОЧКУ ПОВОРОТА. ОДИНАКОВЫЙ - ГОРИЗОТАЛЬ И ТОЧКИ. ТОЧКИ УЖЕ ИСКЛЮЧИЛИ
        {
            IndexList[this->newTraceX[i] - Xmin].push_back(i);
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

            cXval = this->newTraceX[IndexList[I][0]]; //текущая кордината Х
            YvalSize=IndexList[I].size();             //количество узлов для заданного Х

            do{
                notSorted = false;
                for(int j = 0; j < YvalSize - 1; j++)
                {
                    if(this->newTraceY[IndexList[I][j]] > this->newTraceY[IndexList[I][j + 1]])
                    {
                        // меняем элементы местами
                        buffY = IndexList[I][j];
                        IndexList[I][j] = IndexList[I][j + 1];
                        IndexList[I][j + 1] = buffY;
                        notSorted=true;
                    }else if (this->newTraceY[IndexList[I][j]] == this->newTraceY[IndexList[I][j + 1]])
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
                cYval = this->newTraceY[IndexList[I][J]];
                cYval2 = this->newTraceY[IndexList[I][J+1]];

                if(( (cYval+1) < cYval2) && !(this->Map_mtrx[cXval][cYval-1])) // внутри
                {
                    for (int K=cYval+1;K < cYval2; K++)
                    {
                        this->Map_mtrx[cXval][K]=false;
                    }
                }
            }
        }
    }else{
        for (int I=0; I < (Xmax-Xmin+1);I++)
        {
            if (IndexList[I].empty()) continue; //на случай если узел был одинокой точкой

            cXval = this->newTraceX[IndexList[I][0]]; //текущая кордината Х
            YvalSize=IndexList[I].size();             //количество узлов для заданного Х


            do{
                notSorted = false;
                for(int j = 0; j < YvalSize - 1; j++)
                {
                    if(this->newTraceY[IndexList[I][j]] > this->newTraceY[IndexList[I][j + 1]])
                    {
                        // меняем элементы местами
                        buffY = IndexList[I][j];
                        IndexList[I][j] = IndexList[I][j + 1];
                        IndexList[I][j + 1] = buffY;
                        notSorted=true;
                    }else if (this->newTraceY[IndexList[I][j]] == this->newTraceY[IndexList[I][j + 1]])
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
            for(int J=0;J<YvalSize-1;J++) {
                cYval = this->newTraceY[IndexList[I][J]];
                cYval2 = this->newTraceY[IndexList[I][J+1]];

                if(( (cYval+1) < cYval2) && !(this->Map_mtrx[cXval][cYval-1])) // внутри
                {
                    for (int K=cYval+1;K < cYval2; K++)
                    {
                        this->Map_mtrx[cXval][K]=!(this->Map_mtrx[cXval][K]);
                    }
                }
            }
        }
    }

        //Повторное избавление матрицы от самого контура
        for (unsigned int i=0; i< len; i++) {
            //избавление матрицы от самого контура, чтобы избавиться от одиночек и горизонтальных линий
            this->Map_mtrx[this->newTraceX[i]][this->newTraceY[i]]=false;
        }
}



///Поиск внешнего контура
void MooreTracing::startTracing()
{
    clearTrace(); // при повторном считывании следует обнулить контур

    //locateNewStartForScanning(); // НЕ НУЖНО, ЭТА ФУНКЦИЯ СКАНИРУЕТ ПО УЖЕ НАЙДЕННОЙ ОБЛАСТИ

    int startX= this->prevX0;
    int startY= this->prevY0;
    this->consX = startX;
    this->consY = startY;

    this->newTraceX.push_back(startX);
    this->newTraceY.push_back(startY);

    this->TraceXmin=startX;
    this->TraceXmax=startX;
    this->TraceYmin=startY;
    this->TraceYmax=startY;
   // int traceSize = 0;
   // std::cout<<this->newTraceX.size() << '\n';
   // std::cout<<"X,Y = " << this->newTraceX[traceSize] << "  " << this->newTraceY[traceSize] << std::endl;

    setNeighborhood(startX,startY,startX-1,startY);

    if (testNeighborhood()) return; // Если точка одна в котуре - возврат

    traceNeighborhood();
    this->newTraceX.push_back(this->consX);
    this->newTraceY.push_back(this->consY);
    updateLimits();

   // traceSize++;
   // std::cout<<this->newTraceX.size() << '\n';
   // std::cout<<"X,Y = " << this->newTraceX[traceSize] << "  " << this->newTraceY[traceSize] << std::endl;
    int attempt=1;
    int I[3]{};
    while(attempt<=2)
    {
        setNeighborhood(this->consX,this->consY,this->prevX,this->prevY);
        traceNeighborhood();
        this->newTraceX.push_back(this->consX);
        this->newTraceY.push_back(this->consY);
        updateLimits();

        if(((this->newTraceX[0] == this->consX) && (this->newTraceY[0] == this->consY)))
        {
            I[attempt]=newTraceX.size()-1;
            attempt++;
        }
    //    traceSize++;
    //    std::cout<<this->newTraceX.size() << '\n';
    //    std::cout<<"X,Y = " << this->newTraceX.at(traceSize) << "  " << this->newTraceY.at(traceSize) << std::endl;
    }

    if((this->newTraceX[I[0]+1] == this->newTraceX[I[1]+1]) &&
       (this->newTraceY[I[0]+1] == this->newTraceY[I[1]+1]) &&
       (this->newTraceX[I[0]+2] == this->newTraceX[I[1]+2]) &&
       (this->newTraceY[I[0]+2] == this->newTraceY[I[1]+2]) )
    {
        newTraceX.resize(I[1]+1);
        newTraceY.resize(I[1]+1);
    }
}

///Сравнение величин и задание лимитов
void MooreTracing::updateLimits()
{
    // поиск диапазона значений X
    if(this->consX < this->TraceXmin)
    {
        this->TraceXmin = this->consX;
    }else if (this->consX > this->TraceXmax)
    {
         this->TraceXmax = this->consX;
    }

    // поиск диапазона значений Y
    if(this->consY < this->TraceYmin)
    {
        this->TraceYmin = this->consY;
    }else if (this->consY > this->TraceYmax)
    {
         this->TraceYmax = this->consY;
    }
}

/// Очистка векторов от данных
void MooreTracing::clearTrace()
{
   // std::cout << "TraceSize = " << this->newTraceX.size() << std::endl;

    this->newTraceX.clear();
    this->newTraceY.clear();

   // std::cout << "TraceSize = " << this->newTraceX.size() << std::endl;
}

std::vector<int> MooreTracing::getNewTraceX()
{
    int size =(this->newTraceX).size();
    std::vector<int> res (size);
    for (int i=0; i<size; i++)
    {
      res[i]=  (this->newTraceX)[i]-1;
    }
    return res;
}
std::vector<int> MooreTracing::getNewTraceY()
{
    int size =(this->newTraceY).size();
     std::vector<int> res (size);
     for (int i=0; i<size; i++)
     {
       res[i]=  (this->newTraceY)[i]-1;
     }
    return res;
}
int MooreTracing::getTraceXmin()
{
    return this->TraceXmin;
}
int MooreTracing::getTraceXmax()
{
    return this->TraceXmax;
}
int MooreTracing::getTraceYmin()
{
    return this->TraceYmin;
}
int MooreTracing::getTraceYmax()
{
    return this->TraceYmax;
}

void MooreTracing::printMtrx()
{
    int v;    
    for (int j = 0; j < this->Map_height; j++)
    {
        std::cout << std::endl;
        for (int i = 0; i < this->Map_width; i++)
        {
            if (this->Map_mtrx[i][j] == true)
            {
                v=1;
            }else{
                v=0;
            }


            std::cout << v << "\t";
        }

    }
}
