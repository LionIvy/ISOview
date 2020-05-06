#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <string>
#include <iostream>
#include <fstream>
#include <QAction>

#include "mooretracing.h"

#include <QDir>
#include <QFileDialog>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //==================================================
    ///=============Настройка директорий================
    //==================================================
    ///
    {
        QString dirname = QDir::currentPath();
        rootFolder=dirname;
        ui -> statusbar -> showMessage(dirname);
        ui -> statusbar -> update();
    }

    //==================================================
    ///======Настройка взаимодействий с графиками=======
    //==================================================
    ///
    {
    ui -> Axes_plane -> setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui -> Axes_curves-> setInteractions(QCP::iSelectPlottables);
   // ui -> Axes_curves-> setInteractions(QCP::iRangeDrag | QCP::iRangeZoom );
    setAxesSize(ui->Axes_plane, 700, 700);
    setAxesSize(ui->Axes_curves, 700, 700);

    QObject::connect(ui->Axes_curves, SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)),
                     this           , SLOT(axisLabelDoubleClick(QCPAxis*,QCPAxis::SelectablePart)));
    QObject::connect(ui->Axes_plane , SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)),
                     this           , SLOT(axisLabelDoubleClick(QCPAxis*,QCPAxis::SelectablePart)));

    QObject::connect(ui->Axes_curves, SIGNAL(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)),
                     this           , SLOT(legendDoubleClick  (QCPLegend*,QCPAbstractLegendItem*)));


    QObject::connect(ui->toolBox, SIGNAL(currentChanged(int)),
                     this       , SLOT  (toolIsSwitched(int)));
    QObject::connect(ui->tabGraphs, SIGNAL(currentChanged(int)),
                     this       , SLOT  (tabIsSwitched(int)));


    ui -> Axes_plane -> addLayer("pointOfInterest",ui->Axes_plane->layer("main"), QCustomPlot::limAbove);





    QObject::connect(ui-> axXLim_min, SIGNAL(editingFinished()),
                     this           , SLOT(setCurveRange()));
    QObject::connect(ui-> axYLim_min, SIGNAL(editingFinished()),
                     this           , SLOT(setCurveRange()));
    QObject::connect(ui-> axXLim_max, SIGNAL(editingFinished()),
                     this           , SLOT(setCurveRange()));
    QObject::connect(ui-> axYLim_max, SIGNAL(editingFinished()),
                     this           , SLOT(setCurveRange()));

    QObject::connect(ui-> MaxVal_pointX , SIGNAL(editingFinished()),
                     this               , SLOT(updateCurves()));
    QObject::connect(ui-> MaxVal_pointX , SIGNAL(editingFinished()),
                     this               , SLOT(updatePlane()));
    QObject::connect(ui-> MaxVal_pointY , SIGNAL(editingFinished()),
                     this               , SLOT(updateCurves()));
    QObject::connect(ui-> MaxVal_pointY , SIGNAL(editingFinished()),
                     this               , SLOT(updatePlane()));
    QObject::connect(ui-> MaxVal_pointZ , SIGNAL(editingFinished()),
                     this               , SLOT(updateCurves()));
    QObject::connect(ui-> MaxVal_pointZ , SIGNAL(editingFinished()),
                     this               , SLOT(updatePlane()));

    QObject::connect(ui-> ValueAtPoint , SIGNAL(editingFinished()),
                     this               , SLOT(updateCurves()));
    QObject::connect(ui-> ValueAtPoint , SIGNAL(editingFinished()),
                     this               , SLOT(updatePlane()));

    }

    //==================================================
    ///=========Инициализация работы слайдеров==========
    //==================================================
    ///
    {
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

    }

    //==================================================
    ///====Инициализация работы кнопок переключения=====
    //==================================================
    ///
    {
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
    }

    //==================================================
    ///====Ограничения на значения вводимых значений====
    //==================================================
    ///
    {
        setLineValidators();
    }

    //==================================================
    ///===========Задание начальных значений============
    //==================================================
    ///
    {
        int axXLim_max = 0;
        sizeX > axXLim_max ? axXLim_max = sizeX : false;
        sizeY > axXLim_max ? axXLim_max = sizeY : false;
        sizeZ > axXLim_max ? axXLim_max = sizeZ : false;

        ui-> axXLim_min -> setText(QString::number(0));
        ui-> axXLim_max -> setText(QString::number(axXLim_max));
        ui-> axYLim_min -> setText(QString::number(0));
        ui-> axYLim_max -> setText(QString::number(1.1));
    }

    //==================================================
    ///================Коррекция цвета==================
    //==================================================
    ///
    {
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

    }

    //==================================================
    ///===============Настройка кнопок==================
    //==================================================
    ///
    {
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

        QObject::connect(ui->curve_axis_update_btn , SIGNAL(clicked()),
                         this                      , SLOT  (setCurveRange()));
        QObject::connect(ui->curve_axis_reset_btn , SIGNAL(clicked()),
                         this                      , SLOT  (resetCurveRange()));


    }


}

