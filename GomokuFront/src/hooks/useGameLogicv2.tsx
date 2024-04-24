import { useCallback, useEffect, useMemo, useState } from "react";

import {
  getBestMove,
  uniqueUserID,
  emptyBoard,
  logMoveHistory,
  getWorstMove,
  emptySuggestionBoard,
} from "../utils/utils";
import api from "../utils/api";

import { MoveHistory, Mode, Rule, Player, ActionResult } from "../interface";

type GameLogic = {
  board: number[][];
  nextPlayerId: number;
  nextAction: number;
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
    },
    {
      color: 0,
      score: 0,
      time: 0,
    },
  ]);
  const [nextAction, setNextAction] = useState<number>(0);
  const [nextPlayerId, setNextPlayerId] = useState<number>(0);
  const [suggestionBoard, setSuggestionsBoard] = useState<number[][][]>(
    emptySuggestionBoard(size),
  );

  const [isGameCreated, setIsGameCreated] = useState(false);

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
  }, [userId, mode, size, ruleStyle, starter]);

  const updateBoard = (res: ActionResult) => {
    setBoard(res._board);
    setIsGameOver(res._isGameOver);
    setWinnerId(res._winner);
    setListMoves(res._listMoves);
    setCurrentMove(res._currentMove);
    setPlayers(res._players);
    setNextPlayerId(res._nextPlayer);
  };

  const handleMoveResponse = useCallback(
    async (res: ActionResult) => {
      if (!res.success) {
        console.error(res.message);
        return;
      }
      console.log(res);
      updateBoard(res);
      if (!res._has_pending_action) {
        if (res._nextAction == 1) {
          let input = "";
          while (input.toLowerCase() != "y" && input.toLowerCase() != "n")
            input = prompt("swap? (Y/N)") || "";
          const swapActionResult = await api.swap(
            userId,
            input.toLowerCase() === "y",
          );
          handleMoveResponse(swapActionResult);
        }
        return;
      }
      try {
        const newRes = await api.aiTurn(userId);
        handleMoveResponse(newRes);
      } catch (error: any) {
        console.error("Server error");
      }
    },
    [userId],
  );

  const play = useCallback(
    async (row: number, col: number) => {
      try {
        const res = await api.makeMove(userId, row, col);
        handleMoveResponse(res);
      } catch (error: any) {
        console.error("Server error");
      }
    },
    [userId],
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

  const reset = () => {};

  return {
    board,
    nextPlayerId,
    nextAction,
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
