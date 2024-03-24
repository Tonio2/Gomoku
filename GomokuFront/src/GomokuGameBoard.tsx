import React from "react";

import useGameLogic from "./hooks/useGameLogic";

import Board from "./components/Board";
import ListMoves from "./components/ListMoves";

export default function Game() {
  const {
    board,
    listMoves,
    currentMove,
    xIsNext,
    winner,
    handleClick,
    handleReverse,
    handleReapply,
    handleReset,
  } = useGameLogic();

  return (
    <div className="game">
      <div className="game-board">
        <Board
          xIsNext={xIsNext}
          board={board}
          winner={winner}
          handleClick={handleClick}
        />
        <button onClick={() => handleReset()}>Reset</button>
      </div>
      <div className="game-info">
        <ListMoves moves={listMoves} currentMove={currentMove} />
        <button onClick={() => handleReverse()} disabled={currentMove === 0}>
          Reverse
        </button>
        <button
          onClick={() => handleReapply()}
          disabled={currentMove === listMoves.length}
        >
          Reapply
        </button>
      </div>
    </div>
  );
}
