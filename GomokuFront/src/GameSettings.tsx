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
      `/game?mode=${mode}&size=${size}&starter=${starter}&ruleStyle=${ruleStyle}`,
    );
  };

  return (
    <div className="min-h-screen flex flex-col items-center justify-center">
      <h1 className="text-4xl font-bold text-[#4affef] mb-8">Game Menu</h1>
      <div className="bg-gray-800 p-8 rounded-lg shadow-lg text-[#4affef]">
        <div className="mb-6">
          <label className="block mb-2">Board Size:</label>
          <input
            className="bg-gray-700 border-2 border-[#4affef] text-[#4affef] rounded w-full px-3 py-2 focus:outline-none"
            type="number"
            value={size}
            onChange={(e) => setSize(parseInt(e.target.value))}
            min="10"
            max="25"
          />
        </div>

        <div className="mb-6">
          <label className="block mb-2">Mode:</label>
          <select
            className="bg-gray-700 border-2 border-[#4affef] text-[#4affef] rounded w-full px-3 py-2 focus:outline-none"
            value={mode}
            onChange={(e) => setMode(parseInt(e.target.value))}
          >
            <option value="0">Human vs AI</option>
            <option value="1">Human vs Human</option>
          </select>
        </div>

        {mode === 0 && (
          <div className="mb-6">
            <label className="block mb-2">Who starts:</label>
            <select
              className="bg-gray-700 border-2 border-[#4affef] text-[#4affef] rounded w-full px-3 py-2 focus:outline-none"
              value={starter}
              onChange={(e) => setStarter(parseInt(e.target.value))}
            >
              <option value="0">Human</option>
              <option value="1">AI</option>
            </select>
          </div>
        )}

        <div className="mb-8">
          <label className="block mb-2">Rule Style:</label>
          <select
            className="bg-gray-700 border-2 border-[#4affef] text-[#4affef] rounded w-full px-3 py-2 focus:outline-none"
            value={ruleStyle}
            onChange={(e) => setRuleStyle(parseInt(e.target.value))}
          >
            <option value="0">Standard</option>
            <option value="1">Pro</option>
            <option value="3">Swap</option>
            <option value="4">Swap2</option>
          </select>
        </div>

        <button
          className="w-full bg-[rgba(10,23,38,0.8)] hover:bg-[rgba(10,23,38,0.5)] text-[#4affef] font-bold py-2 px-4 rounded focus:outline-none"
          onClick={startGame}
        >
          Play
        </button>
      </div>
    </div>
  );
};

export default GameSetting;
