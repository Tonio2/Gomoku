import React, { useMemo } from "react";
import { useNavigate } from "react-router-dom";

import { status } from "./utils/utils";

import useGameLogic from "./hooks/useOnlineGameLogic";

import Board from "./components/Board";
import ListMoves from "./components/ListMoves";
import Button from "./components/Button";
import ScoreBoard from "./components/ScoreBoard";
import RoleModal from "./components/RoleModal";

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
    availableRoles,
    onRoleSelected,
    isRoleModalVisible,
    playerId,
    handleClick,
    handleReset,
  } = useGameLogic(notify);
  const navigate = useNavigate();

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
        height={40}
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
      </div>
      <RoleModal
        onRoleSelected={onRoleSelected}
        roles={availableRoles}
        isVisible={isRoleModalVisible}
      />
    </div>
  );
};

export default Game;
