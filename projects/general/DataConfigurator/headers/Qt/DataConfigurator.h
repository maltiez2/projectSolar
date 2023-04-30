#pragma once

#include <QtWidgets/QMainWindow>
#include <QDialog>

#include "ui_DataConfigurator.h"

#include "DataProcessor/MeasurementsSystem.h"
#include "Qt/QScienceSpinBox.h"

class DataConfigurator : public QMainWindow
{
    Q_OBJECT

public:
    DataConfigurator(QWidget *parent = nullptr);
    ~DataConfigurator() = default;

    struct BaseUnitInitData
    {
        QString qunatityTitle;
        QString quantitySymbol;
        QString unitTitle;
        QString unitSymbol;
    };
    struct UnitInitData
    {
        QString quantitySymbol;
        QString unitSymbol;
        QString qunatityTitle;
        QString unitTitle;
        qreal convertionRation;
        measurementsSystem::Dimension quantityDimension;
    };

public slots:
    void baseQuantities_table_menuRequested(QPoint pos);
    void baseQuantities_table_addItem(BaseUnitInitData unitData = {"", "", "", ""});
    void basis_apply(QAbstractButton* button);
    void basis_reset(QAbstractButton* button);
    void otherUnits_table_menuRequested(QPoint pos);
    void otherUnits_table_addEmptyItem();
    void otherUnits_table_addItem(UnitInitData unitData);
    void saveUnitsSystem();
    void loadUnitsSystem();

private:
    Ui::DataConfiguratorClass ui;
    measurementsSystem::MeasurementSystemManager unitsSystemManager;

    void setupBasisTables();
    void setupAdditionaUnitslTable();
    void setupTablesValues();
    void addBasisUnitsToUnitsSystem();
    void addOtherUnitsToUnitsSystem();
};
