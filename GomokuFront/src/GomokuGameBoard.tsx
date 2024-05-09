import React, { useEffect, useMemo, useState } from "react";
import { useNavigate } from "react-router-dom";

import { status } from "./utils/utils";

import useGameLogic from "./hooks/useGameLogic";

import Board from "./components/Board";
import ListMoves from "./components/ListMoves";
import Button from "./components/Button";
import ScoreBoard from "./components/ScoreBoard";

const Game: React.FC<{ notify: (msg: string, type: string) => void }> = ({
  notify,
}) => {
  const {
    board,
    nextPlayerId,
    listMoves,
    currentMove,
    winnerId,
    isGameOver,
    players,
    suggestionBoard,
    handleClick,
    handleReverse,
    handleReapply,
    handleReset,
  } = useGameLogic(notify);
  const [height, setHeight] = useState<number>(840);
  const navigate = useNavigate();

  useEffect(() => {
    // Get height of div with id content
    const content = document.getElementById("content");
    if (content) {
      const contentHeight = content.clientHeight;
      setHeight(contentHeight);
    }
  }, []);

  function handleMenu(): void {
    navigate("/");
  }

  const boardJSX = useMemo(() => {
    return (
      <Board
        nextPlayerColor={players[nextPlayerId].color}
        board={board}
        handleClick={handleClick}
        suggestionBoard={suggestionBoard}
        height={height}
      />
    );
  }, [players, nextPlayerId, board, handleClick, suggestionBoard]);

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
        <h1 className="font-bold text-2xl mb-5">
          {status(isGameOver, winnerId, nextPlayerId)}
        </h1>
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
            disabled={currentMove === listMoves.length}
          />
        </div>
      </div>
    </div>
  );
};

export default Game;
