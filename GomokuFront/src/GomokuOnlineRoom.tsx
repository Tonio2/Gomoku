import React, { useMemo } from "react";

import useGameLogic from "./hooks/useOnlineGameLogic";

import Board from "./components/Board";
import ListMoves from "./components/ListMoves";
import Button from "./components/Button";
import ScoreBoard from "./components/ScoreBoard";
import RoleModal from "./components/RoleModal";
import { useNavigate } from "react-router-dom";

const status = (
  isGameOver: boolean,
  winnerId: number,
  nextPlayerId: number,
) => {
  let status;
  let next = (nextPlayerId + 1).toString();
  if (isGameOver) {
    if (winnerId === -1) {
      status = "Draw!";
    } else {
      status = "Player " + next + " wins!";
    }
  } else {
    status = "Waiting for Player " + next + "...";
  }
  return status;
};

const Game: React.FC = () => {
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
    handleReverse,
    handleReapply,
    handleReset,
  } = useGameLogic();
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
      <RoleModal
        onRoleSelected={onRoleSelected}
        roles={availableRoles}
        isVisible={isRoleModalVisible}
      />
    </div>
  );
};

export default Game;
