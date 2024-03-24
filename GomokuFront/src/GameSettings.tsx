import React, { useState } from "react";
import { useNavigate } from "react-router-dom";

const GameSetting: React.FC = () => {
  const [size, setSize] = useState<number>(19); // Default board size
  const [mode, setMode] = useState<number>(0); // Default mode: Human vs AI
  const [starter, setStarter] = useState<number>(0); // Default starter: Human
  const [ruleStyle, setRuleStyle] = useState<number>(0); // Default rule style: Standard
  const navigate = useNavigate();

  const startGame = () => {
    // Redirect to the game page and pass the settings
    navigate(
      `/game?mode=${mode}&size=${size}&starter=${starter}&ruleStyle=${ruleStyle}`
    );
  };

  return (
    <div>
      <h1>Game Setting</h1>
      <label>Board Size:</label>
      <input
        type="number"
        value={size}
        onChange={(e) => setSize(parseInt(e.target.value))}
        min="10"
        max="25"
      />
      <br />
      <br />
      <label>Mode:</label>
      <select value={mode} onChange={(e) => setMode(parseInt(e.target.value))}>
        <option value="0">Human vs AI</option>
        <option value="1">Human vs Human</option>
      </select>
      <br />
      <br />
      {mode === 0 && ( // Show this only if mode is Human vs AI
        <>
          <label>Who starts:</label>
          <select
            value={starter}
            onChange={(e) => setStarter(parseInt(e.target.value))}
          >
            <option value="0">Human</option>
            <option value="1">AI</option>
          </select>
        </>
      )}
      <br />
      <br />
      <label>Rule Style:</label>
      <select
        value={ruleStyle}
        onChange={(e) => setRuleStyle(parseInt(e.target.value))}
      >
        <option value="standard">Standard</option>
        <option value="pro">Pro</option>
        <option value="swap">Swap</option>
        <option value="swap2">Swap2</option>
      </select>
      <button onClick={startGame}>Play</button>
    </div>
  );
};

export default GameSetting;
