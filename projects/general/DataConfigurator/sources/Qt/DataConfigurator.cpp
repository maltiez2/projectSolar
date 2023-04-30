#include "Qt/DataConfigurator.h"
#include "Qt/QScienceSpinBox.h"
#include "DataProcessor/MeasurementsSystem.h"

#include <QInputDialog>
#include <QPushButton>
#include <QList>
#include <QString>
#include <QSpinBox>

DataConfigurator::DataConfigurator(QWidget* parent)
    : QMainWindow(parent),
    unitsSystemManager({ "","default" })
{
    ui.setupUi(this);
    setupBasisTables();

    connect(ui.actionSave, &QAction::triggered, this, &DataConfigurator::saveUnitsSystem);
    connect(ui.actionOpen, &QAction::triggered, this, &DataConfigurator::loadUnitsSystem);
    connect(ui.buttons_basis_applyAndClear, SIGNAL(clicked(QAbstractButton*)), SLOT(basis_apply(QAbstractButton*)));
    connect(ui.buttons_basis_applyAndClear, SIGNAL(clicked(QAbstractButton*)), SLOT(basis_reset(QAbstractButton*)));
    connect(ui.tableWidget_otherUnits, SIGNAL(customContextMenuRequested(QPoint)), SLOT(otherUnits_table_menuRequested(QPoint)));
    connect(ui.pushButton_addRow, SIGNAL(clicked()), SLOT(otherUnits_table_addEmptyItem()));
}

void DataConfigurator::setupBasisTables()
{
    ui.tableWidget_baseCoherentUnits->setColumnCount(4);
    ui.tableWidget_baseCoherentUnits->setRowCount(measurementsSystem::dimensionsSpaceDimesion);
    ui.tableWidget_baseCoherentUnits->setHorizontalHeaderLabels({ "Qunatity title", "Quantity symbol", "Unit title", "Unit symbol"});
    ui.tableWidget_otherUnits->setEnabled(false);
    ui.pushButton_addRow->setEnabled(false);
}
void DataConfigurator::setupAdditionaUnitslTable()
{
    ui.tableWidget_otherUnits->setColumnCount(5 + measurementsSystem::dimensionsSpaceDimesion);
    QList<QString> columnsHeader = { "Qunatity title", "Qunatity symbol", "Unit title", "Unit symbol", "Convertion ratio"};
    for (quint8 id = 0; id < measurementsSystem::dimensionsSpaceDimesion; id++)
    {
        columnsHeader += unitsSystemManager.getUnitsSystem().quantities[id].symbol + " power";
    }
    ui.tableWidget_otherUnits->setHorizontalHeaderLabels(columnsHeader);
    ui.tableWidget_otherUnits->setContextMenuPolicy(Qt::CustomContextMenu);
    ui.tableWidget_otherUnits->setEnabled(true);
    ui.pushButton_addRow->setEnabled(true);
    ui.tableWidget_otherUnits->setRowCount(0);
}
void DataConfigurator::setupTablesValues()
{
    const measurementsSystem::UnitsSystem& system = unitsSystemManager.getUnitsSystem();
    if (system.symbol.isEmpty())
    {
        return;
    }

    ui.tableWidget_baseCoherentUnits->setRowCount(0);
    for (quint8 id = 0; id < measurementsSystem::dimensionsSpaceDimesion; id++)
    {
        baseQuantities_table_addItem({ system.quantities[id].title, system.quantities[id].symbol, system.units[id].title, system.units[id].symbol });
    }

    setupAdditionaUnitslTable();

    for (quint8 id = system.baseQuantities.size(); id < system.units.size(); id++)
    {
        const measurementsSystem::Unit& unit = system.units[id];
        const measurementsSystem::Quantity& quantity = system.quantities[system.units[id].quantityId];
        
        otherUnits_table_addItem({ quantity.symbol, unit.symbol, quantity.title, unit.title, unit.convertionRatioFromBaseUnits, quantity.dimension });
    }

    ui.tableWidget_baseCoherentUnits->setEnabled(false);
    ui.pushButton_addRow->setEnabled(true);
}

