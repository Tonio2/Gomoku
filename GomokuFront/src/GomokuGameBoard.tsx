import React from "react";

import useGameLogic from "./hooks/useGameLogic";

import Board from "./components/Board";
import ListMoves from "./components/ListMoves";
import Button from "./components/Button";
import ScoreBoard from "./components/ScoreBoard";

export default function Game() {
  const {
    board,
    listMoves,
    currentMove,
    xIsNext,
    winner,
    playerX,
    playerO,
    handleClick,
    handleReverse,
    handleReapply,
    handleReset,
  } = useGameLogic();

  let status: string;
  if (winner) {
    status = (winner === 1 ? "Black" : "White") + " wins!";
  } else {
    status = "Next player: " + (xIsNext ? "X" : "O");
  }

  return (
    <div className="flex flex-row justify-evenly p-10">
      <div className="flex flex-col">
        <h1 className="font-bold text-2xl mb-5">{status}</h1>
        <div className="game-board">
          <Board xIsNext={xIsNext} board={board} handleClick={handleClick} />
        </div>
        <Button onClick={() => handleReset()} text="Reset" />
      </div>
      <div className="game-info min-w-[287px]">
        <ScoreBoard playerX={playerX} playerO={playerO} />
        <ListMoves moves={listMoves} currentMove={currentMove} />
        <div className="grid grid-cols-2 gap-4">
          <Button
            onClick={() => handleReverse()}
            text="Reverse"
            disabled={currentMove === 0}
          />
          <Button
            onClick={() => handleReapply()}
            text="Reapply"
            disabled={currentMove === 0}
          />
        </div>
      </div>
    </div>
  );
}
