import { useCallback, useEffect, useMemo, useState } from "react";

import { getBestMove, uniqueUserID, emptyBoard } from "../utils/utils";
import api from "../utils/api";

import { MoveHistory } from "../interface";

type GameLogic = {
  board: number[][];
  listMoves: MoveHistory[];
  currentMove: number;
  xIsNext: boolean;
  winner: string | null;
  handleClick: (row: number, col: number) => void;
  handleReverse: () => void;
  handleReapply: () => void;
};

const useGameLogic = (): GameLogic => {
  const [board, setBoard] = useState<number[][]>(emptyBoard());
  const [listMoves, setListMoves] = useState<MoveHistory[]>([]);
  const [currentMove, setCurrentMove] = useState<number>(0);
  const xIsNext = currentMove % 2 === 0;
  const [winner, setWinner] = useState<string | null>(null);

  const userId = useMemo(() => uniqueUserID(), []);

  const mode = useMemo(
    () => Number(new URLSearchParams(window.location.search).get("mode")),
    []
  );

  useEffect(() => {
    const createGame = async () => {
      try {
        await api.createGame(userId);
      } catch (error) {
        console.error("Error creating the game:", error);
      }
    };

    createGame();
  }, [userId]);

  const play = useCallback(
    async (row: number, col: number) => {
      try {
        const { success, message, moveResult, newBoard, newWinner } =
          await api.makeMove(userId, row, col);
        if (success) {
          setBoard(newBoard);
          setListMoves((prevListMoves) => [
            ...prevListMoves.slice(0, currentMove),
            { row, col, moveResult: moveResult },
          ]);
          setWinner(newWinner);
          setCurrentMove((prevMove) => prevMove + 1);
        } else {
          console.error(message);
        }
      } catch (error: any) {
        console.error("Error making a move:", error.response.data);
      }
    },
    [currentMove, userId]
  );

  useEffect(() => {
    const get_AI_suggestion = async () => {
      try {
        const { moveEvaluation } = await api.getSuggestion(userId);
        const bestMove = getBestMove(moveEvaluation);
        play(bestMove[0], bestMove[1]);
      } catch (error) {
        console.error("Error getting AI suggestion:", error);
      }
    };

    if (mode === 0 && !xIsNext && currentMove === listMoves.length) {
      get_AI_suggestion();
    }
  }, [currentMove, mode, xIsNext, play, listMoves.length, userId]);

  const handleClick = (row: number, col: number) => {
    if (mode === 1 || (mode === 0 && xIsNext)) {
      play(row, col);
    }
  };

  const reverse = async () => {
    try {
      const { success, newBoard } = await api.reverseMove(
        userId,
        listMoves[currentMove - 1].moveResult
      );
      if (success) {
        setBoard(newBoard);
        setCurrentMove((prevMove) => prevMove - 1);
      }
    } catch (error: any) {
      console.error("Error reversing the move:", error.response.data);
    }
  };

  const reapply = async () => {
    try {
      const { success, newBoard } = await api.reapplyMove(
        userId,
        listMoves[currentMove].moveResult
      );
      if (success) {
        setBoard(newBoard);
        setCurrentMove((prevMove) => prevMove + 1);
      }
    } catch (error) {
      console.error("Error reapplying the move:", error);
    }
  };

  return {
    board,
    listMoves,
    currentMove,
    xIsNext,
    winner,
    handleClick: handleClick,
    handleReverse: reverse,
    handleReapply: reapply,
  };
};

export default useGameLogic;
