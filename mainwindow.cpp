#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <string>
#include <iostream>
#include <fstream>
#include <QAction>

#include "mooretracing.h"

#include <QDir>
#include <QFileDialog>

//#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //==================================================
    ///=============Настройка директорий================
    //==================================================
    ///
    QString dirname = QDir::currentPath();
    this->rootFolder=dirname;
    ui -> statusbar -> showMessage(dirname);
    ui -> statusbar -> update();


    //==================================================
    ///======Настройка взаимодействий с графиками=======
    //==================================================
    ///
    ui -> Axes_plane -> setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui -> Axes_curves-> setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    setAxesSize(ui->Axes_plane, 600, 600);
    setAxesSize(ui->Axes_curves, 600, 600);

    QObject::connect(ui->Axes_curves, SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)),
                     this, SLOT(axisLabelDoubleClick(QCPAxis*,QCPAxis::SelectablePart)));
    QObject::connect(ui->Axes_plane, SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)),
                     this, SLOT(axisLabelDoubleClick(QCPAxis*,QCPAxis::SelectablePart)));

    QObject::connect(ui->Axes_curves, SIGNAL(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)),
                     this, SLOT(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*)));




    //==================================================
    ///=========Инициализация работы слайдеров==========
    //==================================================
    ///
    setSliders();

    QObject::connect(ui->X_Slider, SIGNAL(valueChanged(int)),
                     ui->X_val   , SLOT  (setNum(int)));
    QObject::connect(ui->Y_Slider, SIGNAL(valueChanged(int)),
                     ui->Y_val   , SLOT  (setNum(int)));
    QObject::connect(ui->Z_Slider, SIGNAL(valueChanged(int)),
                     ui->Z_val   , SLOT  (setNum(int)));

    QObject::connect(ui->X_Slider, SIGNAL(valueChanged(int)),
                     this        , SLOT  (updateCurves()));
    QObject::connect(ui->Y_Slider, SIGNAL(valueChanged(int)),
                     this        , SLOT  (updateCurves()));
    QObject::connect(ui->Z_Slider, SIGNAL(valueChanged(int)),
                     this        , SLOT  (updateCurves()));

    QObject::connect(ui->X_Slider, SIGNAL(valueChanged(int)),
                     this        , SLOT  (updatePlane()));
    QObject::connect(ui->Y_Slider, SIGNAL(valueChanged(int)),
                     this        , SLOT  (updatePlane()));
    QObject::connect(ui->Z_Slider, SIGNAL(valueChanged(int)),
                     this        , SLOT  (updatePlane()));



    //==================================================
    ///====Инициализация работы кнопок переключения=====
    //==================================================
    ///
    // Выбор плоскости
    QObject::connect(ui->plane_XY_rb, SIGNAL(clicked()),
                     this           , SLOT  (updatePlane()));
    QObject::connect(ui->plane_XZ_rb, SIGNAL(clicked()),
                     this           , SLOT  (updatePlane()));
    QObject::connect(ui->plane_YZ_rb, SIGNAL(clicked()),
                     this           , SLOT  (updatePlane()));

    // Выбор стиля
    QObject::connect(ui->CMap_rb, SIGNAL(clicked()),
                     this       , SLOT  (updatePlane()));
    QObject::connect(ui->ISOCMap_rb, SIGNAL(clicked()),
                     this           , SLOT  (updatePlane()));
    QObject::connect(ui->Isodoses_rb, SIGNAL(clicked()),
                     this           , SLOT  (updatePlane()));
    // Выбор типа графика
    QObject::connect(ui->Dz_chB, SIGNAL(stateChanged(int)),
                     this      , SLOT  (updateCurves()));
    QObject::connect(ui->Dx_chB, SIGNAL(stateChanged(int)),
                     this      , SLOT  (updateCurves()));
    QObject::connect(ui->Dy_chB, SIGNAL(stateChanged(int)),
                     this      , SLOT  (updateCurves()));
    QObject::connect(ui->Dz_total_chB, SIGNAL(stateChanged(int)),
                     this      , SLOT  (updateCurves()));


    //==================================================
    ///====Ограничения на значения вводимых значений====
    //==================================================
    ///
    // Set isodose line edits restrictions
    const int N_lvls=9;
    QLineEdit* isodose_list[N_lvls] = {ui->isodose_lvl_1,
                                       ui->isodose_lvl_2,
                                       ui->isodose_lvl_3,
                                       ui->isodose_lvl_4,
                                       ui->isodose_lvl_5,
                                       ui->isodose_lvl_6,
                                       ui->isodose_lvl_7,
                                       ui->isodose_lvl_8,
                                       ui->isodose_lvl_9 };
    for(int i=0;i<N_lvls;i++)
    {
        isodose_list[i]-> setValidator(new QDoubleValidator(0,200,3,this));
    }


    //==================================================
    ///================Коррекция цвета==================
    //==================================================
    ///
    // Изменение цветов панелей изодоз (легенда)
    setColorPanels();

    QString setBackgroundColor;
    setBackgroundColor="background-color: rgb( 255, 255, 255);";
    ui->X_val->setAutoFillBackground(true);
    ui->X_val->setStyleSheet(setBackgroundColor);
    ui->Y_val->setAutoFillBackground(true);
    ui->Y_val->setStyleSheet(setBackgroundColor);
    ui->Z_val->setAutoFillBackground(true);
    ui->Z_val->setStyleSheet(setBackgroundColor);


    //==================================================
    ///===============Настройка кнопок==================
    //==================================================
    ///
    QObject::connect(ui->updateIsodoses , SIGNAL(clicked()),
                     this               , SLOT  (updatePlane()));
    QObject::connect(ui->actionOpenDoseFile , SIGNAL(triggered()),
                     this                   , SLOT  (openFile()));

    //SaveButtons
    QObject::connect(ui -> actionSave_plane_graph   , SIGNAL(triggered()),
                     this                           , SLOT  (SavePlane()));

    QObject::connect(ui -> actionSave_curve_graph   , SIGNAL(triggered()),
                     this                           , SLOT  (SaveCurves()));

    //Задание кнопок переключения режима считывания
    QObject::connect(ui->actionSRNA, SIGNAL(triggered()),
                     this          , SLOT  (setFileFormatSRNA()));

    QObject::connect(ui->actionGEANT4, SIGNAL(triggered()),
                     this            , SLOT  (setFileFormatGEANT4()));

    // Задание кнопок сохранения данных
    QObject::connect(ui->actionSaveDx, SIGNAL(triggered()),
                     this            , SLOT  (saveDx()));
    QObject::connect(ui->actionSaveDy, SIGNAL(triggered()),
                     this            , SLOT  (saveDy()));
    QObject::connect(ui->actionSaveDz, SIGNAL(triggered()),
                     this            , SLOT  (saveDz()));
    QObject::connect(ui->actionSave_global_D_z, SIGNAL(triggered()),
                     this            , SLOT  (saveGlobalDz()));


}

