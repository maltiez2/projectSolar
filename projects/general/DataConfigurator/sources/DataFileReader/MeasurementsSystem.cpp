#include "DataProcessor/MeasurementsSystem.h"

#include <QByteArray>
#include <QFile>
#include <QTextStream>
#include <QVariant>
#include <QList>
#include <QJsonValue>
#include <QPair>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDir>

using namespace measurementsSystem;

bool Quantity::isEquivalent(const Quantity& first, const Quantity& second)
{
	return first.isEquivalentTo(second);
}
bool Unit::isEquivalent(const Unit& first, const Unit& second, const UnitsSystem& system)
{
	return first.isEquivalentTo(second, system);
}


QJsonObject Quantity::toJson() const
{
	QJsonArray dimensionList;
	for (quint8 index = 0; index < dimensionsSpaceDimesion; index++)
	{
		dimensionList.push_back(dimension[index]);
	}

	return {
		{"symbol", symbol},
		{"title", title},
		{"dimension", dimensionList}
	};
}
Quantity Quantity::fromJson(const QJsonObject& jsonObject)
{
	return { 
		jsonObject.value("symbol").toString(),
		jsonObject.value("title").toString(),
		getDimensionFormObject(jsonObject) 
	};
}
bool Quantity::isEquivalentTo(const Quantity& quantity) const
{
	return dimension == quantity.dimension;
}
Dimension Quantity::getDimensionFormObject(const QJsonObject& jsonObject)
{
	const QJsonArray& jsonArray = jsonObject.value("dimension").toArray();
	Dimension output;
	for (quint8 index = 0; index < dimensionsSpaceDimesion; index++)
	{
		double value = jsonArray[index].toDouble();;
		output[index] = jsonArray[index].toDouble();
	}
	return output;
}


QJsonObject Unit::toJson() const
{
	return {
		{"symbol", symbol},
		{"title", title},
		{"quantity", quantityId},
		{"convertionRatioFromBaseUnits", convertionRatioFromBaseUnits}
	};
}
Unit Unit::fromJson(const QJsonObject& jsonObject)
{
	return {
		jsonObject.value("symbol").toString(),
		jsonObject.value("title").toString(),
		(quint8)jsonObject.value("quantity").toInt(),
		jsonObject.value("convertionRatioFromBaseUnits").toDouble()
	};
}
bool Unit::isEquivalentTo(const Unit& unit, const UnitsSystem& system) const
{
	const Quantity& thisQuantity = system.quantities[quantityId];
	const Quantity& inputQuantity = system.quantities[quantityId];
	return thisQuantity.isEquivalentTo(inputQuantity) && (convertionRatioFromBaseUnits == unit.convertionRatioFromBaseUnits);
}


