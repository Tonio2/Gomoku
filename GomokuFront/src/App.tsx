import React from "react";
import { BrowserRouter as Router, Route, Routes } from "react-router-dom";
import GameSetting from "./GameSettings";
import GomokuGameBoard from "./GomokuGameBoard";
import GomokuOnlineRoom from "./GomokuOnlineRoom";

const App: React.FC = () => {
  return (
    <Router>
      <Routes>
        <Route path="/" element={<GameSetting />} />
        <Route path="/game" element={<GomokuGameBoard />} />
        <Route path="/online_game" element={<GomokuOnlineRoom />} />
      </Routes>
    </Router>
  );
};

export default App;
