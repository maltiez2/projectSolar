#pragma once

#include "EntityComponentSystem.h"
#include "Logger.h"

#include <stdio.h>

namespace projectSolar::ECS
{    
    class Serializer
    {
    public:
        template<typename ... ComponentsList>
        static errno_t serialize(const entt::registry& storage, std::string_view filePath)
        {
            SerializerImpl ser;
            
            auto error = fopen_s(&ser.p_file, filePath.data(), "wb");
            if (error)
            {
                LOG_ERROR(std::string("[ECS] [Serializer] Error on opening file for writing: ") + filePath.data());
                return error;
            }
            entt::snapshot{ storage }
                .entities(ser)
                .component<ComponentsList...>(ser);
            std::fclose(ser.p_file);
            return 0;
        }
        template<typename ... ComponentsList>
        static errno_t deserialize(entt::registry& storage, std::string_view filePath)
        {
            DeserializerImpl des;

            auto error = fopen_s(&des.p_file, filePath.data(), "rb");
            if (error)
            {
                LOG_ERROR(std::string("[ECS] [Serializer] Error on opening file for reading: ") + filePath.data());
                return error;
            }
            entt::snapshot_loader{ storage }
                .entities(des)
                .component<ComponentsList...>(des);
            std::fclose(des.p_file);
            return 0;
        }

        Serializer() = delete;

    private:
        class SerializerImpl
        {
        public:
            std::FILE* p_file = nullptr;

            template<typename idType = entt::entity>
            void operator()(std::underlying_type_t<idType> numberOfRecords) const
            {
                std::fwrite(&numberOfRecords, sizeof(numberOfRecords), 1, p_file);
            }
            template<typename idType = entt::entity>
            void operator()(idType id) const
            {
                std::fwrite(&id, sizeof(id), 1, p_file);
            }
            template<typename Component>
            void operator()(const entt::entity id, const Component& data) const
            {
                std::fwrite(&id, sizeof(id), 1, p_file);
                std::fwrite(&data, sizeof(data), 1, p_file);
            }
        };
        class DeserializerImpl
        {
        public:
            std::FILE* p_file = nullptr;

            template<typename idType = entt::entity>
            void operator()(std::underlying_type_t<idType>& numberOfRecords) const
            {
                std::fread(&numberOfRecords, sizeof(numberOfRecords), 1, p_file);
            }
            template<typename idType = entt::entity>
            void operator()(idType& id) const
            {
                std::fread(&id, sizeof(id), 1, p_file);
            }
            template<typename Component>
            void operator()(entt::entity& id, Component& data) const
            {
                std::fread(&id, sizeof(id), 1, p_file);
                std::fread(&data, sizeof(data), 1, p_file);
            }
        };
    };
}