import React, { useEffect, useMemo, useState } from "react";
import { useNavigate } from "react-router-dom";

import useGameLogic from "../hooks/useOnlineGameLogic";

import Board from "../components/Board";
import RoleModal from "../components/RoleModal";
import ListMoves from "../components/ListMoves";

const OnlineRoomPage: React.FC<{
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
    availableRoles,
    onRoleSelected,
    isRoleModalVisible,
    playerId,
    handleClick,
    handleReset,
  } = useGameLogic(notify);
  const navigate = useNavigate();
  const [height, setHeight] = React.useState(40);
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
        nextPlayerColor={players[playerId - 1] && players[playerId - 1].color}
        board={board}
        handleClick={handleClick}
        suggestionBoard={suggestionBoard}
        height={height}
      />
    );
  }, [players, nextPlayerId, board, handleClick, suggestionBoard]);

  return (
    <div className="flex flex-col h-screen">
      <div className="bg-gray-800 text-white p-4">Menu</div>

      <div className="flex-grow p-8 overflow-x-auto" id="content">
        <div className="flex">
          <div className="flex-grow flex justify-center">
            <div className="game-board">{boardJSX}</div>
          </div>
          {isVisible && (
            <div className="flex justify-start">
              <ListMoves moves={listMoves} currentMove={currentMove} />
            </div>
          )}
        </div>
      </div>

      <div className="bg-gray-700 text-white p-4">Footer</div>
      <RoleModal
        onRoleSelected={onRoleSelected}
        roles={availableRoles}
        isVisible={isRoleModalVisible}
      />
    </div>
  );
};

export default OnlineRoomPage;
