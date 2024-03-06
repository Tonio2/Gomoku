#include <pybind11/pybind11.h>
#include "gomoku_engine.h"

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
}
