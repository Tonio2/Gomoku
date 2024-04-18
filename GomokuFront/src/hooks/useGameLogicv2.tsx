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

import { MoveHistory, Mode, Rule, Player } from "../interface";

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
  const [players, setPlayers] = useState<any[]>([
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
        const { success, message, players, newNextAction, newNextPlayer } =
          await api.createGame(userId, mode, size);
        if (success) {
          setIsGameCreated(true);
          setPlayers(players);
          setNextAction(newNextAction);
          setNextPlayer(newNextPlayer);
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
        const {
          success,
          newBoard,
          newIsGameOver,
          newWinner,
          newNextPlayer,
          newNextAction,
          newListMoves,
          newCurrentMove,
          newPlayers,
        } = await api.makeMove(userId, row, col);
        if (success) {
          setBoard(newBoard);
          setListMoves(newListMoves);
          setWinner(newWinner);
          setIsGameOver(newIsGameOver);
          setCurrentMove(newCurrentMove);
          setPlayers(newPlayers);
          setNextPlayer(newNextPlayer);
          setNextAction(newNextAction);
          setSuggestionsBoard(emptySuggestionBoard(size));
        } else {
          console.error("TODO: handle error message from server");
        }
      } catch (error: any) {
        console.error("Server error");
      }
    },
    [userId]
  );

  const handleClick = async (row: number, col: number) => {
    play(row, col);
  };

  const reverse = () => {};
  const reapply = () => {};
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
