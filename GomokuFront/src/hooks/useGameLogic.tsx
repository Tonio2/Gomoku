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
  handleReset: () => void;
};

const useGameLogic = (): GameLogic => {
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
  const [board, setBoard] = useState<number[][]>(emptyBoard(size));
  const [listMoves, setListMoves] = useState<MoveHistory[]>([]);
  const [currentMove, setCurrentMove] = useState<number>(0);
  const xIsNext = currentMove % 2 === 0;
  const [winner, setWinner] = useState<string | null>(null);

  const [isGameCreated, setIsGameCreated] = useState(false);

  useEffect(() => {
    const createGame = async () => {
      try {
        const { success, message } = await api.createGame(userId, mode, size);
        if (success) {
          console.log("Game created");
          setIsGameCreated(true);
        } else {
          console.error(message);
        }
      } catch (error) {
        console.error("Server error");
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
        console.error("Server error");
      }
    },
    [currentMove, userId]
  );

  useEffect(() => {
    const get_AI_suggestion = async () => {
      try {
        const { success, message, moveEvaluation } = await api.getSuggestion(
          userId
        );
        if (success) {
          const bestMove = getBestMove(moveEvaluation);
          play(bestMove[0], bestMove[1]);
        } else {
          console.error(message);
        }
      } catch (error) {
        console.error("Server error");
      }
    };

    if (
      mode === 0 &&
      Number(xIsNext) === starter &&
      currentMove === listMoves.length &&
      isGameCreated
    ) {
      if (currentMove === 0) {
        play(Math.floor(size / 2), Math.floor(size / 2));
      } else {
        get_AI_suggestion();
      }
    }
  }, [
    currentMove,
    mode,
    xIsNext,
    play,
    listMoves.length,
    userId,
    isGameCreated,
  ]);

  const handleClick = (row: number, col: number) => {
    if (mode === 1 || (mode === 0 && Number(xIsNext) !== starter)) {
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
      console.error("Server error");
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
      console.error("Server error");
    }
  };

  const reset = async () => {
    try {
      const { success, message } = await api.resetGame(userId);
      if (success) {
        setBoard(emptyBoard(size));
        setListMoves([]);
        setCurrentMove(0);
        setWinner(null);
      } else {
        console.error(message);
      }
    } catch (error) {
      console.error("Server error");
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
    handleReset: reset,
  };
};

export default useGameLogic;