QJsonObject UnitsSystem::toJson() const
{
	QJsonObject jsonBaseQuantities;
	foreach (quint8 id, baseQuantities)
		jsonBaseQuantities.insert(QString::number(id), quantities[id].toJson());

	QJsonObject jsonDerivedQuantities;
	foreach(quint8 id, derivedQuantities)
		jsonDerivedQuantities.insert(QString::number(id), quantities[id].toJson());

	QJsonObject jsonCoherentUnits;
	foreach (quint8 id, coherentUnits)
		jsonCoherentUnits.insert(QString::number(id), units[id].toJson());

	QJsonObject jsonNonCoherentUnits;
	foreach(quint8 id, nonCoherentUnits)
		jsonNonCoherentUnits.insert(QString::number(id), units[id].toJson());

	return {
		{"symbol", symbol},
		{"baseQuantities", jsonBaseQuantities},
		{"derivedQuantities", jsonDerivedQuantities},
		{"coherentUnits", jsonCoherentUnits},
		{"nonCoherentUnits", jsonNonCoherentUnits}
	};
}
UnitsSystem UnitsSystem::fromJson(const QJsonObject& jsonObject)
{
	UnitsSystem unitSystem;
	
	unitSystem.symbol = jsonObject.value("symbol").toString();
	
	QJsonObject baseQuantitiesArray = jsonObject.value("baseQuantities").toObject();
	QJsonObject derivedQuantitiesArray = jsonObject.value("derivedQuantities").toObject();
	QJsonObject jsonCoherentUnits = jsonObject.value("coherentUnits").toObject();
	QJsonObject jsonNonCoherentUnits = jsonObject.value("nonCoherentUnits").toObject();

	foreach(const QString& key, baseQuantitiesArray.keys())
	{
		quint8 id = key.toInt();
		unitSystem.quantities.insert(id, Quantity::fromJson((baseQuantitiesArray[key].toObject())));
		unitSystem.baseQuantities.insert(id);
		unitSystem.quantitiesBySymbol.insert(unitSystem.quantities[id].symbol, id);
	}
	foreach(const QString& key, derivedQuantitiesArray.keys())
	{
		quint8 id = key.toInt();
		unitSystem.quantities.insert(id, Quantity::fromJson((derivedQuantitiesArray[key].toObject())));
		unitSystem.derivedQuantities.insert(id);
		unitSystem.quantitiesBySymbol.insert(unitSystem.quantities[id].symbol, id);
	}
	foreach(const QString& key, jsonCoherentUnits.keys())
	{
		quint8 id = key.toInt();
		unitSystem.units.insert(id, Unit::fromJson(jsonCoherentUnits[key].toObject()));
		unitSystem.coherentUnits.insert(id);
		unitSystem.unitsBySymbol.insert(unitSystem.units[id].symbol, id);
		unitSystem.quantitiesByUnits.insert(id, unitSystem.units[id].quantityId);
		if (not unitSystem.coherentUnitsByQuantities.contains(unitSystem.units[id].quantityId))
		{
			unitSystem.coherentUnitsByQuantities.insert(unitSystem.units[id].quantityId, id);
		}
	}
	foreach(const QString& key, jsonNonCoherentUnits.keys())
	{
		quint8 id = key.toInt();
		unitSystem.units.insert(id, Unit::fromJson(jsonNonCoherentUnits[key].toObject()));
		unitSystem.nonCoherentUnits.insert(id);
		unitSystem.unitsBySymbol.insert(unitSystem.units[id].symbol, id);
		unitSystem.quantitiesByUnits.insert(id, unitSystem.units[id].quantityId);
	}

	return unitSystem;
}
void UnitsSystem::addBaseUnit(QString baseQuantitySymbol, QString baseCoherentUnitSymbol, QString baseQuantityTitle, QString baseCoherentUnitTitle)
{
	quint8 id = quantities.size();
	auto quantityDimension = Dimension({0,0,0,0});
	quantityDimension[id] = 1;
	quantities.insert(id, { baseQuantitySymbol, baseQuantityTitle, quantityDimension });
	baseQuantities.insert(id);
	quantitiesBySymbol.insert(baseQuantitySymbol, id);

	units.insert(id, { baseCoherentUnitSymbol, baseCoherentUnitTitle, id, 1 });
	unitsBySymbol.insert(baseCoherentUnitSymbol, id);
	coherentUnits.insert(id);

	quantitiesByUnits.insert(id, id);
	coherentUnitsByQuantities.insert(id, id);
}
void UnitsSystem::addAdditionalUnits(QString quantitySymbol, QString unitSymbol, QString quantityTitle, QString unitTitle, qreal convertionRatio, Dimension qunatityDimension)
{
	if (unitsBySymbol.contains(unitSymbol))
	{
		return;
	}
	
	if (not quantitiesBySymbol.contains(quantitySymbol))
	{
		quint8 quantityId = quantities.size();
		quantities.insert(quantityId, { quantitySymbol, quantityTitle, qunatityDimension });
		quantitiesBySymbol.insert(quantitySymbol, quantityId);
		derivedQuantities.insert(quantityId);
	}

	const Quantity& quantity = quantities[quantitiesBySymbol[quantitySymbol]];

	if (quantity.dimension != qunatityDimension)
	{
		// @TODO Add exception
		return;
	}

	quint8 quantityId = quantitiesBySymbol[quantitySymbol];
	quint8 unitId = units.size();
	units.insert(unitId, { unitSymbol, unitTitle, quantityId, convertionRatio });
	unitsBySymbol.insert(unitSymbol, unitId);
	quantitiesByUnits.insert(unitId, quantityId);
	if (convertionRatio == 1.0) // @TODO Floating point comparison
	{
		coherentUnits.insert(unitId);

		if (not coherentUnitsByQuantities.contains(quantityId))
		{
			coherentUnitsByQuantities.insert(quantityId, unitId);
		}
	}
	else
	{
		nonCoherentUnits.insert(unitId);
	}
}

