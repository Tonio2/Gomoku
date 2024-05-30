
#include "ai/gomoku_ai_datav2.h"
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace AI::MinMaxV2
{
GomokuAIData::GomokuAIData()
{
    values[StructureType::NONE] = 0;
    values[StructureType::FIVE_OR_MORE] = 100000;
    values[StructureType::OPEN_ONE] = 10;
    values[StructureType::ONE] = 5;
    values[StructureType::OPEN_TWO] = 100;
    values[StructureType::TWO] = 50;
    values[StructureType::OPEN_THREE] = 1000;
    values[StructureType::THREE] = 500;
    values[StructureType::OPEN_FOUR] = 10000;
    values[StructureType::FOUR] = 1000;
    values[Pattern::FORK] = 9000;
    values[Pattern::CAPTURE] = 50;
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
    return values[Pattern::FORK];
}

float GomokuAIData::value_of_captures(int capture_count) const
{
    return values[Pattern::CAPTURE] * (capture_count * capture_count);
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
}