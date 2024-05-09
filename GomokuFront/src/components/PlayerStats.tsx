import React from "react";
import Stone from "./Stone";

import { Player, StoneType } from "../interface";

interface IPlayerStats {
  player: Player;
}

const PlayerStats: React.FC<IPlayerStats> = ({ player }) => {
  return (
    <table className="border-separate border-spacing-0 w-full bg-[rgba(10,23,38,0.8)] shadow-blur mt-2 table-fixed">
      <tbody>
        <tr>
          <td>Plays</td>
          <td>
            <div className="flex justify-center">
              <Stone
                value={player.color === 0 ? "X" : "O"}
                type={StoneType.Display}
                height={40}
              />
            </div>
          </td>
        </tr>
        <tr>
          <td>Score</td>
          <td>{player.score}</td>
        </tr>
        <tr>
          <td>Time</td>
          <td>{player.time.toFixed(1)}</td>
        </tr>
      </tbody>
    </table>
  );
};

export default PlayerStats;
