import React, { useMemo } from "react";

import useGameLogic from "./hooks/useGameLogicv2";

import Board from "./components/Board";
import ListMoves from "./components/ListMoves";
import Button from "./components/Button";
import ScoreBoard from "./components/ScoreBoard";
import { useNavigate } from "react-router-dom";

const Game: React.FC = () => {
  const {
    board,
    listMoves,
    currentMove,
    xIsNext,
    winner,
    players,
    suggestionBoard,
    handleClick,
    handleReverse,
    handleReapply,
    handleReset,
  } = useGameLogic();
  const navigate = useNavigate();

  let status: string;
  if (winner) {
    status = (winner === 1 ? "Black" : "White") + " wins!";
  } else {
    status = "Next player: " + (xIsNext ? "X" : "O");
  }

  function handleMenu(): void {
    navigate("/");
  }

  const boardJSX = useMemo(() => {
    return (
      <Board
        xIsNext={xIsNext}
        board={board}
        handleClick={handleClick}
        suggestionBoard={suggestionBoard}
      />
    );
  }, [xIsNext, board, handleClick, suggestionBoard]);

  return (
    <div className="flex flex-row justify-items-center justify-center gap-10 h-screen px-10 items-start pt-20">
      <div className="flex flex-col">
        <div className="game-board flex-none">{boardJSX}</div>
        <div className="grid grid-cols-2 gap-4">
          <Button onClick={() => handleMenu()} text="Menu" />
          <Button onClick={() => handleReset()} text="Reset game" />
        </div>
      </div>
      <div className="game-info flex flex-col max-h-[80vh] w-[325px]">
        <h1 className="font-bold text-2xl mb-5">{status}</h1>
        <ScoreBoard players={players} />
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
};

export default Game;
