import React from "react";
import PlayerStats from "./PlayerStats"
import { Player } from "../interface"

interface IScoreBoard {
  players: Player[];
}

const ScoreBoard: React.FC<IScoreBoard> = ({ players }) => {
  return (
    <div className="grid grid-cols-2 gap-2 mb-6 score-board">
        <div>
            <h2 className="font-bold text-lg text-center">Player 1</h2>
            <PlayerStats player={players[0]} />
        </div>
        <div>
            <h2 className="font-bold text-lg text-center">Player 2</h2>
            <PlayerStats player={players[1]} />
        </div>
    </div>
  )
};

export default ScoreBoard;
