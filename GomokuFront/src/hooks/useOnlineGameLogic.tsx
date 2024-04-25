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
  handleClick: (row: number, col: number) => void;
  handleReverse: () => void;
  handleReapply: () => void;
  handleReset: () => void;
};

const useGameLogic = (): GameLogic => {
  const roomId = useMemo(
    () => new URLSearchParams(window.location.search).get("roomId") || "",
    [],
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
  const [availableRoles, setAvailableRoles] = useState<boolean[]>([true, false, false])
  const [playerId, setPlayerId] = useState<number>(0)
  const [isFetching, setIsFetching] = useState<boolean>(true)
  const [suggestionBoard, setSuggestionsBoard] = useState<number[][][]>(
    emptySuggestionBoard(19),
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
    socket.on("update", updateBoard)
    socket.emit("whoami", {room_id: roomId}, (success: boolean, message: string, _playerId: number, _availableRoles: boolean[]) => {
      if (!success) console.error(message)
      else {
        setPlayerId(_playerId);
        setAvailableRoles(_availableRoles);
        setIsFetching(false);
      }
    });

    return () => {
      socket.off("update", updateBoard)
    }
  }, [roomId])

  useEffect(() => {
    const selectRole = async () => {
      let chosenRole = -1;
      while (chosenRole === -1 || availableRoles[chosenRole] !== true) {
        let text = "Choose a role:\n"
        const roles = ["0. Spectator", "1. Player 1", "2. Player 2"]
        for (let i = 0; i < 3; i++) {
          if (availableRoles[i]) text = text + roles[i] + "\n";
        }
        chosenRole = parseInt(prompt(text) || "")
      }

      socket.emit("join", {room_id: roomId, player_id: chosenRole}, async (success: boolean, message: string) => {
        if (!success) {
          console.error(message)
        } else {
          setPlayerId(chosenRole)
        }
      });
    }

    if (!isFetching && playerId == 0) selectRole()

    
  },  [playerId, availableRoles, isFetching]);


  const play = useCallback((row: number, col: number) => {
    socket.emit("make_move", {room_id: roomId, row: row, col: col}, (success: boolean, message: string) => {
      if (!success) console.error(message);
    });
  }, [roomId]);

  const handleClick = async (row: number, col: number) => {
    play(row, col);
  };

  const reverse = () => {};

  const reapply = () => {};

  const reset = () => {};

  return {
    board,
    nextPlayerId,
    listMoves,
    currentMove,
    winnerId,
    isGameOver,
    players,
    suggestionBoard,
    handleClick: handleClick,
    handleReverse: reverse,
    handleReapply: reapply,
    handleReset: reset,
  };
};

export default useGameLogic;