MainWindow::~MainWindow()
{
    delete[] colorList;

    delete ui;
}

//==================================================
///===========Настройки окна приложения=============
//==================================================
//

void MainWindow::setLineValidators()
{
    //==================================================
    ///====Ограничения на значения вводимых значений====
    //==================================================
    ///

    ui->ValueAtPoint -> setValidator(new QDoubleValidator(0.01, 1000, 2, this));

    ui->MaxVal_pointX-> setValidator(new QIntValidator(0,sizeX-1,this));
    ui->MaxVal_pointY-> setValidator(new QIntValidator(0,sizeY-1,this));
    ui->MaxVal_pointZ-> setValidator(new QIntValidator(0,sizeZ-1,this));


    int axXLim_max = 0;
    sizeX > axXLim_max ? axXLim_max = sizeX : false;
    sizeY > axXLim_max ? axXLim_max = sizeY : false;
    sizeZ > axXLim_max ? axXLim_max = sizeZ : false;

    double MaxVal = ui->ValueAtPoint -> text().toDouble();
    double axYLim_max = 0.0105*MaxVal;

    ui-> axXLim_min -> setValidator(new QDoubleValidator(0,axXLim_max,1,this));
    ui-> axXLim_max -> setValidator(new QDoubleValidator(0,axXLim_max,1,this));
    ui-> axYLim_min -> setValidator(new QDoubleValidator(0,axYLim_max,1,this));
    ui-> axYLim_max -> setValidator(new QDoubleValidator(0,axYLim_max,1,this));
    //ui->axX

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
        isodose_list[i]-> setValidator(new QDoubleValidator(0,2*MaxVal,3,this));
    }


}

void MainWindow::setSliders()
{
    ui -> X_Slider ->setMinimum(0);
    ui -> X_Slider ->setMaximum(sizeX-1);
    ui -> X_Slider ->setSingleStep(1);

    ui -> Y_Slider ->setMinimum(0);
    ui -> Y_Slider ->setMaximum(sizeY-1);
    ui -> Y_Slider ->setSingleStep(1);

    ui -> Z_Slider ->setMinimum(0);
    ui -> Z_Slider ->setMaximum(sizeZ-1);
    ui -> Z_Slider ->setSingleStep(1);

    int X_value = round(0.5*sizeX);
    int Y_value = round(0.5*sizeY);
    int Z_value = round(0.5*sizeZ);

    ui -> X_val ->setNum(X_value);
    ui -> Y_val ->setNum(Y_value);
    ui -> Z_val ->setNum(Z_value);

    ui -> X_Slider ->setValue(X_value);
    ui -> Y_Slider ->setValue(Y_value);
    ui -> Z_Slider ->setValue(Z_value);
}

void MainWindow::toolIsSwitched(int tool)
{
    int currTab = ui->tabGraphs->currentIndex();

    if (      (tool == 1) && (currTab == 1)){
        ui->tabGraphs->setCurrentIndex(0);
    }else if ((tool == 2) && (currTab == 0)){
        ui->tabGraphs->setCurrentIndex(1);
    }

}

