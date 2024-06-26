import { useCallback, useEffect, useMemo, useState } from "react";

import { uniqueUserID, emptyBoard, emptySuggestionBoard } from "../utils/utils";
import api from "../utils/api";
import socket from "../socket";

import { Player, ActionResult, OnlineActionResult } from "../interface";

type GameLogic = {
  board: number[][];
  nextPlayerId: number;
  listMoves: string[];
  currentMove: number;
  isGameOver: boolean;
  winnerId: number;
  players: Player[];
  suggestionBoard: number[][][]; // store [currentMove, value] for each cell
  onRoleSelected: (index: number) => void;
  availableRoles: boolean[];
  isRoleModalVisible: boolean;
  playerId: number;
  handleClick: (row: number, col: number) => void;
  handleReset: () => void;
};

const useGameLogic = (
  notify: (msg: string, type: string) => void
): GameLogic => {
  const roomId = useMemo(
    () => new URLSearchParams(window.location.search).get("roomId") || "",
    []
  );
  const [board, setBoard] = useState<number[][]>(emptyBoard(19));
  const [listMoves, setListMoves] = useState<string[]>([]);
  const [currentMove, setCurrentMove] = useState<number>(0);
  const [isGameOver, setIsGameOver] = useState<boolean>(false);
  const [winnerId, setWinnerId] = useState<number>(0);
  const [players, setPlayers] = useState<Player[]>([
    {
      color: 0,
      score: 0,
      time: 0,
      isAI: false,
    },
    {
      color: 0,
      score: 0,
      time: 0,
      isAI: false,
    },
  ]);
  const [nextPlayerId, setNextPlayerId] = useState<number>(0);
  const [hasPendingAction, setHasPendingAction] = useState<boolean>(false);
  const [nextAction, setNextAction] = useState<number>(0);
  const [isGameReady, setIsGameReady] = useState<boolean>(false);
  const [availableRoles, setAvailableRoles] = useState<boolean[]>([
    true,
    false,
    false,
  ]);
  const [playerId, setPlayerId] = useState<number>(0);
  const [isRoleModalVisible, setIsRoleModalVisible] = useState<boolean>(false);
  const [suggestionBoard, setSuggestionsBoard] = useState<number[][][]>(
    emptySuggestionBoard(19)
  );

  const updateBoard = useCallback((res: OnlineActionResult) => {
    setBoard(res._board);
    setIsGameOver(res._isGameOver);
    setWinnerId(res._winner);
    setListMoves(res._listMoves);
    setCurrentMove(res._currentMove);
    setPlayers(res._players);
    setNextPlayerId(res._nextPlayer);
    setHasPendingAction(res._hasPendingAction);
    setNextAction(res._nextAction);
    setIsGameReady(res._isGameReady);
  }, []);

  useEffect(() => {
    const onDisconnect = (data: { playerId: number }) => {
      notify("Player " + data.playerId.toString() + " disconnected...", "info");
    };

    const onConnect = (_playerId: number) => {
      if (_playerId === 0) notify("A spectator connected...", "info");
      else notify("Player " + _playerId.toString() + " connected...", "info");
    };

    socket.on("update", updateBoard);
    socket.on("disconnected", onDisconnect);
    socket.on("connected", onConnect);
    socket.emit(
      "whoami",
      { room_id: roomId },
      (
        success: boolean,
        message: string,
        _playerId: number,
        _availableRoles: boolean[]
      ) => {
        if (!success) notify(message, "error");
        else {
          setPlayerId(_playerId);
          setAvailableRoles(_availableRoles);
          if (
            (_availableRoles[1] === true || _availableRoles[2] === true) &&
            _playerId === 0
          )
            // If I haven't got a role and roles are available
            setIsRoleModalVisible(true);
        }
      }
    );

    return () => {
      socket.off("update", updateBoard);
      socket.off("disconnected", onDisconnect);
      socket.off("connected", onConnect);
      socket.emit("disconnect_request");
    };
  }, [roomId]);

  useEffect(() => {
    const handleSwapChoice = async () => {
      let input = "";
      while (input.toLowerCase() !== "y" && input.toLowerCase() !== "n")
        input = prompt("swap? (Y/N)") || "";
      socket.emit(
        "swap",
        { room_id: roomId, swap: input.toLowerCase() === "y" },
        (success: boolean, message: string) => {
          if (!success) notify(message, "error");
        }
      );
    };

    if (!hasPendingAction && nextAction === 1 && nextPlayerId == playerId - 1)
      handleSwapChoice();
  }, [hasPendingAction, nextAction, nextPlayerId, playerId]);

  const onRoleSelected = useCallback(
    (_playerId: number) => {
      socket.emit(
        "join",
        { room_id: roomId, player_id: _playerId },
        async (success: boolean, message: string) => {
          if (!success) {
            notify(message, "error");
          } else {
            setPlayerId(_playerId);
            setIsRoleModalVisible(false);
          }
        }
      );
    },
    [roomId]
  );

  const play = useCallback(
    (row: number, col: number) => {
      socket.emit(
        "make_move",
        { room_id: roomId, row: row, col: col },
        (success: boolean, message: string) => {
          if (!success) notify(message, "error");
        }
      );
    },
    [roomId]
  );

  const handleClick = async (row: number, col: number) => {
    play(row, col);
  };

  const reset = async () => {
    try {
      const res = await api.resetOnlineRoom(roomId);
    } catch (error: any) {
      console.error("Server error");
    }
  };

  return {
    board,
    nextPlayerId,
    listMoves,
    currentMove,
    winnerId,
    isGameOver,
    players,
    suggestionBoard,
    onRoleSelected,
    availableRoles,
    isRoleModalVisible,
    playerId,
    handleClick: handleClick,
    handleReset: reset,
  };
};

export default useGameLogic;
