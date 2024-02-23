#include <pybind11/pybind11.h>
#include "gomoku_engine.h"

namespace py = pybind11;

PYBIND11_MODULE(pygomoku, m)
{
  py::class_<GomokuGame>(m, "GomokuGame")
      .def(py::init<>())
      .def("is_game_over", &GomokuGame::is_game_over)
      .def("make_move", &GomokuGame::make_move)
      .def("check_win", &GomokuGame::check_win)
      .def("get_board", &GomokuGame::get_board);
}
