#include "gtest/gtest.h"
#include "engine/gomoku_engine.h"
#include "utils/gomoku_utilities.h"

std::vector<std::vector<Player>> generate_flanked_structures(std::vector<Player> s, StructureType type)
{
    std::vector<std::vector<Player>> ret = {};
    std::vector<Player> flanks = {X, E, O};
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            bool push = true;
            std::vector<Player> flanked_struct = {flanks[i]};
            flanked_struct.insert(flanked_struct.end(), s.begin(), s.end());
            flanked_struct.push_back(flanks[j]);

            if (type == StructureType::TWO || type == StructureType::OPEN_TWO)
            {
                if ((flanks[i] == O && s[0] == E) || (flanks[j] == O && s[s.size() - 1] == E))
                {
                    push = false;
                }
            }

            if (push)
                ret.push_back(flanked_struct);
        }
    }

    return ret;
}

bool test_struct(std::vector<Player> s, StructureType expected_type)
{
    GomokuGame game(s.size(), 1);

    for (int i = 0; i < s.size(); i++)
        game.set_board_value(0, i, s[i]);

    GomokuPatternReconizer reconizer(O);

    reconizer.find_patterns_in_board(game);

    GomokuCellIndex idx(0, 2);

    StructureType type = reconizer.get_structure_at(idx, PatternDirection::LeftToRight).first;

    if (type != expected_type)
    {
        std::cout << "Found " << type << " but expected " << expected_type;
        return false;
    }

    return true;
}

bool test_walled_struct(std::vector<Player> s, StructureType expected_type)
{
    GomokuGame game(s.size(), 1);

    for (int i = 0; i < s.size(); i++)
        game.set_board_value(0, i, s[i]);

    GomokuPatternReconizer reconizer(O);

    reconizer.find_patterns_in_board(game);

    GomokuCellIndex idx(0, 1);

    StructureType type = reconizer.get_structure_at(idx, PatternDirection::LeftToRight).first;

    if (type != expected_type)
    {
        std::cout << "Found " << type << " but expected " << expected_type;
        return false;
    }

    return true;
}

void test_get_structure_at()
{

    std::vector<std::pair<std::vector<Player>, StructureType>> structures = {
        {{E, O, X}, StructureType::ONE},
        {{X, O, E}, StructureType::ONE},
        {{E, O, E}, StructureType::OPEN_ONE},
        {{E, O, O, X}, StructureType::TWO},
        {{X, O, O, E}, StructureType::TWO},
        {{E, O, O, E}, StructureType::OPEN_TWO},
        {{E, O, O, O, X}, StructureType::THREE},
        {{X, O, O, O, E}, StructureType::THREE},
        {{E, O, O, O, E}, StructureType::OPEN_THREE},
        {{E, O, E, O, O, X}, StructureType::THREE},
        {{X, O, E, O, O, E}, StructureType::THREE},
        {{E, O, E, O, O, E}, StructureType::OPEN_THREE},
        {{E, O, O, E, O, X}, StructureType::THREE},
        {{X, O, O, E, O, E}, StructureType::THREE},
        {{E, O, O, E, O, E}, StructureType::OPEN_THREE},
        {{E, O, O, O, O, X}, StructureType::FOUR},
        {{X, O, O, O, O, E}, StructureType::FOUR},
        {{E, O, O, O, O, E}, StructureType::OPEN_FOUR},
        {{E, O, O, O, O, O, X}, StructureType::FIVE_OR_MORE},
        {{X, O, O, O, O, O, E}, StructureType::FIVE_OR_MORE},
        {{E, O, O, O, O, O, E}, StructureType::FIVE_OR_MORE},
    };

    for (int i = 0; i < structures.size(); i++)
    {
        std::cout << "TEMPLATE : " << structures[i].first << " " << structures[i].second << std::endl;
        std::vector<std::vector<Player>> flanked_structures = generate_flanked_structures(structures[i].first, structures[i].second);
        for (std::vector<Player> flanked_structure : flanked_structures)
        {
            std::cout << flanked_structure << " : ";
            if (test_struct(flanked_structure, structures[i].second))
                std::cout << "OK";
            std::cout << std::endl;
        }
        std::cout << "|" << structures[i].first << "|" << " : ";
        if (test_walled_struct(structures[i].first, structures[i].second))
            std::cout << "OK";
        std::cout << std::endl;
    }
}

TEST(Structures, AllCombinaison)
{
}