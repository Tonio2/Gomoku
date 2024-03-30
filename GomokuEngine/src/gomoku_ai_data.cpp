
#include "gomoku_ai_data.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <stdexcept>
#include <sstream>

GomokuAIData::GomokuAIData()
{
    values[0] = 0;
    values[1] = 100000;
    values[2] = 10;
    values[3] = 5;
    values[4] = 100;
    values[5] = 50;
    values[6] = 1000;
    values[7] = 500;
    values[8] = 10000;
    values[9] = 1000;
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

float GomokuAIData::value_of_captures(int capture_count) const
{
    const float a = values[STC + 2];
    const float b = values[STC + 3];
    const float c = values[STC + 4];

    return a * (capture_count * capture_count) + b * (capture_count) + c;
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