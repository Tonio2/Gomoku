import React, { useState } from "react";
import { useNavigate } from "react-router-dom";

const GameSetting: React.FC = () => {
  const [size, setSize] = useState<number>(3); // Default board size
  const navigate = useNavigate();

  const startGame = () => {
    // Redirect to the game page and pass the board size
    navigate(`/game?size=${size}`);
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
      <button onClick={startGame}>Play</button>
    </div>
  );
};

export default GameSetting;
