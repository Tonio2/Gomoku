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
    fetchingSuggestions,
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
            {fetchingSuggestions ? (
              <svg
                aria-hidden="true"
                className="w-4 h-4 text-gray-200 animate-spin dark:text-gray-600 fill-blue-600"
                viewBox="0 0 100 101"
                fill="none"
                xmlns="http://www.w3.org/2000/svg"
              >
                <path
                  d="M100 50.5908C100 78.2051 77.6142 100.591 50 100.591C22.3858 100.591 0 78.2051 0 50.5908C0 22.9766 22.3858 0.59082 50 0.59082C77.6142 0.59082 100 22.9766 100 50.5908ZM9.08144 50.5908C9.08144 73.1895 27.4013 91.5094 50 91.5094C72.5987 91.5094 90.9186 73.1895 90.9186 50.5908C90.9186 27.9921 72.5987 9.67226 50 9.67226C27.4013 9.67226 9.08144 27.9921 9.08144 50.5908Z"
                  fill="currentColor"
                />
                <path
                  d="M93.9676 39.0409C96.393 38.4038 97.8624 35.9116 97.0079 33.5539C95.2932 28.8227 92.871 24.3692 89.8167 20.348C85.8452 15.1192 80.8826 10.7238 75.2124 7.41289C69.5422 4.10194 63.2754 1.94025 56.7698 1.05124C51.7666 0.367541 46.6976 0.446843 41.7345 1.27873C39.2613 1.69328 37.813 4.19778 38.4501 6.62326C39.0873 9.04874 41.5694 10.4717 44.0505 10.1071C47.8511 9.54855 51.7191 9.52689 55.5402 10.0491C60.8642 10.7766 65.9928 12.5457 70.6331 15.2552C75.2735 17.9648 79.3347 21.5619 82.5849 25.841C84.9175 28.9121 86.7997 32.2913 88.1811 35.8758C89.083 38.2158 91.5421 39.6781 93.9676 39.0409Z"
                  fill="currentFill"
                />
              </svg>
            ) : (
              "?"
            )}
          </button>
        </div>
        <p className="py-2">{status(isGameOver, winnerId, nextPlayerId)}</p>
      </div>
    </div>
  );
};

export default RoomPage;