UnitsConverter::UnitsConverter(const UnitsSystem& system) :
	c_unitsSystem(system)
{

}
Value UnitsConverter::convert(const Value& inputValue, const Unit& outputUnit) const
{
	// @TODO Thrown an exception on unitsSystem does not containing 'outputUnit' or 'inputValue.unit'
	return { inputValue.value * outputUnit.convertionRatioFromBaseUnits / inputValue.unit.convertionRatioFromBaseUnits, outputUnit };
}
Value UnitsConverter::toCoherent(const Value& inputValue) const
{
	// @TODO Thrown an exception on unitsSystem does not containing 'inputValue.unit'
	quint8 quantityId = inputValue.unit.quantityId;
	quint8 coherentUnitId = c_unitsSystem.coherentUnitsByQuantities[quantityId];
	return {
		inputValue.value / inputValue.unit.convertionRatioFromBaseUnits,
		c_unitsSystem.units[coherentUnitId]
	};
}


ConfigurationFileManager::ConfigurationFileManager(const ConfigurationFileManagerParameters& inputParameters) :
	c_parameters(inputParameters)
{
}
bool ConfigurationFileManager::writeIntoFile(const UnitsSystem& system)
{
	// @TODO Add check for folder existence

	QJsonDocument document;
	document.setObject(system.toJson());
	QByteArray bytes = document.toJson(QJsonDocument::Indented);
	QString filePath = toPath(system.symbol, c_parameters.folderPath);
	QFile file(filePath);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
	{
		return false;
	}

	QTextStream iStream(&file);
	iStream.setEncoding(QStringConverter::Encoding::Utf8);
	iStream << bytes;
	file.close();

	if (not m_systemsSymbols.contains(system.symbol))
	{
		m_systemsSymbols.push_back(system.symbol);
	}

	return true;
}
UnitsSystem ConfigurationFileManager::readFromFile(const QString& unitsSystemSymbol) const
{
	// @TODO Add check for folder existence

	QFile file(toPath(unitsSystemSymbol, c_parameters.folderPath));

	if (!file.open(QIODevice::ReadOnly))
	{
		// @TODO Throw an exception
		//return;
	}

	QByteArray bytes = file.readAll();
	file.close();

	QJsonParseError jsonError;
	QJsonDocument document = QJsonDocument::fromJson(bytes, &jsonError);
	if (jsonError.error != QJsonParseError::NoError)
	{
		// @TODO Throw an exception
		//return;
	}

	if (!document.isObject())
	{
		// @TODO Throw an exception
		//return;
	}

	return UnitsSystem::fromJson(document.object());
}
const QList<QString>& ConfigurationFileManager::getUnitsSystemSymbols(bool reload)
{
	if (not m_systemsSymbols.isEmpty() and not reload)
	{
		return m_systemsSymbols;
	}
	
	QFileInfoList fileList = QDir(c_parameters.folderPath).entryInfoList(QDir::Files);
	m_systemsSymbols.clear();
	foreach(auto entry, fileList)
		if (entry.filesystemFilePath().extension().string() == "." + c_parameters.extension.toStdString())
		{
			QString fileNameStem = QString::fromStdString(entry.filesystemFilePath().stem().string());
			m_systemsSymbols.push_back(fileNameStem);
		}

	return m_systemsSymbols;
}
QString ConfigurationFileManager::toPath(const QString& unitsSystemSymbol, const QString& folderPath) const
{
	QString path = unitsSystemSymbol + "." + c_parameters.extension;
	if (not folderPath.isEmpty())
	{
		path = folderPath + "/" + path;
	}
	return path;
}


MeasurementSystemManager::MeasurementSystemManager(const MeasurementSystemManagerParameters& inputParameters) :
	m_parameters(inputParameters),
	m_configurationFileManager({ m_parameters.configsFolderPath }),
	m_unitsSystem(m_configurationFileManager.readFromFile(m_parameters.defaultSystemSymbol))
{

}
UnitsSystem& MeasurementSystemManager::modifiableUnitsSystem()
{
	return m_unitsSystem;
}
const UnitsSystem& MeasurementSystemManager::getUnitsSystem(QString symbol, bool reload)
{
	if (symbol == "")
	{
		symbol = m_unitsSystem.symbol;
	}
	
	if (reload or (symbol != m_unitsSystem.symbol))
	{
		m_unitsSystem = UnitsSystem(m_configurationFileManager.readFromFile(symbol));
	}

	return m_unitsSystem;
}
const QList<QString>& MeasurementSystemManager::getUnitsSystemsSymbols(bool reload)
{
	return m_configurationFileManager.getUnitsSystemSymbols(reload);
}
bool MeasurementSystemManager::writeToFile()
{
	return m_configurationFileManager.writeIntoFile(m_unitsSystem);
}