MainWindow::~MainWindow()
{
    delete[] colorList;

    delete ui;
}

//==================================================
///===========Настройки окна приложения=============
//==================================================
// Слоты: ==========================================
void MainWindow::setSliders()
{
    ui -> X_Slider ->setMinimum(0);
    ui -> X_Slider ->setMaximum((this->sizeX)-1);
    ui -> X_Slider ->setSingleStep(1);

    ui -> Y_Slider ->setMinimum(0);
    ui -> Y_Slider ->setMaximum((this->sizeY)-1);
    ui -> Y_Slider ->setSingleStep(1);

    ui -> Z_Slider ->setMinimum(0);
    ui -> Z_Slider ->setMaximum((this->sizeZ)-1);
    ui -> Z_Slider ->setSingleStep(1);

    int X_value = round(0.5*(this->sizeX));
    int Y_value = round(0.5*(this->sizeY));
    int Z_value = round(0.5*(this->sizeZ));

    ui -> X_val ->setNum(X_value);
    ui -> Y_val ->setNum(Y_value);
    ui -> Z_val ->setNum(Z_value);

    ui -> X_Slider ->setValue(X_value);
    ui -> Y_Slider ->setValue(Y_value);
    ui -> Z_Slider ->setValue(Z_value);
}

void MainWindow::legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item)
{
  // Rename a graph by double clicking on its legend item
  Q_UNUSED(legend)
  if (item) // only react if item was clicked (user could have clicked on border padding of legend where there is no item, then item is 0)
  {
    QCPPlottableLegendItem *plItem = qobject_cast<QCPPlottableLegendItem*>(item);
    bool ok;
    QString newName = QInputDialog::getText(this, "Change legend label", "New graph name:", QLineEdit::Normal, plItem->plottable()->name(), &ok);
    if (ok)
    {
      plItem->plottable()->setName(newName);
      legend->parentPlot()->replot();
      //ui->Axes_plane->replot();
    }
  }
}

void MainWindow::axisLabelDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part)
{

    //QCustomPlot* A = ui->Axes_plane;
    //QCustomPlot* B = ui->Axes_curves;

  // Set an axis label by double clicking on it
  if (part == QCPAxis::spAxisLabel) // only react when the actual axis label is clicked, not tick label or axis backbone
  {
    bool ok;
    QString newLabel = QInputDialog::getText(this, "Change axes label", "New axis label:", QLineEdit::Normal, axis->label(), &ok);
    if (ok)
    {
      axis->setLabel(newLabel);
      axis->parentPlot()->replot();
      //ui->Axes_plane->replot();
      //ui->Axes_curves->replot();
    }
  }
}

// Методы: ==========================================

void MainWindow::setColorPanels()
{
    const int N_lvls=9;
    QLineEdit* isodose_list[N_lvls] = {ui->isodose_lvl_1,
                                      ui->isodose_lvl_2,
                                      ui->isodose_lvl_3,
                                      ui->isodose_lvl_4,
                                      ui->isodose_lvl_5,
                                      ui->isodose_lvl_6,
                                      ui->isodose_lvl_7,
                                      ui->isodose_lvl_8,
                                      ui->isodose_lvl_9 };

    double isodose_lvl_num[N_lvls]{};

     //Нормировка на единицу
    for(int lvl=0;lvl<N_lvls;lvl++)
    {
        isodose_lvl_num[lvl]=(isodose_list[lvl]->text().toDouble())/100;
    }



    QCPColorGradient jetCM;
    jetCM.loadPreset(QCPColorGradient::gpJet);
    jetCM.setLevelCount(1000);


    QLabel* isodose_lbl_list[N_lvls] = {ui->cmap_lvl_1,
                                        ui->cmap_lvl_2,
                                        ui->cmap_lvl_3,
                                        ui->cmap_lvl_4,
                                        ui->cmap_lvl_5,
                                        ui->cmap_lvl_6,
                                        ui->cmap_lvl_7,
                                        ui->cmap_lvl_8,
                                        ui->cmap_lvl_9 };

    this->colorList = new QRgb[N_lvls];
    QString setBackgroundColor;
    for (int i=0;i<N_lvls;i++)
    {
      this->colorList[i] = jetCM.color(isodose_lvl_num[i],QCPRange(0, 1),false);
      setBackgroundColor="background-color: rgb(" +
              QString::number(qRed  (this->colorList[i])) + ", " +
              QString::number(qGreen(this->colorList[i])) + ", " +
              QString::number(qBlue (this->colorList[i])) + ");";
      isodose_lbl_list[i]->setAutoFillBackground(true);
      isodose_lbl_list[i]->setStyleSheet(setBackgroundColor);
    }
    //delete[] colorList;
    //colorList = nullptr;

}

void MainWindow::setAxesSize(QCustomPlot* axes, int WidthMultiplier, int HeightMultiplier)
{
    int minimumWidth  = axes->minimumWidth();
    int minimumHeight = axes->minimumHeight();

    int baseWidth=1;
    int baseHeight =1;

    int nWidth = baseWidth *  WidthMultiplier;
    int nHeight = baseHeight *  HeightMultiplier;

    if ( minimumWidth > nWidth)
    {
        axes->setMinimumWidth(nWidth);
    }

    if ( minimumHeight > nHeight)
    {
        axes->setMinimumHeight(nHeight);
    }

    axes -> setGeometry(axes->pos().x(), axes->pos().y(), nWidth, nHeight);

}

//==================================================
///===============Работа с файлами==================
//==================================================
// Слоты: ==========================================
void MainWindow::setFileFormatSRNA()
{
   bool chB_GEANT4_on = ui->actionGEANT4->isChecked();
   ui->actionGEANT4->setChecked(!chB_GEANT4_on);
}
void MainWindow::setFileFormatGEANT4()
{
   bool chB_SRNA_on   = ui->actionSRNA  ->isChecked();
   ui->actionSRNA->setChecked(!chB_SRNA_on);
}