void MainWindow::tabIsSwitched(int tab)
{
    int currTool = ui->toolBox->currentIndex();

    if (      (tab == 0) && (currTool == 2)){
        ui->toolBox->setCurrentIndex(1);
    }else if ((tab == 1) && (currTool == 1)){
        ui->toolBox->setCurrentIndex(2);
    }
}

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

    colorList = new QRgb[N_lvls];
    QString setBackgroundColor;
    for (int i=0;i<N_lvls;i++)
    {
      colorList[i] = jetCM.color(isodose_lvl_num[i],QCPRange(0, 1),false);
      setBackgroundColor="background-color: rgb(" +
              QString::number(qRed  (colorList[i])) + ", " +
              QString::number(qGreen(colorList[i])) + ", " +
              QString::number(qBlue (colorList[i])) + ");";
      isodose_lbl_list[i]->setAutoFillBackground(true);
      isodose_lbl_list[i]->setStyleSheet(setBackgroundColor);
    }
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
//
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
    if (loadlock) return;

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

     loadlock = true;

     loadNewDoseFile(filename);
     DistributionIsSet=true;
     loadlock = false;

     QString qstr_folderName(charfolderName);
     folderLocation=qstr_folderName;

     ui -> statusbar->showMessage(qstr_folderName);
     ui -> statusbar -> update();

     setLineValidators();
     int axXLim_max = 0;
     sizeX > axXLim_max ? axXLim_max = sizeX : false;
     sizeY > axXLim_max ? axXLim_max = sizeY : false;
     sizeZ > axXLim_max ? axXLim_max = sizeZ : false;
     double MaxVal = ui->ValueAtPoint -> text().toDouble();
     double axYLim_max = 0.0105*MaxVal;
     ui-> axXLim_min -> setText(QString::number(0));
     ui-> axXLim_max -> setText(QString::number(axXLim_max));
     ui-> axYLim_min -> setText(QString::number(0));
     ui-> axYLim_max -> setText(QString::number(axYLim_max));

     setSliders();

     updateCurves();
     updatePlane();
}

void MainWindow::loadNewDoseFile(QString filename)
{
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

    DoseDistr = DoseVector(filename, FileFormat);
    sizeX = DoseDistr.getXSize();
    sizeY = DoseDistr.getYSize();
    sizeZ = DoseDistr.getZSize();
    doseMaxValue = DoseDistr.getDoseMaxValue();

    doseMaxValPos = DoseDistr.getMaxValPosition();
    ui->MaxVal_pointX->setText(QString::number(std::get<0>(doseMaxValPos)));
    ui->MaxVal_pointY->setText(QString::number(std::get<1>(doseMaxValPos)));
    ui->MaxVal_pointZ->setText(QString::number(std::get<2>(doseMaxValPos)));
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

        QVector<double> x(sizeX), Dx(sizeX); // initialize with entries 0..100
        for (int i=0; i<sizeX; ++i)
        {
          x[i] = i;
          Dx[i] = (DoseDistr.element(i,Y,Z));
        }

        QString filename = folderLocation + "D(x,"+QString::number(Y)+","+QString::number(Z)+").txt";
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

        QVector<double> y(sizeY), Dy(sizeY); // initialize with entries 0..100
        for (int i=0; i<sizeY; ++i)
        {
          y[i] = i;
          Dy[i] = (DoseDistr.element(X,i,Z)); // let's plot a quadratic function
        }
        QString filename = folderLocation + "D("+QString::number(X)+",y,"+QString::number(Z)+").txt";
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

        QVector<double> z(sizeZ);
        QVector<double> Dz(sizeZ); // initialize with entries 0..100
        for (int i=0; i<sizeZ; ++i)
        {
          z[i] = i; // x goes from -1 to 1
          Dz[i] = (DoseDistr.element(X,Y,i)); // let's plot a quadratic function
        }

        QString filename = folderLocation + "D("+QString::number(X)+","+QString::number(Y)+",z).txt";
        printDose2File(filename, "Z", X, Y, z, Dz);

    }
}
void MainWindow::saveGlobalDz()
{
    if (DistributionIsSet)
    {
        QVector<double> z(sizeZ);
        QVector<double> Dz(sizeZ,0.0); // initialize with entries 0..100

        for (int i=0; i<sizeZ; ++i)
        {
          z[i] = i; // x goes from -1 to 1
        }

        for (int z=0; z<sizeZ; ++z)
        {
            for(int X=0; X<sizeX; ++X){
                for(int Y=0; Y<sizeY; ++Y){
                    Dz[z] += (DoseDistr.element(X,Y,z)); // let's plot a quadratic function
                }
            }
        }
        QString filename = folderLocation + "D(z).txt";
        printDose2File(filename, "Z", -999, -999, z, Dz);

    }

}

