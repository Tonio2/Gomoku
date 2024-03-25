import React from "react";

interface IScoreBoard {
  playerX: { score: number; time: number };
  playerO: { score: number; time: number };
}

const ScoreBoard: React.FC<IScoreBoard> = ({ playerX, playerO }) => {
  return (
    <div className="flex flex-col mb-5">
      <div className="grid grid-cols-3 gap-2">
        <h2 className="font-bold text-lg">Player X</h2>
        <h2 className="font-bold text-lg text-center">{playerX.score}</h2>
        <h2 className="font-bold text-lg text-right">
          {playerX.time.toFixed(1)}
        </h2>
      </div>
      <div className="grid grid-cols-3 gap-2">
        <h2 className="font-bold text-lg">Player O</h2>
        <h2 className="font-bold text-lg text-center">{playerO.score}</h2>
        <h2 className="font-bold text-lg text-right">
          {playerO.time.toFixed(1)}
        </h2>
      </div>
    </div>
  );
};

export default ScoreBoard;
