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

  const ruleStyle = useMemo(
    () => Number(new URLSearchParams(window.location.search).get("ruleStyle")),
    []
  );
  const [board, setBoard] = useState<number[][]>(emptyBoard(size));
  const [listMoves, setListMoves] = useState<MoveHistory[]>([]);
  const [currentMove, setCurrentMove] = useState<number>(0);
  const xIsNext = currentMove % 2 === 0;
  const [winner, setWinner] = useState<string | null>(null);

  const [isGameCreated, setIsGameCreated] = useState(false);
  const [xIsAI, setXIsAI] = useState<boolean>(
    Boolean(new URLSearchParams(window.location.search).get("starter") === "1")
  );
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
  }, [userId, mode, size]);

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

  const isThreecellsAwayFrom = (candidate: number[], target: number[]) => {
    return (
      Math.abs(candidate[0] - target[0]) >= 3 ||
      Math.abs(candidate[1] - target[1]) >= 3
    );
  };

  useEffect(() => {
    const play_AI_suggestion = async (
      predicate: (move: number[]) => boolean = (move: number[]) => true
    ) => {
      try {
        const { success, message, moveEvaluation } = await api.getSuggestion(
          userId
        );
        if (success) {
          const bestMove = getBestMove(moveEvaluation, predicate);
          play(bestMove[0], bestMove[1]);
        } else {
          console.error(message);
        }
      } catch (error) {
        console.error("Server error");
      }
    };

    const swap_AI_suggestion = async () => {
      try {
        const { success, message, moveEvaluation } = await api.getSuggestion(
          userId
        );
        if (success) {
          if (moveEvaluation.score < 0) {
            setXIsAI(true);
            console.log("AI takes X");
          } else {
            console.log("AI takes O");
          }
        } else {
          console.error(message);
          console.log("AI takes O");
        }
      } catch (error) {
        console.error("Server error");
        console.log("AI takes O");
      }
    };

    const playAI = async () => {
      if (ruleStyle === 0) {
        if (xIsNext === xIsAI) {
          // Next Player is AI
          if (currentMove === 0) {
            // First move
            play(Math.floor(size / 2), Math.floor(size / 2)); // Play around the center
          } else {
            await play_AI_suggestion();
          }
        }
      } else if (ruleStyle === 1) {
        // Pro
        if (xIsNext === xIsAI) {
          // Next Player is AI
          if (currentMove === 0) {
            // First move
            play(Math.floor(size / 2), Math.floor(size / 2));
          } else {
            let predicate = (move: number[]) => true;
            if (currentMove === 2)
              // Check that the second move is at least 3 cells away from the first move
              predicate = (move: number[]) =>
                isThreecellsAwayFrom(move, [
                  listMoves[0].row,
                  listMoves[0].col,
                ]);
            await play_AI_suggestion(predicate);
          }
        }
      } else if (ruleStyle === 2) {
        // Swap
        if (currentMove === 3 && !xIsAI) {
          await swap_AI_suggestion();
        }
        if (
          (currentMove <= 2 && xIsAI) ||
          (xIsNext === xIsAI && currentMove >= 3)
        ) {
          console.log("AI's choosing the three first pieces");
          if (currentMove === 0) {
            play(Math.floor(size / 2), Math.floor(size / 2));
          } else {
            await play_AI_suggestion();
          }
        }
      }
    };

    const askUserForChoice = async () => {
      let success = false;
      if (ruleStyle === 2 && currentMove === 3 && xIsAI) {
        while (!success) {
          const choice = prompt("Do you want to take X or O?");
          if (choice === "X") {
            setXIsAI(false);
            console.log("You take X");
            success = true;
          } else if (choice === "O") {
            console.log("You take O");
            success = true;
          } else {
            console.error("You must enter either X or O");
          }
        }
      }
    };

    if (isGameCreated && mode === 0 && currentMove === listMoves.length) {
      playAI();
      askUserForChoice();
    }
  }, [
    currentMove,
    mode,
    xIsNext,
    xIsAI,
    size,
    ruleStyle,
    play,
    listMoves,
    userId,
    isGameCreated,
  ]);

  const handleClick = (row: number, col: number) => {
    if (mode === 1) {
      play(row, col);
    } else if (mode === 0) {
      if (ruleStyle === 0 && xIsNext !== xIsAI) {
        play(row, col);
      }
      if (ruleStyle === 1 && xIsNext !== xIsAI) {
        if (
          currentMove === 0 &&
          (row !== Math.floor(size / 2) || col !== Math.floor(size / 2))
        ) {
          console.error("First move must be at the center");
          return;
        }
        if (
          currentMove === 2 &&
          !isThreecellsAwayFrom(
            [row, col],
            [listMoves[0].row, listMoves[0].col]
          )
        ) {
          console.error(
            "Second move must be at least 3 cells away from the first move"
          );
          return;
        }
        play(row, col);
      }
      if (ruleStyle === 2) {
        if (
          (currentMove <= 2 && !xIsAI) ||
          (xIsNext !== xIsAI && currentMove >= 3)
        ) {
          play(row, col);
        }
      }
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
        setXIsAI(
          Boolean(
            new URLSearchParams(window.location.search).get("starter") === "1"
          )
        );
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
