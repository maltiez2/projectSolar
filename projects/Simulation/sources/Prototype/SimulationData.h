#pragma once

#include "DoubleBufferedContainer.h"

#include <Eigen/Eigen>

#include <shared_mutex>
#include <array>
#include <optional>


namespace projectSolar::Simulation
{
	class Serializer;

	/*
		Contains data for celestial objects like stars, planets, moons, artificial objects.
		Double buffered.
	*/
	class ClestialObjectsData
	{
	public:
		struct MotionData
		{
			double mass;
			Eigen::Vector3d position;
			Eigen::Vector3d velocity;
			Eigen::Vector3d externalForce;
		};

		struct ObjectId
		{
			size_t index;
			uint64_t version;
			uint8_t objectClass;
		};

		enum celestialObjectClass : uint8_t
		{
			BIG_ATTRACTORS,
			SMALL_ATTRACTORS,
			NOT_ATTRACING_OBJECTS,
			CELESTIAL_OBJECTS_CLASSES_NUMBER
		};

		std::shared_mutex dataMutex;
		std::array<DataStructures::DoubleBuffVectorWithVersions<MotionData>, CELESTIAL_OBJECTS_CLASSES_NUMBER> celestialObjects;

		ObjectId add(uint8_t objectClass, double mass, Eigen::Vector3d position, Eigen::Vector3d velocity, Eigen::Vector3d externalForce);
		std::optional<MotionData> get(ObjectId id);
		bool delIfExists(ObjectId id);
		bool exists(ObjectId id);

		void save(Serializer& serializer);
		void load(Serializer& serializer);
		void swap();
	};

	/*
		Contains all the data for all systems.
		Should be provided to Core for access to data.
		Should be double buffered.
		Manages saving and loading.
	*/
	class SimulationData
	{
	public:
		std::shared_mutex swapMutex;

		ClestialObjectsData celestialObjects;

		void save(Serializer& serializer);
		void load(Serializer& serializer);
	};
}
