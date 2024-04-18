import React from "react";

interface IScoreBoard {
  players: {
    color: number;
    isAI: boolean;
    score: number;
    time: number;
  }[];
}

const ScoreBoard: React.FC<IScoreBoard> = ({ players }) => {
  const playerX = players[0];
  const playerO = players[1];
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
