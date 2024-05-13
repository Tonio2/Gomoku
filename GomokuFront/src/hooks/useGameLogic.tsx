import { useCallback, useEffect, useMemo, useState } from "react";

import { uniqueUserID, emptyBoard, emptySuggestionBoard } from "../utils/utils";
import api from "../utils/api";

import { Player, ActionResult, MoveEvaluation } from "../interface";

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
  getSuggestionBoard: () => void;
  fetchingSuggestions: boolean;
  handleReverse: () => void;
  handleReapply: () => void;
  handleReset: () => void;
};

const useGameLogic = (
  notify: (msg: string, type: string) => void
): GameLogic => {
  const size = useMemo(
    () => Number(new URLSearchParams(window.location.search).get("size")),
    []
  );
  const userId = useMemo(() => uniqueUserID(), []);

  const mode = useMemo(
    () => Number(new URLSearchParams(window.location.search).get("mode")),
    []
  );

  const starter = useMemo(
    () => Number(new URLSearchParams(window.location.search).get("starter")),
    []
  );

  const aiName = useMemo(
    () => new URLSearchParams(window.location.search).get("aiName") || "medium",
    []
  );

  const ruleStyle = useMemo(
    () => Number(new URLSearchParams(window.location.search).get("ruleStyle")),
    []
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
    emptySuggestionBoard(size)
  );
  const [fetchingSuggestions, setFetchingSuggestions] =
    useState<boolean>(false);

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
    setSuggestionsBoard(emptySuggestionBoard(size));
  }, []);

  const getBestEval = (
    moveEvaluation: MoveEvaluation,
    maximizing: boolean
  ): MoveEvaluation => {
    if (!moveEvaluation.listMoves || moveEvaluation.listMoves.length === 0) {
      return moveEvaluation;
    }
    let bestEval = moveEvaluation.listMoves[0];
    let bestScore = maximizing ? -Infinity : Infinity;
    for (let i = 1; i < moveEvaluation.listMoves.length; i++) {
      if (maximizing && moveEvaluation.listMoves[i].score > bestScore) {
        console.log(moveEvaluation.listMoves[i].score);
        bestEval = moveEvaluation.listMoves[i];
        bestScore = moveEvaluation.listMoves[i].score;
      }
      if (!maximizing && moveEvaluation.listMoves[i].score < bestScore) {
        bestEval = moveEvaluation.listMoves[i];
        bestScore = moveEvaluation.listMoves[i].score;
      }
    }
    return bestEval;
  };

  const handleMoveResponse = useCallback(
    async (res: ActionResult) => {
      if (!res.success) {
        notify(res.message, "error");
        return;
      }
      updateBoard(res);

      // Perform pending actions if any
      try {
        if (res._hasPendingAction) {
          const newRes = await api.performPendingAction(userId);
          handleMoveResponse(newRes);
        }
      } catch (error: any) {
        console.error("Server error");
      }
    },
    [userId, updateBoard]
  );

  useEffect(() => {
    const handleSwapChoice = async () => {
      let input = "";
      while (input.toLowerCase() !== "y" && input.toLowerCase() !== "n")
        input = prompt("swap? (Y/N)") || "";
      const swapActionResult = await api.swap(
        userId,
        input.toLowerCase() === "y"
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
          aiName
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
    [userId, handleMoveResponse]
  );

  const handleClick = async (row: number, col: number) => {
    play(row, col);
  };

  const getAISuggestion = async () => {
    try {
      setFetchingSuggestions(true);
      let { moveEvaluation } = await api.getSuggestion(userId);
      const newSuggestionBoard = emptySuggestionBoard(size);
      let maximizing = true;
      let index = 1;
      while (moveEvaluation.listMoves && moveEvaluation.listMoves.length > 0) {
        const bestEval = getBestEval(moveEvaluation, maximizing);
        const nextColor = players[nextPlayerId].color + 1;
        const otherColor = nextColor === 1 ? 2 : 1;
        newSuggestionBoard[bestEval.move[0]][bestEval.move[1]] = [
          currentMove + index,
          maximizing ? nextColor : otherColor,
        ];
        moveEvaluation = bestEval;
        maximizing = !maximizing;
        index++;
      }
      setSuggestionsBoard(newSuggestionBoard);
    } catch (error: any) {
      console.error("Server error");
    } finally {
      setFetchingSuggestions(false);
    }
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
    getSuggestionBoard: getAISuggestion,
    fetchingSuggestions,
    handleClick: handleClick,
    handleReverse: reverse,
    handleReapply: reapply,
    handleReset: reset,
  };
};

export default useGameLogic;
