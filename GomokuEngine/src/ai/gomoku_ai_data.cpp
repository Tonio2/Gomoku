
#include "ai/gomoku_ai_data.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <stdexcept>
#include <sstream>

namespace AI::MinMaxV2
{

    GomokuAIData::GomokuAIData()
    {
        values[StructureType::NONE] = 0;
        values[StructureType::FIVE_OR_MORE] = 3000;
        values[StructureType::OPEN_ONE] = 500;
        values[StructureType::ONE] = 250;
        values[StructureType::OPEN_TWO] = 666;
        values[StructureType::TWO] = 333;
        values[StructureType::OPEN_THREE] = 1000;
        values[StructureType::THREE] = 500;
        values[StructureType::OPEN_FOUR] = 2000;
        values[StructureType::FOUR] = 1000;
        values[10] = 9000;
        values[11] = 9000;
        values[12] = 10;
        values[13] = 1;
        values[14] = 1;
    }

    GomokuAIData::GomokuAIData(const GomokuAIData &copy)
    {
        for (int i = 0; i < VALUES_COUNT; i++)
            values[i] = copy.values[i];
    }

    GomokuAIData &GomokuAIData::operator=(const GomokuAIData &copy)
    {
        if (this != &copy)
        {
            for (int i = 0; i < VALUES_COUNT; i++)
                values[i] = copy.values[i];
        }
        return *this;
    }

    GomokuAIData::~GomokuAIData()
    {
    }

    void GomokuAIData::save_to_file(std::string filename) const
    {
        namespace fs = std::filesystem;
        fs::path filepath(filename);
        std::string directory = filepath.parent_path().string();

        if (!directory.empty() && !fs::exists(directory))
        {
            if (!fs::create_directories(directory))
            {
                throw std::runtime_error("Error: Unable to create directory: " + directory);
            }
        }

        std::ofstream file(filename);
        if (!file.is_open())
        {
            throw std::runtime_error("Error: Unable to open file for writing: " + filename);
        }

        file << *this << std::endl;

        file.close();
    }

    void GomokuAIData::load_from_file(std::string filename)
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            throw std::runtime_error("Error: Unable to open file for reading: " + filename);
        }

        std::string line;
        if (std::getline(file, line))
        {
            std::stringstream ss(line);
            std::string cell;
            int i = 0;
            while (std::getline(ss, cell, ',') && i < VALUES_COUNT)
            {
                values[i++] = std::stof(cell);
            }
        }
        else
        {
            throw std::runtime_error("Error: File is empty: " + filename);
        }

        file.close();
    }

    float GomokuAIData::value_of_structure(int structure_type) const
    {
        return values[structure_type];
    }

    float GomokuAIData::value_of_multiple_forced() const
    {
        return values[STC + 0];
    }

    float GomokuAIData::value_of_multiple_o4() const
    {
        return values[STC + 1];
    }

    float GomokuAIData::value_of_captures(int capture_count, int half_open_two_count) const
    {
        int a = 4 - capture_count;
        int b = 10 - 2 * capture_count;
        int c = 5 - capture_count;

        return 1000 * (std::min(half_open_two_count - a, 1) / std::max(b - half_open_two_count, c));
    }

    std::ostream &operator<<(std::ostream &stream, const GomokuAIData &ai_data)
    {
        for (int i = 0; i < GomokuAIData::VALUES_COUNT; ++i)
        {
            stream << ai_data.values[i];
            if (i < GomokuAIData::VALUES_COUNT - 1)
                stream << ",";
        }
        return stream;
    }

} // namespace AI::MinMaxV2
