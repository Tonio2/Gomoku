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
      .def("get_current_player", &GomokuGame::get_current_player);
}