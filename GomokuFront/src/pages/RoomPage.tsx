import React, { useEffect, useMemo, useState } from "react";
import { useNavigate } from "react-router-dom";

import useGameLogic from "../hooks/useGameLogic";

import Board from "../components/Board";
import ListMoves from "../components/ListMoves";
import Player1 from "../components/Player1";
import Player2 from "../components/Player2";
import { status } from "../utils/utils";
import Button from "../components/Button";

const RoomPage: React.FC<{
  notify: (msg: string, type: string) => void;
}> = ({ notify }) => {
  const {
    board,
    nextPlayerId,
    listMoves,
    currentMove,
    winnerId,
    isGameOver,
    players,
    suggestionBoard,
    getSuggestionBoard,
    handleClick,
    handleReverse,
    handleReapply,
    handleReset,
  } = useGameLogic(notify);
  const navigate = useNavigate();
  const [height, setHeight] = React.useState(820);
  const [isVisible, setIsVisible] = useState<boolean>(false);

  useEffect(() => {
    // Get height of div with id content
    const content = document.getElementById("content");
    if (content) {
      const contentHeight = content.clientHeight;
      setHeight(contentHeight);
      const contentWidth = content.clientWidth;
      if (contentWidth > contentHeight + 450) {
        setIsVisible(true);
      }
    }
  }, []);

  useEffect(() => {
    const scrollContainer = document.getElementById("content");
    if (scrollContainer) {
      scrollContainer.scrollLeft =
        (scrollContainer.scrollWidth - scrollContainer.clientWidth) / 2;
    }
  }, [height]);

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
    <div className="flex flex-col h-screen">
      <div className="bg-gray-800 text-white p-4 flex justify-center md:gap-8 gap-2">
        <Player1 player={players[0]} highlighted={nextPlayerId == 0} />
        <Player2 player={players[1]} highlighted={nextPlayerId == 1} />
      </div>
      <div className="flex-grow p-4 overflow-x-auto" id="content">
        <div className="flex">
          <div className="flex-grow flex justify-center">
            <div className="game-board me-4 md:me-0">{boardJSX}</div>
          </div>
          {isVisible && (
            <div
              className="flex flex-col justify-start"
              style={{ height: height - 32 }}
            >
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
          )}
        </div>
      </div>
      <div className="bg-gray-700 text-white p-4 flex justify-between">
        <div className="flex justify-start">
          <button
            className="bg-gray-800 text-white p-2 rounded-md"
            onClick={handleMenu}
          >
            Menu
          </button>
          <button
            className="bg-gray-800 text-white p-2 rounded-md ms-2"
            onClick={handleReset}
          >
            Reset
          </button>
          <button
            className="bg-gray-800 text-white p-2 rounded-md ms-2"
            onClick={getSuggestionBoard}
          >
            ?
          </button>
        </div>
        <p className="py-2">{status(isGameOver, winnerId, nextPlayerId)}</p>
      </div>
    </div>
  );
};

export default RoomPage;
