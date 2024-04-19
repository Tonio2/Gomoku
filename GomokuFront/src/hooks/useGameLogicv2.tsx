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
  listMoves: MoveHistory[];
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

  //   const [timerStarted, setTimerStarted] = useState(
  //     ruleStyle === Rule.Standard || ruleStyle === Rule.Pro
  //   );

  //   useEffect(() => {
  //     const timer = setInterval(() => {
  //       if (timerStarted && !isGameOver) {
  //         if (xIsNext) {
  //           setPlayerX((prevPlayer) => ({
  //             ...prevPlayer,
  //             time: prevPlayer.time + 0.1,
  //           }));
  //         } else {
  //           setPlayerO((prevPlayer) => ({
  //             ...prevPlayer,
  //             time: prevPlayer.time + 0.1,
  //           }));
  //         }
  //       }
  //     }, 100);

  //     return () => clearInterval(timer);
  //   }, [timerStarted, xIsNext, isGameOver]);

  useEffect(() => {
    const createGame = async () => {
      try {
        const res = await api.createGame(userId, mode, size);
        handleMoveResponse(res);
      } catch (error) {
        console.error("Server error");
      }
    };

    createGame();
  }, [userId, mode, size]);

  const updateBoard = (res: ActionResult) => {
    setBoard(res._board);
    setIsGameOver(res._isGameOver);
    setWinner(res._winner);
    setListMoves(res._listMoves);
    setCurrentMove(res._currentMove);
    setPlayers(res._players);
  };

  const handleMoveResponse = useCallback(
    async (res: ActionResult) => {
      if (!res.success) {
        console.error(res.message);
        return;
      }
      console.log(res);
      console.log(res._players[res._nextPlayer].isAI);
      updateBoard(res);
      if (!res._players[res._nextPlayer].isAI) return;
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
