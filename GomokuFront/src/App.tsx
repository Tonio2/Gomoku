import React from "react";
import { BrowserRouter as Router, Route, Routes } from "react-router-dom";
import GameSetting from "./GameSettings";
import GomokuGameBoard from "./GomokuGameBoard";

const App: React.FC = () => {
  return (
    <Router>
      <Routes>
        <Route path="/" element={<GameSetting />} />
        <Route path="/game" element={<GomokuGameBoard />} />
      </Routes>
    </Router>
  );
};

export default App;