void DataConfigurator::addBasisUnitsToUnitsSystem()
{
    for (quint8 rowIndex = 0; rowIndex < ui.tableWidget_baseCoherentUnits->rowCount(); rowIndex++)
    {
        QString quantityTitle = ui.tableWidget_baseCoherentUnits->item(rowIndex, 0)->text();
        QString quantitySymbol = ui.tableWidget_baseCoherentUnits->item(rowIndex, 1)->text();
        QString unitTitle = ui.tableWidget_baseCoherentUnits->item(rowIndex, 2)->text();
        QString unitSymbol = ui.tableWidget_baseCoherentUnits->item(rowIndex, 3)->text();
        if (
            quantityTitle.isEmpty() or
            quantitySymbol.isEmpty() or
            unitTitle.isEmpty() or
            unitSymbol.isEmpty()
        )
        {
            ui.tableWidget_baseCoherentUnits->setEnabled(true);
            ui.buttons_basis_applyAndClear->setEnabled(true);
            return;
        }

        unitsSystemManager.modifiableUnitsSystem().addBaseUnit(quantitySymbol, unitSymbol, quantityTitle, unitTitle);
    }
}
void DataConfigurator::addOtherUnitsToUnitsSystem()
{
    quint8 size = ui.tableWidget_otherUnits->rowCount();
    for (quint8 rowIndex = 0; rowIndex < size; rowIndex++)
    {
        QString quantityTitle = ui.tableWidget_otherUnits->item(rowIndex, 0)->text();
        QString quantitySymbol = ui.tableWidget_otherUnits->item(rowIndex, 1)->text();
        QString unitTitle = ui.tableWidget_otherUnits->item(rowIndex, 2)->text();
        QString unitSymbol = ui.tableWidget_otherUnits->item(rowIndex, 3)->text();
        qreal convertionRatio = ((QScienceSpinBox*)ui.tableWidget_otherUnits->cellWidget(rowIndex, 4))->value();
        measurementsSystem::Dimension dim;
        for (quint8 dimIndex = 0; dimIndex < measurementsSystem::dimensionsSpaceDimesion; dimIndex++)
        {
            dim[dimIndex] = ((QSpinBox*)ui.tableWidget_otherUnits->cellWidget(rowIndex, 5 + dimIndex))->value();
        }
        unitsSystemManager.modifiableUnitsSystem().addAdditionalUnits(quantitySymbol, unitSymbol, quantityTitle, unitTitle, convertionRatio, dim);
    }
}

