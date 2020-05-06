#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSlider>
#include <QLabel>

#include "DoseVector.h"
#include <vector>
#include <QThread>

#include "qcustomplot.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

//protected:
//    void resizeEvent(QResizeEvent *event) override;

private slots:

    //==================================================
    ///===========Настройки окна приложения============
    //==================================================

    void setLineValidators();
    void setSliders();

    void toolIsSwitched(int tool);
    void tabIsSwitched(int tool);

    //==================================================
    ///===============Работа с файлами==================
    //==================================================
    void setFileFormatSRNA();
    void setFileFormatGEANT4();

    void openFile();

    void SavePlane();
    void SaveCurves();


    void saveDx();
    void saveDy();
    void saveDz();
    void saveGlobalDz();
    void printDose2File(QString FileName, QString Direction, int coordinate1, int coordinate2, QVector<double> direction, QVector<double> Dose);

    //==================================================
    ///=============Построение графиков:================
    //==================================================

    void updatePlane();
    void updateCurves();

    void setCurveRange();
    void resetCurveRange();

    void legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item);
    void axisLabelDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part);

    void resetPointOfMaximum();


private:


    //==================================================
    ///============Настройки окна приложения============
    //==================================================

    Ui::MainWindow *ui;

    QString rootFolder;
    QString folderLocation;

    QRgb* colorList;
    void setColorPanels();


    //void changeAxesProportion(QCustomPlot* axes, int sizeX, int sizeY);

    //==================================================
    ///===============Работа с файлами==================
    //==================================================

    bool loadlock=false;
    void loadNewDoseFile(QString filename);

    bool DistributionIsSet = false;
    DoseVector DoseDistr;
    int sizeX = 100, sizeY = 100, sizeZ = 100;    // matrix dimentions
    double doseMaxValue = 0;
    std::tuple<int,int,int> doseMaxValPos = std::make_tuple(0,0,0);



    //==================================================
    ///=============Построение графиков:================
    //==================================================

    int curvesMaximum=3;
    //void plotCurves();

    int CMap();

    int ISOCMap();

    int ISOdoses();
    std::vector<int> curveX;
    std::vector<int> curveY;

    int planesWidthMultiplier = 1;
    int planesHeightMultiplier = 1;
    int curvesWidthMultiplier = 1;
    int curvesHeightMultiplier = 1;
    void setAxesSize(QCustomPlot* axes, int WidthMultiplier, int HeightMultiplier);

    void showPointOfInterest();



};


#endif // MAINWINDOW_H
