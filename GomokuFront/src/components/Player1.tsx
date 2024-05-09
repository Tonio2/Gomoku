import React from "react";

import { Player, StoneType } from "../interface";
import Stone from "./Stone";

const Player1: React.FC<{ player: Player; highlighted: boolean }> = ({
  player,
  highlighted,
}) => {
  const { color, score, time } = player;
  return (
    <div
      className={
        "flex flex-row items-center gap-3 px-2 md:px-4 py-1" +
        (highlighted ? " bg-gray-700" : "")
      }
    >
      <div className="flex flex-col items-center md:gap-1">
        <h2>Player 1</h2>
        <div className="text-white">{time.toFixed(2)}</div>
      </div>
      <div className="flex flex-row gap-2">
        <Stone
          value={color === 0 ? "X" : "O"}
          type={StoneType.Display}
          height={40}
        />
        <p className="text-white align-center leading-[30px]">{score}</p>
      </div>
    </div>
  );
};

export default Player1;