void DataConfigurator::baseQuantities_table_menuRequested(QPoint pos)
{
    auto* menu = new QMenu(this);

    if (ui.tableWidget_baseCoherentUnits->itemAt(pos) == nullptr)
    {
        QAction* action = new QAction("Add quantity", this);
        connect(action, SIGNAL(triggered()), SLOT(baseQuantities_table_addItem()));
        menu->addAction(action);
    }
    else
    {
        QModelIndex index = ui.tableWidget_baseCoherentUnits->indexAt(pos);
        QAction* action = new QAction("Delete quantity" , this);
        connect(action, &QAction::triggered, [=]() { ui.tableWidget_baseCoherentUnits->removeRow(index.row()); });
        menu->addAction(action);
    }

    menu->popup(ui.tableWidget_baseCoherentUnits->viewport()->mapToGlobal(pos));
}
void DataConfigurator::baseQuantities_table_addItem(BaseUnitInitData unitData)
{
    ui.tableWidget_baseCoherentUnits->insertRow(ui.tableWidget_baseCoherentUnits->rowCount());
    ui.tableWidget_baseCoherentUnits->setItem(ui.tableWidget_baseCoherentUnits->rowCount() - 1, 0, new QTableWidgetItem(unitData.qunatityTitle));
    ui.tableWidget_baseCoherentUnits->setItem(ui.tableWidget_baseCoherentUnits->rowCount() - 1, 1, new QTableWidgetItem(unitData.quantitySymbol));
    ui.tableWidget_baseCoherentUnits->setItem(ui.tableWidget_baseCoherentUnits->rowCount() - 1, 2, new QTableWidgetItem(unitData.unitTitle));
    ui.tableWidget_baseCoherentUnits->setItem(ui.tableWidget_baseCoherentUnits->rowCount() - 1, 3, new QTableWidgetItem(unitData.unitSymbol));
}
void DataConfigurator::basis_apply(QAbstractButton* button)
{
    if (ui.buttons_basis_applyAndClear->buttonRole(button) != QDialogButtonBox::ButtonRole::ApplyRole)
    {
        return;
    }

    if (ui.tableWidget_baseCoherentUnits->rowCount() == 0)
    {
        return;
    }
    
    ui.tableWidget_baseCoherentUnits->setEnabled(false);
    ui.buttons_basis_applyAndClear->setEnabled(false);
    ui.pushButton_addRow->setEnabled(false);
    
    addBasisUnitsToUnitsSystem();
    setupAdditionaUnitslTable();

    ui.buttons_basis_applyAndClear->setEnabled(true);
    ui.tableWidget_otherUnits->setEnabled(true);
    ui.pushButton_addRow->setEnabled(true);
}
void DataConfigurator::basis_reset(QAbstractButton* button)
{
    if (ui.buttons_basis_applyAndClear->buttonRole(button) != QDialogButtonBox::ButtonRole::ResetRole)
    {
        return;
    }

    ui.tableWidget_otherUnits->setRowCount(0);
    ui.tableWidget_baseCoherentUnits->setRowCount(measurementsSystem::dimensionsSpaceDimesion);
    unitsSystemManager.modifiableUnitsSystem() = measurementsSystem::UnitsSystem();
    ui.tableWidget_baseCoherentUnits->setEnabled(true);
    ui.tableWidget_otherUnits->setEnabled(false);
    ui.pushButton_addRow->setEnabled(false);
}
void DataConfigurator::otherUnits_table_menuRequested(QPoint pos)
{
    auto* menu = new QMenu(this);

    if (ui.tableWidget_otherUnits->itemAt(pos) == nullptr)
    {
        QAction* action = new QAction("Add quantity", this);
        connect(action, SIGNAL(triggered()), SLOT(otherUnits_table_addEmptyItem()));
        menu->addAction(action);
    }
    else
    {
        QModelIndex index = ui.tableWidget_otherUnits->indexAt(pos);
        QAction* action = new QAction("Delete quantity", this);
        connect(action, &QAction::triggered, [=]() { ui.tableWidget_otherUnits->removeRow(index.row()); });
        menu->addAction(action);
    }

    menu->popup(ui.tableWidget_otherUnits->viewport()->mapToGlobal(pos));
}
void DataConfigurator::otherUnits_table_addEmptyItem()
{
    auto dim = measurementsSystem::Dimension();
    dim.setZero();
    otherUnits_table_addItem({ "", "", "", "", 1.0, dim });
}
void DataConfigurator::otherUnits_table_addItem(UnitInitData unitData)
{
    ui.tableWidget_otherUnits->insertRow(ui.tableWidget_otherUnits->rowCount());
    ui.tableWidget_otherUnits->setItem(ui.tableWidget_otherUnits->rowCount() - 1, 0, new QTableWidgetItem(unitData.qunatityTitle));
    ui.tableWidget_otherUnits->setItem(ui.tableWidget_otherUnits->rowCount() - 1, 1, new QTableWidgetItem(unitData.quantitySymbol));
    ui.tableWidget_otherUnits->setItem(ui.tableWidget_otherUnits->rowCount() - 1, 2, new QTableWidgetItem(unitData.unitTitle));
    ui.tableWidget_otherUnits->setItem(ui.tableWidget_otherUnits->rowCount() - 1, 3, new QTableWidgetItem(unitData.unitSymbol));
    ui.tableWidget_otherUnits->setCellWidget(ui.tableWidget_otherUnits->rowCount() - 1, 4, new QScienceSpinBox());
    ((QScienceSpinBox*)ui.tableWidget_otherUnits->cellWidget(ui.tableWidget_otherUnits->rowCount() - 1, 4))->setValue(unitData.convertionRation);
    for (quint8 index = 0; index < measurementsSystem::dimensionsSpaceDimesion; index++)
    {
        ui.tableWidget_otherUnits->setCellWidget(ui.tableWidget_otherUnits->rowCount() - 1, 5 + index, new QSpinBox());
        auto item = ((QSpinBox*)ui.tableWidget_otherUnits->cellWidget(ui.tableWidget_otherUnits->rowCount() - 1, 5 + index));
        item->setMinimum(-item->maximum());
        item->setValue(unitData.quantityDimension[index]);
    }
}

void DataConfigurator::saveUnitsSystem()
{
    auto& system = unitsSystemManager.modifiableUnitsSystem();
    system = measurementsSystem::UnitsSystem();
    system.symbol = ui.lineEdit_unitsSystemSymbol->text();

    addBasisUnitsToUnitsSystem();
    addOtherUnitsToUnitsSystem();

    unitsSystemManager.writeToFile();
}
void DataConfigurator::loadUnitsSystem()
{
    QInputDialog dialog = QInputDialog(this);
    dialog.setOptions(QInputDialog::UseListViewForComboBoxItems);
    dialog.setComboBoxItems(unitsSystemManager.getUnitsSystemsSymbols());
    dialog.setWindowTitle("Choose system symbol");
    if (dialog.exec())
    {
        unitsSystemManager.getUnitsSystem(dialog.textValue());
    }
    ui.lineEdit_unitsSystemSymbol->setText(unitsSystemManager.getUnitsSystem().symbol);
    setupTablesValues();
}
