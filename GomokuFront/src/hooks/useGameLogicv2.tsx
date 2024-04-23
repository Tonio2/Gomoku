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
  nextPlayer: number;
  nextAction: number;
  listMoves: string[];
  currentMove: number;
  xIsNext: boolean;
  winner: number;
  players: Player[];
  suggestionBoard: number[][][]; // store [currentMove, value] for each cell
  handleClick: (row: number, col: number) => void;
  handleReverse: () => void;
  handleReapply: () => void;
  handleReset: () => void;
};

function getTimeDifferenceInSeconds(dateStringFromBackend: string): number {
  // Convert the ISO string to a Date object
  const backendDate = new Date(dateStringFromBackend);

  // Get the current date and time
  const currentDate = new Date();

  // Compute the difference in milliseconds
  const differenceInMilliseconds = currentDate.getTime() - backendDate.getTime() + 7200000;

  // Convert milliseconds to seconds
  return differenceInMilliseconds / 1000;
}

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

  const ruleStyle = useMemo(
    () => Number(new URLSearchParams(window.location.search).get("ruleStyle")),
    []
  );
  const [board, setBoard] = useState<number[][]>(emptyBoard(size));
  const [listMoves, setListMoves] = useState<string[]>([]);
  const [currentMove, setCurrentMove] = useState<number>(0);
  const xIsNext = currentMove % 2 === 0;
  const [isGameOver, setIsGameOver] = useState<boolean>(false);
  const [winner, setWinner] = useState<number>(0);
  const [players, setPlayers] = useState<Player[]>([
    {
      color: 0,
      isAI: false,
      score: 0,
      time: 0,
    },
    {
      color: 0,
      isAI: false,
      score: 0,
      time: 0,
    },
  ]);
  const [nextAction, setNextAction] = useState<number>(0);
  const [nextPlayer, setNextPlayer] = useState<number>(0);
  const [suggestionBoard, setSuggestionsBoard] = useState<number[][][]>(
    emptySuggestionBoard(size)
  );

  const [isGameCreated, setIsGameCreated] = useState(false);


  useEffect(() => {
    const createGame = async () => {
      try {
        const res = await api.createGame(userId, mode, size, ruleStyle, starter);
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
    setWinner(res._winner);
    setListMoves(res._listMoves);
    setCurrentMove(res._currentMove);
    setPlayers(res._players);
    setNextPlayer(res._nextPlayer);
  };

  const handleMoveResponse = useCallback(
    async (res: ActionResult) => {
      if (!res.success) {
        console.error(res.message);
        return;
      }
      console.log(res);
      updateBoard(res);
      if (!res._players[res._nextPlayer].isAI) {
          if (res._nextAction == 1) {
              console.log("Hello");
              let input = ""
              while (input.toLowerCase() != "y" && input.toLowerCase() != "n")
                  input = prompt("swap? (Y/N)") || ""
              const _res = await api.swap(userId, input.toLowerCase() === "y")
              handleMoveResponse(_res)
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
    [userId]
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
    [userId]
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
    nextPlayer,
    nextAction,
    listMoves,
    currentMove,
    xIsNext,
    winner,
    players,
    suggestionBoard,
    handleClick: handleClick,
    handleReverse: reverse,
    handleReapply: reapply,
    handleReset: reset,
  };
};

export default useGameLogic;
