#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "engine/gomoku_pattern_reconizer.h"
#include "engine/gomoku_engine.h"
#include "ai/gomoku_ai.h"
#include "room/game_room.h"
#include "utils/gomoku_utilities.h"

namespace py = pybind11;

PYBIND11_MODULE(pygomoku, m)
{
    /** Engine */
    py::class_<GomokuGame>(m, "GomokuGame")
        .def(py::init<uint, uint>())
        .def("is_game_over", &GomokuGame::is_game_over)
        .def("make_move", &GomokuGame::make_move)
        .def("check_win", &GomokuGame::check_win)
        .def("get_winner", &GomokuGame::get_winner)
        .def("get_board", &GomokuGame::get_board)
        .def("get_board_value", &GomokuGame::get_board_value)
        .def("get_board_width", &GomokuGame::get_board_width)
        .def("get_board_height", &GomokuGame::get_board_height)
        .def("get_current_player", &GomokuGame::get_current_player)
        .def("get_player_score", &GomokuGame::get_player_score)
        .def("reverse_move", &GomokuGame::reverse_move)
        .def("reapply_move", &GomokuGame::reapply_move);
    py::class_<CellChange>(m, "CellChange")
        .def(py::init<>())
        .def_readwrite("row", &CellChange::row)
        .def_readwrite("col", &CellChange::col)
        .def_readwrite("old_value", &CellChange::old_value)
        .def_readwrite("new_value", &CellChange::new_value);
    py::class_<MoveResult>(m, "MoveResult")
        .def(py::init<>())
        .def_readwrite("cell_changes", &MoveResult::cell_changes)
        .def_readwrite("white_score_change", &MoveResult::white_score_change)
        .def_readwrite("black_score_change", &MoveResult::black_score_change);
    py::enum_<Player>(m, "Player")
        .value("EMPTY", Player::EMPTY)
        .value("BLACK", Player::BLACK)
        .value("WHITE", Player::WHITE)
        .export_values();

    /** Utils */
    m.def("char_to_coordinate", &char_to_coordinate);
    m.def("coordinate_to_char", &coordinate_to_char);

    /** Ai */
    py::class_<MoveEvaluation>(m, "MoveEvaluation")
        .def(py::init<>())
        .def_readwrite("move", &MoveEvaluation::move)
        .def_readwrite("score", &MoveEvaluation::score)
        .def_readwrite("listMoves", &MoveEvaluation::listMoves);
    py::class_<GomokuAI>(m, "GomokuAI")
        .def(py::init<int>())
        .def("suggest_move", &GomokuAI::suggest_move);

    /** Room */
    py::enum_<GameActionType>(m, "GameActionType")
        .value("MOVE", GameActionType::MOVE)
        .value("SWAP", GameActionType::SWAP)
        .export_values();
    py::enum_<GameRoomRuleStyle>(m, "GameRoomRuleStyle")
        .value("STANDARD", GameRoomRuleStyle::STANDARD)
        .value("PRO", GameRoomRuleStyle::PRO)
        .value("LONG_PRO", GameRoomRuleStyle::LONG_PRO)
        .value("SWAP", GameRoomRuleStyle::SWAP)
        .value("SWAP2", GameRoomRuleStyle::SWAP2)
        .export_values();
    py::class_<GameActionValue_Move>(m, "GameActionValue_Move")
        .def_readwrite("row", &GameActionValue_Move::row)
        .def_readwrite("col", &GameActionValue_Move::col)
        .def_readwrite("result", &GameActionValue_Move::result);
    py::class_<GameActionValue_Swap>(m, "GameActionValue_Swap")
        .def_readwrite("did_swap", &GameActionValue_Swap::did_swap);
    py::class_<GameActionValue>(m, "GameActionValue")
        .def(py::init<>())
        .def_readwrite("move", &GameActionValue::move)
        .def_readwrite("swap", &GameActionValue::swap);
    py::class_<GameAction>(m, "GameAction")
        .def_readwrite("player", &GameAction::player)
        .def_readwrite("action_type", &GameAction::action_type)
        .def_readwrite("action_value", &GameAction::action_value);
    py::class_<GameActionResult>(m, "GameActionResult")
        .def_readwrite("success", &GameActionResult::success)
        .def_readwrite("message", &GameActionResult::message);
    py::class_<GameEntitySetting>(m, "GameEntitySetting")
        .def(py::init<>())
        .def_readwrite("is_ai", &GameEntitySetting::is_ai)
        .def_readwrite("ai_depth", &GameEntitySetting::ai_depth);
    py::class_<GameRoomSettings>(m, "GameRoomSettings")
        .def(py::init<>())
        .def_readwrite("width", &GameRoomSettings::width)
        .def_readwrite("height", &GameRoomSettings::height)
        .def_readwrite("rule_style", &GameRoomSettings::rule_style)
        .def_readwrite("p1", &GameRoomSettings::p1)
        .def_readwrite("p2", &GameRoomSettings::p2);
    py::class_<GameRoom>(m, "GameRoom")
        .def(py::init<const GameRoomSettings &>())
        .def("perform_action_move", &GameRoom::perform_action_move)
        .def("perform_action_swap", &GameRoom::perform_action_swap)
        .def("has_pending_action", &GameRoom::has_pending_action)
        .def("perform_pending_action", &GameRoom::perform_pending_action)
        .def("expected_player", &GameRoom::expected_player)
        .def("expected_action", &GameRoom::expected_action)
        .def("get_actions_history", &GameRoom::get_actions_history)
        .def("get_action_index", &GameRoom::get_action_index)
        .def("get_settings", &GameRoom::get_settings)
        .def("get_game", &GameRoom::get_game)
        .def("get_color_score", &GameRoom::get_color_score)
        .def("gomoku_player_from_id", &GameRoom::gomoku_player_from_id)
        .def("id_from_gomoku_player", &GameRoom::id_from_gomoku_player)
        .def("can_reverse_last_action", &GameRoom::can_reverse_last_action)
        .def("reverse_last_action", &GameRoom::reverse_last_action)
        .def("can_reapply_last_action", &GameRoom::can_reapply_last_action)
        .def("reapply_last_action", &GameRoom::reapply_last_action);
}