void MainWindow::openFile()
{
    if (this->loadlock) return;

    // Выбор заданной директории
    QString locstr = ui -> statusbar->currentMessage();

    // Диалоговое окно выбора файла
    /// При использовании кириллицы в имени есть проблемы чтения файла
     QString filter = "*.dat *.DAT";
     QString filename =   QFileDialog::getOpenFileName(0, "Set dose distribution", locstr, filter);//, 0, QFileDialog::DontUseNativeDialog) ;

     if (filename == "")
         return;

     QChar* charArr = filename.data();
     int stringSize = filename.size()-1;
     int num= filename.size()-1;

     while(charArr[num] != '/')
     {   num--;
         if( num<0)
         {
             qDebug() << "Error while loading\n";
             return;
         }
     }

     num++;
     QChar charName[stringSize-num+2];
     for (int i=num;i<=stringSize;++i)
     {
         charName[i-num]= charArr[i];
     }
     QString FN(charName);
     ui -> actionFileName->setText(FN);// fileName_label->setText(FN);

     QChar charfolderName[num+1];
     for (int i=0;i<num;i++)
     {
         charfolderName[i]= charArr[i];
     }

     ui -> statusbar->showMessage("LOADING...");
     ui -> statusbar -> update();

     this->loadlock = true;

     loadNewDoseFile(filename);
     this->DistributionIsSet=true;
     this->loadlock = false;

     QString qstr_folderName(charfolderName);
     this->folderLocation=qstr_folderName;

     ui -> statusbar->showMessage(qstr_folderName);
     ui -> statusbar -> update();

     setSliders();

     updateCurves();
     updatePlane();
}

void MainWindow::SavePlane()
{
    QString locstr = ui -> statusbar->currentMessage();
    QString filter = "*.jpg";

    QString filename = QFileDialog::getSaveFileName(0, "Save graphs", locstr, filter);
    //QString fileName("C:/example/customPlot.png");

    QFile file(filename);

    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << file.errorString();
    } else {
        ui -> Axes_plane->saveJpg(filename);
    }
}

void MainWindow::SaveCurves()
{
    QString locstr = ui -> statusbar->currentMessage();
    QString filter = "*.jpg";

    QString filename = QFileDialog::getSaveFileName(0, "Save graphs", locstr, filter);
    //QString fileName("C:/example/customPlot.png");

    QFile file(filename);

    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << file.errorString();
    } else {
        ui -> Axes_curves->saveJpg(filename);
    }

}

void MainWindow::saveDx()
{
    if (DistributionIsSet)
    {
        //Определение текущих координат
        //int X = ui->X_val->text().toInt();
        int Y = ui->Y_val->text().toInt();
        int Z = ui->Z_val->text().toInt();

        QVector<double> x(this->sizeX), Dx(this->sizeX); // initialize with entries 0..100
        for (int i=0; i<this->sizeX; ++i)
        {
          x[i] = i;
          Dx[i] = (DoseDistr.element(i,Y,Z));
        }

        QString filename = this->folderLocation + "D(x,"+QString::number(Y)+","+QString::number(Z)+").txt";
        printDose2File(filename, "X", Y, Z, x, Dx);
    }

}
void MainWindow::saveDy()
{
    if (DistributionIsSet)
    {
        //Определение текущих координат
        int X = ui->X_val->text().toInt();
        //int Y = ui->Y_val->text().toInt();
        int Z = ui->Z_val->text().toInt();

        QVector<double> y(this->sizeY), Dy(this->sizeY); // initialize with entries 0..100
        for (int i=0; i<this->sizeY; ++i)
        {
          y[i] = i;
          Dy[i] = (DoseDistr.element(X,i,Z)); // let's plot a quadratic function
        }
        QString filename = this->folderLocation + "D("+QString::number(X)+",y,"+QString::number(Z)+").txt";
        printDose2File(filename, "Y", X, Z, y, Dy);
    }
}
void MainWindow::saveDz()
{
    if (DistributionIsSet)
    {
        //Определение текущих координат
        int X = ui->X_val->text().toInt();
        int Y = ui->Y_val->text().toInt();
       // int Z = ui->Z_val->text().toInt();

        QVector<double> z(this->sizeZ);
        QVector<double> Dz(this->sizeZ); // initialize with entries 0..100
        for (int i=0; i<this->sizeZ; ++i)
        {
          z[i] = i; // x goes from -1 to 1
          Dz[i] = (DoseDistr.element(X,Y,i)); // let's plot a quadratic function
        }

        QString filename = this->folderLocation + "D("+QString::number(X)+","+QString::number(Y)+",z).txt";
        printDose2File(filename, "Z", X, Y, z, Dz);

    }
}

void MainWindow::saveGlobalDz()
{
    if (DistributionIsSet)
    {
        QVector<double> z(this->sizeZ);
        QVector<double> Dz(this->sizeZ,0.0); // initialize with entries 0..100

        for (int i=0; i<this->sizeZ; ++i)
        {
          z[i] = i; // x goes from -1 to 1
        }

        for (int z=0; z<this->sizeZ; ++z)
        {
            for(int X=0; X<this->sizeX; ++X){
                for(int Y=0; Y<this->sizeY; ++Y){
                    Dz[z] += (DoseDistr.element(X,Y,z)); // let's plot a quadratic function
                }
            }
        }
        QString filename = this->folderLocation + "D(z).txt";
        printDose2File(filename, "Z", -999, -999, z, Dz);

    }

}


// Методы: ==========================================
void MainWindow::loadNewDoseFile(QString filename)
{
   // this->DoseDestr = new DoseDistribution(filename.toStdString());
    bool FileFormSRNA = ui->actionSRNA->isChecked();
    bool FileFormGEANT4 = ui->actionGEANT4->isChecked();

    QString FileFormat;
    if(FileFormSRNA){
        FileFormat = "SRNA";
    } else if (FileFormGEANT4){
        FileFormat = "GEANT4";
    } else {
        return;
    }
    //this->DoseDestr = new DoseDistribution(filename, FileFormat);
    DoseDistr = DoseVector(filename, FileFormat);
    sizeX = DoseDistr.getXSize();
    sizeY = DoseDistr.getYSize();
    sizeZ = DoseDistr.getZSize();
}

