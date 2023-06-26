#include "pch.h"

#include "SimulationData.h"
#include "Serializer.h"


namespace projectSolar::Simulation
{
	// ClestialObjectsData
	ClestialObjectsData::ObjectId ClestialObjectsData::add(uint8_t objectClass, double mass, Eigen::Vector3d position, Eigen::Vector3d velocity, Eigen::Vector3d externalForce)
	{
		std::unique_lock lock(dataMutex);
		size_t index = celestialObjects[objectClass].addElement(
			{
				mass,
				position,
				velocity,
				externalForce
			}
		);
		return {
			index,
			celestialObjects[objectClass].getDataVersion(index),
			objectClass
		};
	}
	std::optional<ClestialObjectsData::MotionData> ClestialObjectsData::get(ObjectId id)
	{
		std::shared_lock lock(dataMutex);
		if (!celestialObjects[id.objectClass].isValid(id.index) || celestialObjects[id.objectClass].getDataVersion(id.index) != id.version)
		{
			return {};
		}

		return celestialObjects[id.objectClass].getData()[id.index].value;
	}
	bool ClestialObjectsData::delIfExists(ObjectId id)
	{
		std::unique_lock lock(dataMutex);
		if (!celestialObjects[id.objectClass].isValid(id.index) || celestialObjects[id.objectClass].getDataVersion(id.index) != id.version)
		{
			return false;
		}

		return celestialObjects[id.objectClass].delElement(id.index);
	}
	bool ClestialObjectsData::exists(ObjectId id)
	{
		std::shared_lock lock(dataMutex);
		return celestialObjects[id.objectClass].isValid(id.index) && celestialObjects[id.objectClass].getDataVersion(id.index) == id.version;
	}

	void ClestialObjectsData::save(Serializer& serializer)
	{
		for (auto container : celestialObjects)
		{
			serializer.serialize(container);
		}
	}
	void ClestialObjectsData::load(Serializer& serializer)
	{
		for (auto container : celestialObjects)
		{
			serializer.deserialize(container);
		}
	}
	void ClestialObjectsData::swap()
	{
		for (auto container : celestialObjects)
		{
			container.swap();
		}
	}

	// SimulationData
	void SimulationData::save(Serializer& serializer)
	{
		PROFILE_FUNCTION();

		celestialObjects.save(serializer);
	}
	void SimulationData::load(Serializer& serializer)
	{
		PROFILE_FUNCTION();
		
		celestialObjects.load(serializer);
	}
}