void MainWindow::printDose2File(QString FileName,QString Direction, int coordinate1, int coordinate2, QVector<double> direction, QVector<double> Dose)
{
    //QString FileName =folderLocation + "D("+Direction+").txt";

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
//
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
        if (N<curvesMaximum)
        {
            for(int i=N-1;i<curvesMaximum;i++)
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

        //Параметры нормировки
        int posX = ui->MaxVal_pointX->text().toInt();
        int posY = ui->MaxVal_pointY->text().toInt();
        int posZ = ui->MaxVal_pointZ->text().toInt();
        double doseAtPoint = DoseDistr.element(posX,posY,posZ);
        double relVal = ui->ValueAtPoint -> text().toDouble();

        double DoseMax = doseAtPoint/(0.01*relVal);


        //Считчик количества графиков (для построения легенды)
        int graphCounter=-1;

        if(Dz_rb_val==2)
        {
            QVector<double> z(sizeZ), Dz(sizeZ); // initialize with entries 0..100
            for (int i=0; i<sizeZ; ++i)
            {
              z[i] = i; // x goes from -1 to 1
              Dz[i] = (DoseDistr.element(X,Y,i))/DoseMax; // let's plot a quadratic function
            }

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
            QVector<double> x(sizeX), Dx(sizeX); // initialize with entries 0..100
            for (int i=0; i<sizeX; ++i)
            {
              x[i] = i;
              Dx[i] = (DoseDistr.element(i,Y,Z))/DoseMax;
            }

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
            QVector<double> y(sizeY), Dy(sizeY); // initialize with entries 0..100
            for (int i=0; i<sizeY; ++i)
            {
              y[i] = i;
              Dy[i] = (DoseDistr.element(X,i,Z))/DoseMax; // let's plot a quadratic function
            }

            graphCounter++;
            ui -> Axes_curves -> addGraph();
            ui -> Axes_curves -> graph(graphCounter)->setName("D(y)");
            ui -> Axes_curves -> graph(graphCounter)->setData(y, Dy);                         // Передача данных на график
            ui -> Axes_curves -> graph(graphCounter)->setPen(LinePenBlue);
            ui -> Axes_curves -> graph(graphCounter)->setLineStyle(QCPGraph::lsStepLeft);  // Задание типа линии (lsStepCenter - ступенчатый)
        }

        if(Dz_total_val == 2){

            QVector<double> z(sizeZ);
            QVector<double> TDz(sizeZ,0.0); // initialize with entries 0..100
            double totalMaximum=0;

            for (int i=0; i<sizeZ; ++i)
            {
              z[i] = i; // x goes from -1 to 1
            }

            for (int z=0; z<sizeZ; ++z)
            {
                for(int X=0; X<sizeX; ++X){
                    for(int Y=0; Y<sizeY; ++Y){
                        TDz[z] += (DoseDistr.element(X,Y,z)); // let's plot a quadratic function
                        if(TDz[z]>totalMaximum)
                        {totalMaximum=TDz[z];}
                    }
                }
            }
            for (int z=0; z<sizeZ; ++z)
            {
                TDz[z] /= totalMaximum;
            }

            graphCounter++;
            ui -> Axes_curves -> addGraph();
            ui -> Axes_curves -> graph(graphCounter)->setName("D(z)");
            // Построение гафика #M
            ui -> Axes_curves -> graph(graphCounter)->setData(z, TDz);
            ui -> Axes_curves -> graph(graphCounter)->setPen(LinePenBlack);
            ui -> Axes_curves -> graph(graphCounter)->setLineStyle(QCPGraph::lsStepLeft);  // Задание типа линии (lsStepCenter - ступенчатый)
        }

        double axisXmin = ui->axXLim_min->text().toDouble();
        double axisXmax = ui->axXLim_max->text().toDouble();
        double axisYmin = ui->axYLim_min->text().toDouble();
        double axisYmax = ui->axYLim_max->text().toDouble();
        //int yTickCount = axisYmax-axisYmin

        ui -> Axes_curves -> xAxis->setRange(axisXmin, axisXmax);
        ui -> Axes_curves -> yAxis->setRange(axisYmin, axisYmax);

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
        ui -> Axes_plane  -> setCurrentLayer("main");
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

        ui -> Axes_plane -> replot();



    }
}