void MainWindow::printDose2File(QString FileName,QString Direction, int coordinate1, int coordinate2, QVector<double> direction, QVector<double> Dose)
{
    //QString FileName =this->folderLocation + "D("+Direction+").txt";

    QFile newFile(FileName);
    newFile.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text);
    QTextStream newData(&newFile);

    QString strCoord1, strCoord2;
    if (coordinate1<0){
       strCoord1 = "total";
    }else
    {
       strCoord1 = " = "+ QString::number(coordinate1);
    }

    if (coordinate2<0){
       strCoord2 = "total";
    }else
    {
       strCoord2 = " = "+ QString::number(coordinate2);
    }

    if (Direction == "X")
    {
        newData << "Y" << strCoord1 << " vox.u. \n";
        newData << "Z" << strCoord2 << " vox.u. \n";
        newData << "X, vox.u.\t Dose\n";
    }else if (Direction == "Y")
    {
        newData << "X" << strCoord1 << " vox.u. \n";
        newData << "Z" << strCoord2 << " vox.u. \n";
        newData << "Y, vox.u.\t Dose\n";
    }else if (Direction == "Z")
    {
        newData << "X" << strCoord1 << " vox.u. \n";
        newData << "Y" << strCoord2 << " vox.u. \n";
        newData << "Z, vox.u.\t Dose\n";
    }else {return;}

    for(int i=0; i < Dose.size(); i++)
    {
       newData << direction[i] << "\t";
       newData.setRealNumberNotation(QTextStream::ScientificNotation);
       newData << Dose[i] <<'\n';
       newData.setRealNumberNotation(QTextStream::RealNumberNotation());
//       newData.setRealNumberNotation(QTextStream::RealNumberNotation);
    }
    newFile.close();
}


//==================================================
///=============Построение графиков:================
//==================================================
// Слоты: ==========================================
void MainWindow::updateCurves()
{

    //Если в памяти уже есть распределене дозы
    if (DistributionIsSet)
    {
        //Определение какие графики следует построить
        int Dx_rb_val = ui->Dx_chB->checkState();
        int Dy_rb_val = ui->Dy_chB->checkState();
        int Dz_rb_val = ui->Dz_chB->checkState();
        int Dz_total_val = ui->Dz_total_chB->checkState();

        //Инициализация осей и очистка от старых графиков
        int N = ui->Axes_curves->graphCount();
        if (N<this->curvesMaximum)
        {
            for(int i=N-1;i<this->curvesMaximum;i++)
            {
                ui->Axes_curves->addGraph();
            }
        }

          ui -> Axes_curves->clearGraphs();
          ui -> Axes_curves -> legend ->clearItems();
          ui -> Axes_curves -> setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom)); // period as decimal separator and comma as thousand separator
          ui -> Axes_curves -> legend->setVisible(false);
          //ui -> Axes_curves -> legend ->setInteraction(QCP::iRangeDrag, true);

          QPen LinePenBlack, LinePenRed, LinePenBlue;
          LinePenBlack.setWidth(5);
          LinePenBlack.setWidthF(3);
          LinePenBlack.setColor(Qt::black);

          LinePenRed.setWidth(5);
          LinePenRed.setWidthF(3);
          LinePenRed.setColor(Qt::red);

          LinePenBlue.setWidth(5);
          LinePenBlue.setWidthF(3);
          LinePenBlue.setColor(Qt::blue);

        //Определение текущих координат
        int X = ui->X_val->text().toInt();
        int Y = ui->Y_val->text().toInt();
        int Z = ui->Z_val->text().toInt();

        //Параметры нормировки о область построения
        double DoseMax=DoseDistr.getDoseMaxValue();
        int xLimits[3]{};

        //Считчик количества графиков (для построения легенды)
        int graphCounter=-1;

        if(Dz_rb_val==2)
        {
            QVector<double> z(this->sizeZ), Dz(this->sizeZ); // initialize with entries 0..100
            for (int i=0; i<this->sizeZ; ++i)
            {
              z[i] = i; // x goes from -1 to 1
              Dz[i] = (DoseDistr.element(X,Y,i))/DoseMax; // let's plot a quadratic function
            }
            xLimits[2]=(this->sizeZ)-1;

            graphCounter++;
            ui -> Axes_curves -> addGraph();
            ui -> Axes_curves -> graph(graphCounter)->setName("D(z)");
            // Построение гафика #M
            ui -> Axes_curves -> graph(graphCounter)->setData(z, Dz);
            ui -> Axes_curves -> graph(graphCounter)->setPen(LinePenBlack);
            ui -> Axes_curves -> graph(graphCounter)->setLineStyle(QCPGraph::lsStepLeft);  // Задание типа линии (lsStepCenter - ступенчатый)
        }

        if(Dx_rb_val==2)
        {

            QVector<double> x(this->sizeX), Dx(this->sizeX); // initialize with entries 0..100
            for (int i=0; i<this->sizeX; ++i)
            {
              x[i] = i;
              Dx[i] = (DoseDistr.element(i,Y,Z))/DoseMax;
            }
            xLimits[0]=(this->sizeX)-1;

            graphCounter++;
            ui -> Axes_curves -> addGraph();
            ui -> Axes_curves -> graph(graphCounter)->setName("D(x)");
            // Построение гафика #M
            ui -> Axes_curves -> graph(graphCounter)->setData(x, Dx);
            ui -> Axes_curves -> graph(graphCounter)->setPen(LinePenRed);
            ui -> Axes_curves -> graph(graphCounter)->setLineStyle(QCPGraph::lsStepLeft);  // Задание типа линии (lsStepCenter - ступенчатый)
        }

        if(Dy_rb_val==2)
        {
            QVector<double> y(this->sizeY), Dy(this->sizeY); // initialize with entries 0..100
            for (int i=0; i<this->sizeY; ++i)
            {
              y[i] = i;
              Dy[i] = (DoseDistr.element(X,i,Z))/DoseMax; // let's plot a quadratic function
            }
            xLimits[1]=(this->sizeY)-1;

            graphCounter++;
            ui -> Axes_curves -> addGraph();
            ui -> Axes_curves -> graph(graphCounter)->setName("D(y)");
            ui -> Axes_curves -> graph(graphCounter)->setData(y, Dy);                         // Передача данных на график
            ui -> Axes_curves -> graph(graphCounter)->setPen(LinePenBlue);
            ui -> Axes_curves -> graph(graphCounter)->setLineStyle(QCPGraph::lsStepLeft);  // Задание типа линии (lsStepCenter - ступенчатый)
        }

        if(Dz_total_val == 2){

            QVector<double> z(this->sizeZ);
            QVector<double> TDz(this->sizeZ,0.0); // initialize with entries 0..100
            double totalMaximum=0;

            for (int i=0; i<this->sizeZ; ++i)
            {
              z[i] = i; // x goes from -1 to 1
            }

            for (int z=0; z<this->sizeZ; ++z)
            {
                for(int X=0; X<this->sizeX; ++X){
                    for(int Y=0; Y<this->sizeY; ++Y){
                        TDz[z] += (DoseDistr.element(X,Y,z)); // let's plot a quadratic function
                        if(TDz[z]>totalMaximum)
                        {totalMaximum=TDz[z];}
                    }
                }
            }
            for (int z=0; z<this->sizeZ; ++z)
            {
                TDz[z] /= totalMaximum;
            }

            xLimits[2]=(this->sizeZ)-1;

            graphCounter++;
            ui -> Axes_curves -> addGraph();
            ui -> Axes_curves -> graph(graphCounter)->setName("D(z)");
            // Построение гафика #M
            ui -> Axes_curves -> graph(graphCounter)->setData(z, TDz);
            ui -> Axes_curves -> graph(graphCounter)->setPen(LinePenBlack);
            ui -> Axes_curves -> graph(graphCounter)->setLineStyle(QCPGraph::lsStepLeft);  // Задание типа линии (lsStepCenter - ступенчатый)
        }

        int xLimit=0;
        for (int i=0;i<3;i++)
        {
            if(xLimits[i]>xLimit)
            {    xLimit=xLimits[i];}
        }

        ui -> Axes_curves -> xAxis->setRange(0, xLimit);
        ui -> Axes_curves -> yAxis->setRange(0, 1.0);
        ui -> Axes_curves -> yAxis->ticker()->setTickCount(10);

        QString xAxlabel="";
        QString delim="/ ";
        bool wehavegraph=false;

        if (Dx_rb_val == 2)
        {
            xAxlabel += "X ";
            wehavegraph = true;
        }
        if (Dy_rb_val == 2)
        {
            if(wehavegraph) xAxlabel += delim;

            xAxlabel += "Y ";
            wehavegraph = true;
        }
        if ((Dz_rb_val == 2)||(Dz_total_val == 2))
        {
            if(wehavegraph) xAxlabel += delim;

            xAxlabel += "Z ";
            wehavegraph = true;
        }
        if ((Dx_rb_val == 2)||(Dy_rb_val == 2)||(Dz_rb_val == 2)||(Dz_total_val == 2))
        {
          xAxlabel += ", voxels";

          ui -> Axes_curves -> legend->setVisible(true);
          QFont legendFont = font();  // start out with MainWindow's font..
          legendFont.setPointSize(14); // and make a bit smaller for legend
          ui -> Axes_curves -> legend->setFont(legendFont);
          ui -> Axes_curves -> legend->setBrush(QBrush(QColor(255,255,255,230)));
          // by default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement:
          ui -> Axes_curves -> axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignLeft);

        }

        QFont labelFont = font();
        labelFont.setPointSize(14);
        ui -> Axes_curves -> xAxis->setLabelFont(labelFont);
        ui -> Axes_curves -> yAxis->setLabelFont(labelFont);

        ui -> Axes_curves -> xAxis->setTickLabelFont(labelFont);
        ui -> Axes_curves -> yAxis->setTickLabelFont(labelFont);

        ui -> Axes_curves -> xAxis->setLabel(xAxlabel);
        ui -> Axes_curves -> yAxis->setLabel("Dose, rel.u.");

        ui -> Axes_curves -> replot();
    }
}

