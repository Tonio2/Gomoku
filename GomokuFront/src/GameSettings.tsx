import React, { useState } from "react";
import { useNavigate } from "react-router-dom";

const GameSetting: React.FC = () => {
  const [size, setSize] = useState<number>(3); // Default board size
  const [mode, setMode] = useState<number>(0); // Default mode [Human vs Human
  const navigate = useNavigate();

  const startGame = () => {
    // Redirect to the game page and pass the board size
    navigate(`/game?mode=${mode}`);
  };

  return (
    <div>
      <h1>Game Setting</h1>
      <input
        type="number"
        value={size}
        onChange={(e) => setSize(parseInt(e.target.value))}
        min="3"
        max="10"
      />
      <select value={mode} onChange={(e) => setMode(parseInt(e.target.value))}>
        <option value="0">Human vs AI</option>
        <option value="1">Human vs Human</option>
      </select>
      <button onClick={startGame}>Play</button>
    </div>
  );
};

export default GameSetting;
