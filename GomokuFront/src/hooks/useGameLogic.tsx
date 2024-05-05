import { useCallback, useEffect, useMemo, useState } from "react";

import { uniqueUserID, emptyBoard, emptySuggestionBoard } from "../utils/utils";
import api from "../utils/api";

import { Player, ActionResult } from "../interface";

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

const useGameLogic = (notify: (msg: string, type: string) => void): GameLogic => {
  const size = useMemo(
    () => Number(new URLSearchParams(window.location.search).get("size")),
    [],
  );
  const userId = useMemo(() => uniqueUserID(), []);

  const mode = useMemo(
    () => Number(new URLSearchParams(window.location.search).get("mode")),
    [],
  );

  const starter = useMemo(
    () => Number(new URLSearchParams(window.location.search).get("starter")),
    [],
  );

  const ruleStyle = useMemo(
    () => Number(new URLSearchParams(window.location.search).get("ruleStyle")),
    [],
  );
  const [board, setBoard] = useState<number[][]>(emptyBoard(size));
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
  const [suggestionBoard, setSuggestionsBoard] = useState<number[][][]>(
    emptySuggestionBoard(size),
  );

  const updateBoard = useCallback((res: ActionResult) => {
    setBoard(res._board);
    setIsGameOver(res._isGameOver);
    setWinnerId(res._winner);
    setListMoves(res._listMoves);
    setCurrentMove(res._currentMove);
    setPlayers(res._players);
    setNextPlayerId(res._nextPlayer);
    setHasPendingAction(res._hasPendingAction);
    setNextAction(res._nextAction);
  }, []);

  const handleMoveResponse = useCallback(
    async (res: ActionResult) => {
      if (!res.success) {
        notify(res.message, "error");
        return;
      }
      updateBoard(res);
      try {
        if (res._players[res._nextPlayer].isAI) {
          const newRes = await api.aiTurn(userId);
          handleMoveResponse(newRes);
        }
      } catch (error: any) {
        console.error("Server error");
      }
    },
    [userId, updateBoard],
  );

  useEffect(() => {
    const handleSwapChoice = async () => {
      let input = "";
      while (input.toLowerCase() !== "y" && input.toLowerCase() !== "n")
        input = prompt("swap? (Y/N)") || "";
      const swapActionResult = await api.swap(
        userId,
        input.toLowerCase() === "y",
      );
      await handleMoveResponse(swapActionResult);
    };

    if (!hasPendingAction && nextAction === 1) handleSwapChoice();
  }, [userId, hasPendingAction, nextAction, handleMoveResponse]);

  useEffect(() => {
    const createGame = async () => {
      try {
        const res = await api.createGame(
          userId,
          mode,
          size,
          ruleStyle,
          starter,
        );
        handleMoveResponse(res);
      } catch (error) {
        console.error("Server error");
      }
    };

    createGame();
  }, [userId, mode, size, ruleStyle, starter, handleMoveResponse]);

  const play = useCallback(
    async (row: number, col: number) => {
      try {
        const res = await api.makeMove(userId, row, col);
        handleMoveResponse(res);
      } catch (error: any) {
        console.error("Server error");
      }
    },
    [userId, handleMoveResponse],
  );

  const handleClick = async (row: number, col: number) => {
    play(row, col);
  };

  const reverse = async () => {
    try {
      const res = await api.reverseMove(userId);
      updateBoard(res);
    } catch (error: any) {
      console.error("Error while reversing move");
    }
  };

  const reapply = async () => {
    try {
      const res = await api.reapplyMove(userId);
      updateBoard(res);
    } catch (error: any) {
      console.error("Error while reapplying  move");
    }
  };

  const reset = async () => {
    try {
      const res = await api.resetGame(userId);
      handleMoveResponse(res);
    } catch (error: any) {
      console.error("Error while reseting game");
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
    handleClick: handleClick,
    handleReverse: reverse,
    handleReapply: reapply,
    handleReset: reset,
  };
};

export default useGameLogic;