void MainWindow::updatePlane()
{
    setColorPanels();
    ui->Axes_plane->yAxis->setRangeReversed(true);


    if (DistributionIsSet)
    {

        //очистка осей от старых графиков
        ui -> Axes_plane->clearPlottables();
        ui -> Axes_plane -> clearItems();
        ui -> Axes_plane -> clearGraphs();

        if(ui->CMap_rb->isChecked())//
        {
           if(CMap()==1) return;
        }else if(ui->ISOCMap_rb->isChecked())
        {
           if(ISOCMap()==1) return;
        }else if(ui->Isodoses_rb->isChecked())
        {
            if(ISOdoses()==1) return;
        }

        QFont labelFont = font();
        labelFont.setPointSize(14);
        ui -> Axes_plane -> xAxis->setLabelFont(labelFont);
        ui -> Axes_plane -> yAxis->setLabelFont(labelFont);

        //changeAxesProportion(ui->Axes_curves, 300, 600);
        //setAxesSize(ui->Axes_plane, 1000, 300);
        //setAxesSize(ui->Axes_curves, 700, 700);

    }
}

// Методы: ==========================================
int MainWindow::CMap()
{
    int X = ui->X_val->text().toInt();
    int Y = ui->Y_val->text().toInt();
    int Z = ui->Z_val->text().toInt();
    double DoseMax=DoseDistr.getDoseMaxValue();

    QCPColorMap *colorMap = new QCPColorMap(ui->Axes_plane->xAxis, ui->Axes_plane->yAxis);

    QString xAxlabel="", yAxlabel = "";

    if(ui->plane_XY_rb->isChecked())
    {
        colorMap->data()->setSize(this->sizeX, this->sizeY);
        colorMap->data()->setRange(QCPRange(0, this->sizeX), QCPRange(0, this->sizeY));
        for (int x=0; x<this->sizeX; ++x)
        {
            for (int y=0; y<this->sizeY; ++y)
            {
                colorMap->data()->setCell(x, y, (DoseDistr.element(x,y,Z))/DoseMax);
            }
        }

        int MaxInd=0, xShift=0, yShift=0;
        if(this->sizeX>this->sizeY)
        {
            MaxInd=this->sizeX;
            yShift = 0.5*((this->sizeX) - (this->sizeY));
        }else
        {
            MaxInd=this->sizeY;
            xShift = 0.5*((this->sizeY) - (this->sizeX));
        }
        ui->Axes_plane->xAxis->setRange(0-xShift,MaxInd-xShift);
        ui->Axes_plane->yAxis->setRange(0-yShift,MaxInd-yShift);

        xAxlabel="X, vox.u.", yAxlabel = "Y, vox.u.";

    }else if(ui->plane_XZ_rb->isChecked())
    {
        colorMap->data()->setSize(this->sizeZ, this->sizeX);
        colorMap->data()->setRange(QCPRange(0, this->sizeZ), QCPRange(0, this->sizeX));
        for (int z=0; z<this->sizeZ; ++z)
        {
            for (int x=0; x<this->sizeX; ++x)
            {
                colorMap->data()->setCell(z, x, (DoseDistr.element(x,Y,z))/DoseMax);
            }
        }

        int MaxInd=0, xShift=0, yShift=0;
        if(this->sizeX>this->sizeZ)
        {
            MaxInd = this->sizeX;
            xShift = 0.5*((this->sizeX) - (this->sizeZ));
        }else
        {
            MaxInd = this->sizeZ;
            yShift = 0.5*((this->sizeZ) - (this->sizeX));
        }
        ui->Axes_plane->xAxis->setRange(0-xShift,MaxInd-xShift);
        ui->Axes_plane->yAxis->setRange(0-yShift,MaxInd-yShift);

        xAxlabel="Z, vox.u.", yAxlabel = "X, vox.u.";

    }else if(ui->plane_YZ_rb->isChecked())
    {
        colorMap->data()->setSize(this->sizeZ, this->sizeY);
        colorMap->data()->setRange(QCPRange(0, this->sizeZ), QCPRange(0, this->sizeY));
        for (int z=0; z<this->sizeZ; ++z)
        {
            for (int y=0; y<this->sizeX; ++y)
            {
                colorMap->data()->setCell(z, y, (DoseDistr.element(X,y,z))/DoseMax);
            }
        }

        int MaxInd=0, xShift=0, yShift=0;
        if(this->sizeZ>this->sizeY)
        {
            MaxInd=this->sizeZ;
            yShift = 0.5*((this->sizeZ) - (this->sizeY));
        }else
        {
            MaxInd=this->sizeY;
            xShift = 0.5*((this->sizeZ) - (this->sizeY));
        }
        ui->Axes_plane->xAxis->setRange(0-xShift,MaxInd-xShift);
        ui->Axes_plane->yAxis->setRange(0-yShift,MaxInd-yShift);

        xAxlabel="Z, vox.u.", yAxlabel = "Y, vox.u.";

    }else return 1;

    colorMap->setGradient(QCPColorGradient::gpJet);
    colorMap->setDataRange(QCPRange(0, 1));
    colorMap->setInterpolate(false);
    //ui->Axes_plane->rescaleAxes();

    QFont labelFont = font();
    labelFont.setPointSize(14);
    ui -> Axes_plane -> xAxis->setLabelFont(labelFont);
    ui -> Axes_plane -> yAxis->setLabelFont(labelFont);

    ui -> Axes_plane -> xAxis->setTickLabelFont(labelFont);
    ui -> Axes_plane -> yAxis->setTickLabelFont(labelFont);

    ui -> Axes_plane -> xAxis->setLabel(xAxlabel);
    ui -> Axes_plane -> yAxis->setLabel(yAxlabel);


    ui->Axes_plane->replot();
    return 0;
}

