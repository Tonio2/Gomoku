#include <pybind11/pybind11.h>
#include <pybind11/stl.h> 
#include "gomoku_engine.h"
#include "AI1.h"

namespace py = pybind11;

PYBIND11_MODULE(pygomoku, m)
{
  py::class_<GomokuGame>(m, "GomokuGame")
      .def(py::init<uint>())
      .def("is_game_over", &GomokuGame::is_game_over)
      .def("make_move", &GomokuGame::make_move)
      .def("check_win", &GomokuGame::check_win)
      .def("count_open_threes", &GomokuGame::count_open_threes)
      .def("get_winner", &GomokuGame::get_winner)
      .def("get_board_value", &GomokuGame::get_board_value)
      .def("get_board_size", &GomokuGame::get_board_size)
      .def("get_current_player", &GomokuGame::get_current_player)
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
  py::class_<MoveEvaluation>(m, "MoveEvaluation")
      .def(py::init<>())
      .def_readwrite("move", &MoveEvaluation::move)
      .def_readwrite("score", &MoveEvaluation::score)
      .def_readwrite("listMoves", &MoveEvaluation::listMoves);
  py::class_<GomokuAI>(m, "GomokuAI")
      .def(py::init<GomokuGame &, Player, int>())
      .def("suggest_move", &GomokuAI::suggest_move);
}