int MainWindow::CMap()
{
    int X = ui->X_val->text().toInt();
    int Y = ui->Y_val->text().toInt();
    int Z = ui->Z_val->text().toInt();

    //Условия нормировки
    int posX = ui->MaxVal_pointX->text().toInt();
    int posY = ui->MaxVal_pointY->text().toInt();
    int posZ = ui->MaxVal_pointZ->text().toInt();
    double doseAtPoint = DoseDistr.element(posX,posY,posZ);
    double relVal = ui->ValueAtPoint -> text().toDouble();
    double DoseMax = doseAtPoint/(0.01*relVal);

    QCPColorMap *colorMap = new QCPColorMap(ui->Axes_plane->xAxis, ui->Axes_plane->yAxis);

    QString xAxlabel="", yAxlabel = "";

    if(ui->plane_XY_rb->isChecked())
    {
        colorMap->data()->setSize(sizeX,sizeY);
        colorMap->data()->setRange(QCPRange(0, sizeX-1),
                                   QCPRange(0, sizeY-1));
        for (int x=0; x<sizeX; ++x)
        {
            for (int y=0; y<sizeY; ++y)
            {
                colorMap->data()->setCell(x, y, (DoseDistr.element(x,y,Z))/DoseMax);
            }
        }

        int MaxInd=0, xShift=0, yShift=0;
        if(sizeX > sizeY)
        {
            MaxInd=sizeX-1;
            yShift = 0.5*(sizeX - sizeY);
        }else
        {
            MaxInd=sizeY-1;
            xShift = 0.5*(sizeY - sizeX);
        }
        ui->Axes_plane->xAxis->setRange(0-xShift,MaxInd-xShift);
        ui->Axes_plane->yAxis->setRange(0-yShift,MaxInd-yShift);

        xAxlabel="X, vox.u.", yAxlabel = "Y, vox.u.";

    }else if(ui->plane_XZ_rb->isChecked())
    {
        colorMap->data()->setSize(sizeZ, sizeX);
        colorMap->data()->setRange(QCPRange(0, sizeZ-1),
                                   QCPRange(0, sizeX-1));
        for (int z=0; z< sizeZ; ++z)
        {
            for (int x=0; x<sizeX; ++x)
            {
                colorMap->data()->setCell(z, x, (DoseDistr.element(x,Y,z))/DoseMax);
            }
        }

        int MaxInd=0, xShift=0, yShift=0;
        if( sizeX> sizeZ)
        {
            MaxInd = sizeX-1;
            xShift = 0.5*(sizeX - sizeZ);
        }else
        {
            MaxInd = sizeZ -1;
            yShift = 0.5*(sizeZ - sizeX);
        }
        ui->Axes_plane->xAxis->setRange(0-xShift,MaxInd-xShift);
        ui->Axes_plane->yAxis->setRange(0-yShift,MaxInd-yShift);

        xAxlabel="Z, vox.u.", yAxlabel = "X, vox.u.";

    }else if(ui->plane_YZ_rb->isChecked())
    {
        colorMap->data()->setSize(sizeZ, sizeY);
        colorMap->data()->setRange(QCPRange(0, sizeZ-1),
                                   QCPRange(0, sizeY-1));
        for (int z = 0; z < sizeZ; ++z)
        {
            for (int y = 0; y < sizeX; ++y)
            {
                colorMap->data()->setCell(z, y, (DoseDistr.element(X,y,z))/DoseMax);
            }
        }

        int MaxInd=0, xShift=0, yShift=0;
        if(sizeZ > sizeY)
        {
            MaxInd = sizeZ-1;
            yShift = 0.5*(sizeZ - sizeY);
        }else
        {
            MaxInd= sizeY-1;
            xShift = 0.5*(sizeZ - sizeY);
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

    //Условия нормировки
    int posX = ui->MaxVal_pointX->text().toInt();
    int posY = ui->MaxVal_pointY->text().toInt();
    int posZ = ui->MaxVal_pointZ->text().toInt();
    double doseAtPoint = DoseDistr.element(posX,posY,posZ);
    double relVal = ui->ValueAtPoint -> text().toDouble();
    double DoseMax = doseAtPoint/(0.01*relVal);

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
        colorMap->data()->setSize(sizeX, sizeY);
        colorMap->data()->setRange(QCPRange(0, sizeX-1),
                                   QCPRange(0, sizeY-1));
        for (int x=0; x < sizeX; ++x)
        {
            for (int y=0; y < sizeY; ++y)
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
        if( sizeX > sizeY)
        {
            MaxInd= sizeX - 1;
            yShift = 0.5*(sizeX - sizeY);
        }else
        {
            MaxInd = sizeY - 1;
            xShift = 0.5*(sizeY - sizeX);
        }
        ui->Axes_plane->xAxis->setRange(0-xShift,MaxInd-xShift);
        ui->Axes_plane->yAxis->setRange(0-yShift,MaxInd-yShift);

        xAxlabel="X, vox.u.", yAxlabel = "Y, vox.u.";

    }else if(ui->plane_XZ_rb->isChecked())
    {
        colorMap->data()->setSize(sizeZ, sizeX);
        colorMap->data()->setRange(QCPRange(0, sizeZ-1),
                                   QCPRange(0, sizeX-1));
        for (int z=0; z < sizeZ; ++z)
        {
            for (int x=0; x < sizeX; ++x)
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
        if(sizeX > sizeZ)
        {
            MaxInd = sizeX - 1;
            xShift = 0.5*(sizeX - sizeZ);
        }else
        {
            MaxInd = sizeZ - 1;
            yShift = 0.5*(sizeZ - sizeX);
        }
        ui->Axes_plane->xAxis->setRange(0-xShift,MaxInd-xShift);
        ui->Axes_plane->yAxis->setRange(0-yShift,MaxInd-yShift);
        xAxlabel="Z, vox.u.", yAxlabel = "X, vox.u.";
    }else if(ui->plane_YZ_rb->isChecked())
    {
        colorMap->data()->setSize(sizeZ, sizeY);
        colorMap->data()->setRange(QCPRange(0, sizeZ-1),
                                   QCPRange(0, sizeY-1));
        for (int z=0; z < sizeZ; ++z)
        {
            for (int y=0; y < sizeX; ++y)
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
        if(sizeZ > sizeY)
        {
            MaxInd= sizeZ - 1;
            yShift = 0.5*(sizeZ - sizeY);
        }else
        {
            MaxInd=sizeY-1;
            xShift = 0.5*(sizeZ - sizeY);
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

    ui -> Axes_plane -> replot();

    showPointOfInterest();
    return 0;
}

int MainWindow::ISOdoses()
{
    int Xval = ui->X_val->text().toInt();
    int Yval = ui->Y_val->text().toInt();
    int Zval = ui->Z_val->text().toInt();

//    if (Zval>=256){
//        int stopIT=0;
//    }
    //Условия нормировки
    int posX = ui->MaxVal_pointX->text().toInt();
    int posY = ui->MaxVal_pointY->text().toInt();
    int posZ = ui->MaxVal_pointZ->text().toInt();
    double doseAtPoint = DoseDistr.element(posX,posY,posZ);
    double relVal = ui->ValueAtPoint -> text().toDouble();
    double DoseMax = doseAtPoint/(0.01*relVal);

    //==============================================================
    /// Проверка и упорядочивание уровней изодозных кривых
    //==============================================================
    ///
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


    //==============================================================
    ///              Насройка стиля отображения
    //==============================================================
    ///
    // Общий стиль линий
    QPen PenStyle;
    PenStyle.setWidth(3);

    // Подписи осей
    QString xAxlabel="", yAxlabel = "";

    // Лимиты отображения осей и возможные сдивиги осей для точного отображения
    int MaxInd=0, xShift=0, yShift=0;

    // Направления вдоль осей X и Y. Нужны для переназначения координат матрицы
    int dirX, dirY; // (например dirX = Z, dirY = X => плоскость ZX

    //Счетчик контуров на графике
    //int ContourCounter = 0;

    //Длина текущего контура
    int curveLength;

    //Данные для отображения текущего контура на графике
    QVector<double> xData, yData, tData;

    int clearObjLessThen=2;

    bool traceCavities = false;

    // Вектор всех кривых на графике
    QVector<QCPCurve*> newCurve;
    // Считчик контуров
    int curveID = -1;


    //==============================================================
    ///     Определение активного сечения и задание имени осей
    //==============================================================
    ///
    if(ui->plane_XZ_rb->isChecked())
    {
        dirX = sizeZ;
        dirY = sizeX;

        xAxlabel="Z, vox.u.", yAxlabel = "X, vox.u.";

    }else if(ui->plane_XY_rb->isChecked())
    {
        dirX = sizeX;
        dirY = sizeY;

        xAxlabel="X, vox.u.", yAxlabel = "Y, vox.u.";

    }else if(ui->plane_YZ_rb->isChecked())
    {
        dirX = sizeZ;
        dirY = sizeY;

        xAxlabel="Z, vox.u.", yAxlabel = "Y, vox.u.";

    }else return 1;

    //==============================================================
    ///        Задание матрицы текущего среза
    //==============================================================
    ///
    // Инициализация
    std::vector<std::vector<double>> planeMtrx (dirX, std::vector<double>(dirY, 0.0));

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

    //Проверка
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

    //==============================================================
    ///        Поиск изодозных кривых
    //==============================================================
    ///
    //Инициализация поиска
    QVector<MooreTracing> isoCurve(N_lvls);

    //Сканирование по изодозам
    for(int lvl=0; lvl<N_lvls;lvl++)
    {
        isoCurve[lvl] = MooreTracing(planeMtrx,dirX,dirY,isodose_lvl_num[lvl]);
        PenStyle.setColor(colorList[lvl]);
        //Поиск всех контуров
        while(isoCurve[lvl].traceNewObj(!traceCavities))
        {
            curveX.clear();
            curveY.clear();
            xData.clear();
            yData.clear();
            tData.clear();

            curveX = isoCurve[lvl].getNewTraceX();
            curveY = isoCurve[lvl].getNewTraceY();
            curveLength=curveX.size();
            if(curveX.empty()) continue;
            if(clearObjLessThen > curveLength) continue;

            curveID++;
            //ContourCounter++;
            //qDebug() << newCurve.size() <<'\n';

            newCurve.push_back(new QCPCurve(ui -> Axes_plane ->xAxis, ui -> Axes_plane ->yAxis));

            xData.resize(curveLength);
            yData.resize(curveLength);
            tData.resize(curveLength);

            for (int i=0; i<curveLength; i++)
            {
              xData[i] = curveX[i];
              yData[i] = curveY[i];
              tData[i] = i;
            }

            newCurve[curveID]->setData(tData, xData, yData);
            newCurve[curveID]->setLineStyle(QCPCurve::LineStyle::lsLine);
            newCurve[curveID]->setPen(PenStyle);

            ui->Axes_plane->replot();
        }
    }
/* /// Для последнего контура нужен поиск полостей, есть проблема по части динамической памяти.
 /// Если полостей слишком много строчка
 /// newCurve.push_back(new QCPCurve(ui -> Axes_plane ->xAxis, ui -> Axes_plane ->yAxis));
 /// переполняет память -> вылет
    // Для последнего контура нужен поиск полостей
    {
        int lvl=N_lvls-1;

        isoCurve[lvl] = MooreTracing(planeMtrx,dirX,dirY,isodose_lvl_num[lvl]);
        PenStyle.setColor(colorList[lvl]);
        //Поиск всех контуров
        while(isoCurve[lvl].traceNewObj(false))
        {
            curveX.clear();
            curveY.clear();
            xData.clear();
            yData.clear();
            tData.clear();

            curveX = isoCurve[lvl].getNewTraceX();
            curveY = isoCurve[lvl].getNewTraceY();
            curveLength=curveX.size();
            if(curveX.empty()) continue;
            if(clearObjLessThen > curveLength) continue;

            curveID++;
            //ContourCounter++;
            qDebug() << newCurve.size() <<'\n';
            newCurve.push_back(new QCPCurve(ui -> Axes_plane ->xAxis, ui -> Axes_plane ->yAxis));

            xData.resize(curveLength);
            yData.resize(curveLength);
            tData.resize(curveLength);

            for (int i=0; i<curveLength; i++)
            {
              xData[i] = curveX[i];
              yData[i] = curveY[i];
              tData[i] = i;
            }

            newCurve[curveID]->setData(tData, xData, yData);
            newCurve[curveID]->setLineStyle(QCPCurve::LineStyle::lsLine);
            newCurve[curveID]->setPen(PenStyle);

            ui->Axes_plane->replot();
        }
    }
*/


    planeMtrx.clear();


    //==============================================================
    ///        Настройка осей графика
    //==============================================================
    ///
    // Организация области отображения. Можно переделать, с учетом растяжения графика
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

    showPointOfInterest();
    ui->Axes_plane->replot();

    //ui->Axes_plane->setBackground(Qt::white);
    return 0;

}

void MainWindow::setCurveRange()
{
    double axisXmin = ui->axXLim_min->text().toDouble();
    double axisXmax = ui->axXLim_max->text().toDouble();
    double axisYmin = ui->axYLim_min->text().toDouble();
    double axisYmax = ui->axYLim_max->text().toDouble();

    ui -> Axes_curves -> xAxis->setRange(axisXmin, axisXmax);
    ui -> Axes_curves -> yAxis->setRange(axisYmin, axisYmax);

    ui -> Axes_curves -> replot();
}

void MainWindow::resetCurveRange()
{
    double axisXmin = 0;
    ui->axXLim_min->setText(QString::number(axisXmin));

    double axisXmax = 0;
    sizeX > axisXmax ? axisXmax = sizeX : false;
    sizeY > axisXmax ? axisXmax = sizeY : false;
    sizeZ > axisXmax ? axisXmax = sizeZ : false;
    ui->axXLim_max->setText(QString::number(axisXmax));

    double axisYmin = 0;
    ui->axYLim_min->setText(QString::number(axisYmin));

    double MaxVal = ui->ValueAtPoint -> text().toDouble();
    double axisYmax = 0.0105*MaxVal;
    ui->axYLim_max->setText(QString::number(axisYmax));

    ui -> Axes_curves -> xAxis->setRange(axisXmin, axisXmax);
    ui -> Axes_curves -> yAxis->setRange(axisYmin, axisYmax);

    ui -> Axes_curves -> replot();
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

void MainWindow::resetPointOfMaximum()
{
    if (DistributionIsSet)
    {
        doseMaxValPos = DoseDistr.getMaxValPosition();
        ui->MaxVal_pointX->setText(QString::number(std::get<0>(doseMaxValPos)));
        ui->MaxVal_pointY->setText(QString::number(std::get<1>(doseMaxValPos)));
        ui->MaxVal_pointZ->setText(QString::number(std::get<2>(doseMaxValPos)));

        updateCurves();
        updatePlane();

    }
}

void MainWindow::showPointOfInterest()
{
    if (!(ui->show_point_chB->isChecked())) return;

    int Xval = ui->X_val->text().toInt();
    int Yval = ui->Y_val->text().toInt();
    int Zval = ui->Z_val->text().toInt();

    int pointX = ui->MaxVal_pointX->text().toInt();
    int pointY = ui->MaxVal_pointY->text().toInt();
    int pointZ = ui->MaxVal_pointZ->text().toInt();

    double ax_X, ax_Y;
    double dx = 2.5;
    double dy = 2.5;

    if(ui->plane_XZ_rb->isChecked())
    {
        if (Yval!=pointY) return;

        ax_X = pointZ+0.5;
        ax_Y = pointX+0.5;

    }else if(ui->plane_XY_rb->isChecked())
    {
        if (Zval!=pointZ) return;

        ax_X = pointX+0.5;
        ax_Y = pointY+0.5;

    }else if(ui->plane_YZ_rb->isChecked())
    {
        if (Xval!=pointX) return;

        ax_X = pointZ+0.5;
        ax_Y = pointY+0.5;

    }else return;

    ui -> Axes_plane  -> setCurrentLayer("pointOfInterest");
    ui -> Axes_plane  -> clearGraphs();

    QPen pointPen;
    pointPen.setColor(Qt::black);
    pointPen.setWidth(3);
    //pointPen.setWidthF(3);
    pointPen.setStyle(Qt::DotLine);

    QVector<double> x(2),y(2);

    ui -> Axes_plane-> addGraph();
    x[0]=ax_X-dx; y[0]=ax_Y+dy;
    x[1]=ax_X+dx; y[1]=ax_Y-dy;
    ui -> Axes_plane  -> graph(0)->setData(x, y);
    ui -> Axes_plane-> graph(0)->setPen(pointPen);
    ui -> Axes_plane-> replot();

    ui -> Axes_plane-> addGraph();
    x[0]=ax_X-dx; y[0]=ax_Y-dy;
    x[1]=ax_X+dx; y[1]=ax_Y+dy;
    ui -> Axes_plane-> graph(1)->setData(x, y);
    ui -> Axes_plane-> graph(1)->setPen(pointPen);
    ui -> Axes_plane-> replot();

    ui -> Axes_plane-> addGraph();
    x[0]=ax_X; y[0]=ax_Y+dy;
    x[1]=ax_X; y[1]=ax_Y-dy;
    ui -> Axes_plane-> graph(2)->setData(x, y);
    ui -> Axes_plane-> graph(2)->setPen(pointPen);
    ui -> Axes_plane-> replot();

    ui -> Axes_plane-> addGraph();
    x[0]=ax_X-dx; y[0]=ax_Y;
    x[1]=ax_X+dx; y[1]=ax_Y;
    ui -> Axes_plane-> graph(3)->setData(x, y);
    ui -> Axes_plane-> graph(3)->setPen(pointPen);
    ui -> Axes_plane-> replot();



}