int MainWindow::ISOCMap()
{
    int X = ui->X_val->text().toInt();
    int Y = ui->Y_val->text().toInt();
    int Z = ui->Z_val->text().toInt();
    double DoseMax=DoseDistr.getDoseMaxValue();

    QCPColorMap *colorMap = new QCPColorMap(ui->Axes_plane->xAxis, ui->Axes_plane->yAxis);

    QString xAxlabel="", yAxlabel = "";

    const int N_lvls=9;
    QLineEdit* isodose_list[N_lvls] = {ui->isodose_lvl_1,
                                       ui->isodose_lvl_2,
                                       ui->isodose_lvl_3,
                                       ui->isodose_lvl_4,
                                       ui->isodose_lvl_5,
                                       ui->isodose_lvl_6,
                                       ui->isodose_lvl_7,
                                       ui->isodose_lvl_8,
                                       ui->isodose_lvl_9 };

    double isodose_lvl_num[N_lvls]{};
    //double maxVal=0;double Val;

    for(int lvl=0;lvl<N_lvls;lvl++)
    {
        isodose_lvl_num[lvl]=isodose_list[lvl]->text().toDouble();
    }

    //Сортировка
    double buff=0;bool notSorted;
    do{
        notSorted = false;
        for(int lvl=0;lvl<N_lvls-1;lvl++)
        {
            if(isodose_lvl_num[lvl+1]>isodose_lvl_num[lvl])
            {
                buff=isodose_lvl_num[lvl];
                isodose_lvl_num[lvl]=isodose_lvl_num[lvl+1];
                isodose_lvl_num[lvl+1]=buff;
                notSorted=true;
            }
        }
    }while(notSorted);

    for(int lvl=0;lvl<N_lvls;lvl++)
    {
        isodose_list[lvl]->setText(QString::number(isodose_lvl_num[lvl]));
    }

    //Нормировка на единицу
    for(int lvl=0;lvl<N_lvls;lvl++)
    {
        isodose_lvl_num[lvl]=isodose_lvl_num[lvl] / 100;
    }



    bool pointIsSet; // указание что точка округлена до ближайшего уровня изодозы
    int lvl;//текущий уровень изодозы
    double doseVal;

    if(ui->plane_XY_rb->isChecked())
    {
        colorMap->data()->setSize(this->sizeX, this->sizeY);
        colorMap->data()->setRange(QCPRange(0, this->sizeX), QCPRange(0, this->sizeY));
        for (int x=0; x<this->sizeX; ++x)
        {
            for (int y=0; y<this->sizeY; ++y)
            {
                doseVal=(DoseDistr.element(x,y,Z))/DoseMax;// нормированное значение дозы в точке
                lvl=0;//текущий уровень изодозы
                do{
                    pointIsSet=false;
                    if(doseVal>isodose_lvl_num[lvl])
                    {
                        doseVal=isodose_lvl_num[lvl];
                        pointIsSet=true;
                    }else{
                        if(lvl>=N_lvls) // Случай, если дозы меньше нижнего уровня
                        {
                            doseVal=0;
                            pointIsSet=true;
                        }
                        lvl++;
                    }
                }while(pointIsSet);

                colorMap->data()->setCell(x, y, doseVal);
            }
        }

        int MaxInd=0, xShift=0, yShift=0;
        if(this->sizeX>this->sizeY)
        {
            MaxInd=this->sizeX;
            yShift = 0.5*((this->sizeX) - (this->sizeY));
        }else
        {
            MaxInd=this->sizeY;
            xShift = 0.5*((this->sizeY) - (this->sizeX));
        }
        ui->Axes_plane->xAxis->setRange(0-xShift,MaxInd-xShift);
        ui->Axes_plane->yAxis->setRange(0-yShift,MaxInd-yShift);

        xAxlabel="X, vox.u.", yAxlabel = "Y, vox.u.";

    }else if(ui->plane_XZ_rb->isChecked())
    {
        colorMap->data()->setSize(this->sizeZ, this->sizeX);
        colorMap->data()->setRange(QCPRange(0, this->sizeZ), QCPRange(0, this->sizeX));
        for (int z=0; z<this->sizeZ; ++z)
        {
            for (int x=0; x<this->sizeX; ++x)
            {
                doseVal=(DoseDistr.element(x,Y,z))/DoseMax;// нормированное значение дозы в точке
                lvl=0;//текущий уровень изодозы
                do{
                    pointIsSet=false;
                    if(doseVal>isodose_lvl_num[lvl])
                    {
                        doseVal=isodose_lvl_num[lvl];
                        pointIsSet=true;
                    }else{
                        if(lvl>=N_lvls) // Случай, если дозы меньше нижнего уровня
                        {
                            doseVal=0;
                            pointIsSet=true;
                        }
                        lvl++;
                    }
                }while(pointIsSet);

                colorMap->data()->setCell(z, x,doseVal);
            }
        }

        int MaxInd=0, xShift=0, yShift=0;
        if(this->sizeX>this->sizeZ)
        {
            MaxInd = this->sizeX;
            xShift = 0.5*((this->sizeX) - (this->sizeZ));
        }else
        {
            MaxInd = this->sizeZ;
            yShift = 0.5*((this->sizeZ) - (this->sizeX));
        }
        ui->Axes_plane->xAxis->setRange(0-xShift,MaxInd-xShift);
        ui->Axes_plane->yAxis->setRange(0-yShift,MaxInd-yShift);
        xAxlabel="Z, vox.u.", yAxlabel = "X, vox.u.";
    }else if(ui->plane_YZ_rb->isChecked())
    {
        colorMap->data()->setSize(this->sizeZ, this->sizeY);
        colorMap->data()->setRange(QCPRange(0, this->sizeZ), QCPRange(0, this->sizeY));
        for (int z=0; z<this->sizeZ; ++z)
        {
            for (int y=0; y<this->sizeX; ++y)
            {
                doseVal=(DoseDistr.element(X,y,z))/DoseMax;// нормированное значение дозы в точке
                lvl=0;//текущий уровень изодозы
                do{
                    pointIsSet=false;
                    if(doseVal>isodose_lvl_num[lvl])
                    {
                        doseVal=isodose_lvl_num[lvl];
                        pointIsSet=true;
                    }else{
                        if(lvl>=N_lvls) // Случай, если дозы меньше нижнего уровня
                        {
                            doseVal=0;
                            pointIsSet=true;
                        }
                        lvl++;
                    }
                }while(pointIsSet);

                colorMap->data()->setCell(z, y, doseVal);
            }
        }

        int MaxInd=0, xShift=0, yShift=0;
        if(this->sizeZ>this->sizeY)
        {
            MaxInd=this->sizeZ;
            yShift = 0.5*((this->sizeZ) - (this->sizeY));
        }else
        {
            MaxInd=this->sizeY;
            xShift = 0.5*((this->sizeZ) - (this->sizeY));
        }
        ui->Axes_plane->xAxis->setRange(0-xShift,MaxInd-xShift);
        ui->Axes_plane->yAxis->setRange(0-yShift,MaxInd-yShift);
        xAxlabel="Z, vox.u.", yAxlabel = "Y, vox.u.";
    }else return 1;

    QCPColorGradient jetCM;
    jetCM.loadPreset(QCPColorGradient::gpJet);
    jetCM.setLevelCount(1000);

    colorMap->setGradient(jetCM);
    colorMap->setDataRange(QCPRange(0, 1));
    colorMap->setInterpolate(false);

    QFont labelFont = font();
    labelFont.setPointSize(14);
    ui -> Axes_plane -> xAxis->setLabelFont(labelFont);
    ui -> Axes_plane -> yAxis->setLabelFont(labelFont);

    ui -> Axes_plane -> xAxis->setTickLabelFont(labelFont);
    ui -> Axes_plane -> yAxis->setTickLabelFont(labelFont);

    ui -> Axes_plane -> xAxis->setLabel(xAxlabel);
    ui -> Axes_plane -> yAxis->setLabel(yAxlabel);

    ui->Axes_plane->replot();
    return 0;
}

