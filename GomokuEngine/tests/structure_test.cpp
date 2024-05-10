#include "gtest/gtest.h"
#include "engine/gomoku_engine.h"
#include "utils/gomoku_utilities.h"

static const std::vector<std::pair<std::vector<Player>, StructureType>> structures_list = {
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

std::vector<std::vector<Player>> generate_flanked_structures(std::vector<Player> structure, StructureType type)
{
    std::vector<std::vector<Player>> ret = {};
    std::vector<Player> flanks = {X, E, O};
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            bool push = true;
            std::vector<Player> flanked_struct = {flanks[i]};
            flanked_struct.insert(flanked_struct.end(), structure.begin(), structure.end());
            flanked_struct.push_back(flanks[j]);

            if (type == StructureType::TWO || type == StructureType::OPEN_TWO)
            {
                if ((flanks[i] == O && structure[0] == E) || (flanks[j] == O && structure[structure.size() - 1] == E))
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

TEST(Structures, StructureAt)
{
    for (const auto &[structure, expected_type] : structures_list)
    {
        std::vector<std::vector<Player>> flanked_structures = generate_flanked_structures(structure, expected_type);
        for (std::vector<Player> flanked_structure : flanked_structures)
        {
            GomokuGame game(flanked_structure.size(), 1);

            for (int i = 0; i < flanked_structure.size(); i++)
                game.set_board_value(0, i, flanked_structure[i]);

            GomokuPatternReconizer reconizer(O);

            reconizer.find_patterns_in_board(game);

            GomokuCellIndex idx(0, 2);

            StructureType type = reconizer.get_structure_at(idx, PatternDirection::LeftToRight).first;

            EXPECT_EQ(type, expected_type);
        }
    }
}

TEST(Structures, WalledStructs)
{
    for (const auto &[structure, expected_type] : structures_list)
    {
        GomokuGame game(structure.size(), 1);

        for (int i = 0; i < structure.size(); i++)
            game.set_board_value(0, i, structure[i]);

        GomokuPatternReconizer reconizer(O);

        reconizer.find_patterns_in_board(game);

        GomokuCellIndex idx(0, 1);

        StructureType type = reconizer.get_structure_at(idx, PatternDirection::LeftToRight).first;

        EXPECT_EQ(type, expected_type);
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