int MainWindow::ISOdoses()
{


    int Xval = ui->X_val->text().toInt();
    int Yval = ui->Y_val->text().toInt();
    int Zval = ui->Z_val->text().toInt();

    double DoseMax=DoseDistr.getDoseMaxValue();

    const int N_lvls=9;
    QLineEdit* isodose_list[N_lvls] = {ui->isodose_lvl_1,
                                       ui->isodose_lvl_2,
                                       ui->isodose_lvl_3,
                                       ui->isodose_lvl_4,
                                       ui->isodose_lvl_5,
                                       ui->isodose_lvl_6,
                                       ui->isodose_lvl_7,
                                       ui->isodose_lvl_8,
                                       ui->isodose_lvl_9 };

    double isodose_lvl_num[N_lvls]{};
    //double maxVal=0;double Val;

    for(int lvl=0;lvl<N_lvls;lvl++)
    {
        isodose_lvl_num[lvl]=isodose_list[lvl]->text().toDouble();
    }

    //Сортировка
    double buff=0;bool notSorted;
    do{
        notSorted = false;
        for(int lvl=0;lvl<N_lvls-1;lvl++)
        {
            if(isodose_lvl_num[lvl+1]>isodose_lvl_num[lvl])
            {
                buff=isodose_lvl_num[lvl];
                isodose_lvl_num[lvl]=isodose_lvl_num[lvl+1];
                isodose_lvl_num[lvl+1]=buff;
                notSorted=true;
            }
        }
    }while(notSorted);

    for(int lvl=0;lvl<N_lvls;lvl++)
    {
        isodose_list[lvl]->setText(QString::number(isodose_lvl_num[lvl]));
    }

    //Нормировка на единицу
    for(int lvl=0;lvl<N_lvls;lvl++)
    {
        isodose_lvl_num[lvl]=isodose_lvl_num[lvl] / 100;
    }

    QString xAxlabel="", yAxlabel = "";

    int ContourCounter = 0;
    int curveLength;

    QVector<double> xData, yData, tData;

    QPen PenStyle;
    PenStyle.setWidth(3);


    int I = -1;
    int clearObjLessThen=2;
    bool traceCavities = false;

    QVector<QCPCurve*> newCurve;

    int MaxInd=0, xShift=0, yShift=0;
    int dirX, dirY; //направления вдоль осей X и Y. Нужны для переназначения координат матрицы

    if(ui->plane_XZ_rb->isChecked())
    {
        dirX = this->sizeZ;
        dirY = this->sizeX;

        xAxlabel="Z, vox.u.", yAxlabel = "X, vox.u.";

    }else if(ui->plane_XY_rb->isChecked())
    {
        dirX = this->sizeX;
        dirY = this->sizeY;

        xAxlabel="X, vox.u.", yAxlabel = "Y, vox.u.";

    }else if(ui->plane_YZ_rb->isChecked())
    {
        dirX = this->sizeZ;
        dirY = this->sizeY;

        xAxlabel="Z, vox.u.", yAxlabel = "Y, vox.u.";

    }else return 1;

    //++++ИНИЦИАЛИЗАЦИЯ МАТРИЦЫ+++++
    double** planeMtrx = new double* [dirX]{};
    for (int i=0; i<dirX; i++)
    {
        planeMtrx[i]=new double[dirY]{};
    }
    // Присвоение элементов среза
    if(ui->plane_XZ_rb->isChecked())
    {
        for (int z=0; z<dirX; z++)
        {
            for (int x=0; x<dirY; x++)
            {
                planeMtrx[z][x]=(DoseDistr.element(x,Yval,z))/DoseMax;
            }
        }
    }else if(ui->plane_XY_rb->isChecked())
    {
        for (int x=0; x<dirX; x++)
        {
            for (int y=0; y<dirY; y++)
            {
                planeMtrx[x][y]=(DoseDistr.element(x,y,Zval))/DoseMax;
            }
        }
    }else if(ui->plane_YZ_rb->isChecked())
    {
        for (int z=0; z<dirX; z++)
        {
            for (int y=0; y<dirY; y++)
            {
                planeMtrx[z][y]=(DoseDistr.element(Xval,y,z))/DoseMax;
            }
        }

    }else return 1;

    bool WriteNewFile = false;//true;//
    if (WriteNewFile)
    {
        QFile newFile("Cut.txt");
        newFile.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text);
        QTextStream newData(&newFile);
        newData << dirX <<'\t' << dirY << '\n';
        for (int j=0; j < dirY; j++)
        {
            for (int i=0; i < dirX; i++)
            {
                newData << planeMtrx[i][j] << "  ";
            }
            newData << '\n';
        }
        newFile.close();
    }

    //Инициализация поиска
    //MooreTracing isodoseCurve(planeMtrx,dirX,dirY,0.0);
    QVector<MooreTracing*> isoCurve(N_lvls);

    //Сканирование по изодозам
    for(int lvl=0; lvl<N_lvls;lvl++)
    {
        isoCurve[lvl] = new MooreTracing(planeMtrx,dirX,dirY,isodose_lvl_num[lvl]);
        PenStyle.setColor(this->colorList[lvl]);
        //Поиск контуров
        while(isoCurve[lvl]->traceNewObj(!traceCavities))
        {
            //Устранение контуров - линий;
          // if ((isoCurve[lvl]->getTraceXmax()-isoCurve[lvl]->getTraceXmin())==0) continue;
          // if ((isoCurve[lvl]->getTraceYmax()-isoCurve[lvl]->getTraceYmin())==0) continue;

            curveX.clear();
            curveY.clear();
            xData.clear();
            yData.clear();
            tData.clear();

            curveX = isoCurve[lvl]->getNewTraceX();
            curveY = isoCurve[lvl]->getNewTraceY();
            curveLength=curveX.size();
            if(curveX.empty()) continue;
            if(clearObjLessThen > curveLength) continue;

            I++;
            ContourCounter++;

            newCurve.push_back(new QCPCurve(ui -> Axes_plane ->xAxis, ui -> Axes_plane ->yAxis));

            xData.resize(curveLength);
            yData.resize(curveLength);
            tData.resize(curveLength);

            for (int i=0; i<curveLength; i++)
            {
              xData[i] = curveX[i] + 0.5;
              yData[i] = curveY[i] + 0.5;
              tData[i] = i;
            }

            newCurve[I]->setData(tData, xData, yData);
            newCurve[I]->setLineStyle(QCPCurve::LineStyle::lsLine);
            newCurve[I]->setPen(PenStyle);

            ui->Axes_plane->replot();
        }
    }

    //++++УДАЛЕНИЕ МАТРИЦЫ+++++
    for (int i=0;i<dirX;i++)
    {
        delete [] planeMtrx[i];
    }
    delete [] planeMtrx;


    //Организация области отображения. Можно переделать, с учетом растяжения графика
    if(dirX > dirY)
    {
        MaxInd=dirX;
        yShift = 0.5*(dirX - dirY);
    }else
    {
        MaxInd=dirY;
        xShift = 0.5*(dirY - dirX);
    }
    ui->Axes_plane->xAxis->setRange(0-xShift,MaxInd-xShift);
    ui->Axes_plane->yAxis->setRange(0-yShift,MaxInd-yShift);

    //Задание подписи осей
    QFont labelFont = font();
    labelFont.setPointSize(14);
    ui -> Axes_plane -> xAxis->setLabelFont(labelFont);
    ui -> Axes_plane -> yAxis->setLabelFont(labelFont);

    ui -> Axes_plane -> xAxis->setTickLabelFont(labelFont);
    ui -> Axes_plane -> yAxis->setTickLabelFont(labelFont);

    ui -> Axes_plane -> xAxis->setLabel(xAxlabel);
    ui -> Axes_plane -> yAxis->setLabel(yAxlabel);

    ui->Axes_plane->axisRect()->setBackground(Qt::gray);

    ui->Axes_plane->replot();

    //ui->Axes_plane->setBackground(Qt::white);
    return 0;

}






