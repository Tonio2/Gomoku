// import { useCallback, useEffect, useMemo, useState } from "react";

// import {
//   getBestMove,
//   uniqueUserID,
//   emptyBoard,
//   logMoveHistory,
//   getWorstMove,
//   emptySuggestionBoard,
// } from "../utils/utils";
// import api from "../utils/api";

// import { MoveHistory, Mode, Rule } from "../interface";

// type GameLogic = {
//   board: number[][];
//   listMoves: MoveHistory[];
//   currentMove: number;
//   xIsNext: boolean;
//   winner: number;
//   playerX: { score: number; time: number };
//   playerO: { score: number; time: number };
//   suggestionBoard: number[][][]; // store [currentMove, value] for each cell
//   handleClick: (row: number, col: number) => void;
//   handleReverse: () => void;
//   handleReapply: () => void;
//   handleReset: () => void;
// };

// const useGameLogic = (): GameLogic => {
//   const size = useMemo(
//     () => Number(new URLSearchParams(window.location.search).get("size")),
//     []
//   );
//   const userId = useMemo(() => uniqueUserID(), []);

//   const mode = useMemo(
//     () => Number(new URLSearchParams(window.location.search).get("mode")),
//     []
//   );

//   const ruleStyle = useMemo(
//     () => Number(new URLSearchParams(window.location.search).get("ruleStyle")),
//     []
//   );
//   const [board, setBoard] = useState<number[][]>(emptyBoard(size));
//   const [listMoves, setListMoves] = useState<MoveHistory[]>([]);
//   const [currentMove, setCurrentMove] = useState<number>(0);
//   const xIsNext = currentMove % 2 === 0;
//   const [isGameOver, setIsGameOver] = useState<boolean>(false);
//   const [winner, setWinner] = useState<number>(0);
//   const [playerX, setPlayerX] = useState({ score: 0, time: 0 });
//   const [playerO, setPlayerO] = useState({ score: 0, time: 0 });
//   const [suggestionBoard, setSuggestionsBoard] = useState<number[][][]>(
//     emptySuggestionBoard(size)
//   );

//   const [isGameCreated, setIsGameCreated] = useState(false);
//   const [xIsAI, setXIsAI] = useState<boolean>(
//     Boolean(
//       new URLSearchParams(window.location.search).get("starter") ===
//         Mode.PVP.toString()
//     )
//   );
//   const [swapDecisionMade, setSwapDecisionMade] = useState(false);
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

//   useEffect(() => {
//     const createGame = async () => {
//       try {
//         const { success, message } = await api.createGame(userId, mode, size);
//         if (success) {
//           setIsGameCreated(true);
//         } else {
//           console.error(message);
//         }
//       } catch (error) {
//         console.error("Server error");
//       }
//     };

//     createGame();
//   }, [userId, mode, size]);

//   const play = useCallback(
//     async (row: number, col: number) => {
//       try {
//         const {
//           success,
//           message,
//           moveResult,
//           newBoard,
//           newWinner,
//           newIsGameOver,
//         } = await api.makeMove(userId, row, col);
//         if (success) {
//           setBoard(newBoard);
//           setListMoves((prevListMoves) => [
//             ...prevListMoves.slice(0, currentMove),
//             { row, col, moveResult: moveResult },
//           ]);
//           setWinner(newWinner);
//           setIsGameOver(newIsGameOver);
//           setCurrentMove((prevMove) => prevMove + 1);
//           setPlayerX((prevPlayer) => ({
//             ...prevPlayer,
//             score: prevPlayer.score + moveResult.black_score_change,
//           }));
//           setPlayerO((prevPlayer) => ({
//             ...prevPlayer,
//             score: prevPlayer.score + moveResult.white_score_change,
//           }));
//           setSuggestionsBoard(emptySuggestionBoard(size));
//         } else {
//           console.error(message);
//         }
//       } catch (error: any) {
//         console.error("Server error");
//       }
//     },
//     [currentMove, userId, xIsNext]
//   );

//   const isThreecellsAwayFrom = (candidate: number[], target: number[]) => {
//     return (
//       Math.abs(candidate[0] - target[0]) >= 3 ||
//       Math.abs(candidate[1] - target[1]) >= 3
//     );
//   };

//   useEffect(() => {
//     const display_AI_suggestion = async () => {
//       try {
//         let { success, message, moveEvaluation } = await api.getSuggestion(
//           userId
//         );
//         if (success) {
//           const newSuggestionBoard = emptySuggestionBoard(size);
//           let maximizing = true;
//           let index = 1;
//           while (
//             moveEvaluation.listMoves &&
//             moveEvaluation.listMoves.length > 0
//           ) {
//             const move = maximizing
//               ? getBestMove(moveEvaluation, (move: number[]) => true)
//               : getWorstMove(moveEvaluation, (move: number[]) => true);
//             newSuggestionBoard[move.move[0]][move.move[1]] = [
//               currentMove + index,
//               xIsNext === maximizing ? 1 : 2,
//             ];
//             moveEvaluation = move;
//             maximizing = !maximizing;
//             index++;
//           }
//           setSuggestionsBoard(newSuggestionBoard);
//         } else {
//           console.error(message);
//         }
//       } catch (error) {
//         console.error("Server error");
//       }
//     };

//     const play_AI_suggestion = async (
//       predicate: (move: number[]) => boolean = (move: number[]) => true
//     ) => {
//       try {
//         const { success, message, moveEvaluation } = await api.getSuggestion(
//           userId
//         );
//         if (success) {
//           const bestMove = getBestMove(moveEvaluation, predicate);
//           await play(bestMove.move[0], bestMove.move[1]);
//         } else {
//           console.error(message);
//         }
//       } catch (error) {
//         console.error("Server error");
//       }
//     };

//     const swap_AI_suggestion = async () => {
//       try {
//         const { success, message, moveEvaluation } = await api.getSuggestion(
//           userId
//         );
//         if (success) {
//           if (moveEvaluation.score < 0) {
//             setXIsAI(true);
//             console.log("AI takes X");
//           } else {
//             console.log("AI takes O");
//           }
//         } else {
//           console.error(message);
//           console.log("AI takes O");
//         }
//       } catch (error) {
//         console.error("Server error");
//         console.log("AI takes O");
//       }
//       setSwapDecisionMade(true);
//       setTimerStarted(true);
//     };

//     const playAI = async () => {
//       if (ruleStyle === Rule.Standard) {
//         if (xIsNext === xIsAI) {
//           if (currentMove === 0) {
//             await play(Math.floor(size / 2), Math.floor(size / 2)); // Play around the center
//           } else {
//             await play_AI_suggestion();
//           }
//         }
//       } else if (ruleStyle === Rule.Pro) {
//         if (xIsNext === xIsAI) {
//           if (currentMove === 0) {
//             await play(Math.floor(size / 2), Math.floor(size / 2));
//           } else {
//             let predicate = (move: number[]) => true;
//             if (currentMove === 2)
//               predicate = (move: number[]) =>
//                 isThreecellsAwayFrom(move, [
//                   listMoves[0].row,
//                   listMoves[0].col,
//                 ]);
//             await play_AI_suggestion(predicate);
//           }
//         }
//       } else if (ruleStyle === Rule.Swap) {
//         if (currentMove === 3 && !xIsAI && !swapDecisionMade) {
//           await swap_AI_suggestion();
//         } else if (
//           (currentMove <= 2 && xIsAI) ||
//           (xIsNext === xIsAI && currentMove >= 3)
//         ) {
//           if (currentMove === 0) {
//             await play(Math.floor(size / 2), Math.floor(size / 2));
//           } else {
//             await play_AI_suggestion();
//           }
//         }
//       }
//     };

//     const askUserForChoice = () => {
//       let success = false;
//       if (
//         ruleStyle === Rule.Swap &&
//         currentMove === 3 &&
//         xIsAI &&
//         !swapDecisionMade
//       ) {
//         while (!success) {
//           const choice = prompt("Do you want to take X or O?");
//           if (choice === "X") {
//             setXIsAI(false);
//             console.log("You take X");
//             success = true;
//           } else if (choice === "O") {
//             console.log("You take O");
//             success = true;
//           } else {
//             console.error("You must enter either X or O");
//           }
//         }
//         setSwapDecisionMade(true);
//         setTimerStarted(true);
//       }
//     };

//     if (
//       isGameCreated &&
//       mode === Mode.PVAI &&
//       currentMove === listMoves.length
//     ) {
//       askUserForChoice();
//       playAI();
//     }

//     if (mode === Mode.PVP && ruleStyle === Rule.Swap && currentMove === 3) {
//       alert("Have you made your choice ?");
//       setTimerStarted(true);
//     }

//     if (
//       mode === Mode.PVP &&
//       currentMove === listMoves.length &&
//       !isGameOver &&
//       currentMove !== 0
//     ) {
//       display_AI_suggestion();
//     }
//     logMoveHistory(listMoves);
//   }, [
//     currentMove,
//     mode,
//     xIsNext,
//     xIsAI,
//     size,
//     ruleStyle,
//     play,
//     listMoves,
//     userId,
//     isGameCreated,
//   ]);

//   const handleClick = useCallback(
//     async (row: number, col: number) => {
//       if (mode === Mode.PVP) {
//         await play(row, col);
//       } else if (mode === Mode.PVAI) {
//         if (ruleStyle === Rule.Standard && xIsNext !== xIsAI) {
//           await play(row, col);
//         }
//         if (ruleStyle === Rule.Pro && xIsNext !== xIsAI) {
//           if (
//             currentMove === 0 &&
//             (row !== Math.floor(size / 2) || col !== Math.floor(size / 2))
//           ) {
//             console.error("First move must be at the center");
//             return;
//           }
//           if (
//             currentMove === 2 &&
//             !isThreecellsAwayFrom(
//               [row, col],
//               [listMoves[0].row, listMoves[0].col]
//             )
//           ) {
//             console.error(
//               "Second move must be at least 3 cells away from the first move"
//             );
//             return;
//           }
//           await play(row, col);
//         }
//         if (ruleStyle === Rule.Swap) {
//           if (
//             (currentMove <= 2 && !xIsAI) ||
//             (xIsNext !== xIsAI && currentMove >= 3)
//           ) {
//             await play(row, col);
//           }
//         }
//       }
//     },
//     [currentMove, mode, xIsNext, xIsAI, size, ruleStyle, play, listMoves]
//   );

//   const reverse = async () => {
//     try {
//       const { success, newBoard } = await api.reverseMove(
//         userId,
//         listMoves[currentMove - 1].moveResult
//       );
//       if (success) {
//         setBoard(newBoard);
//         setCurrentMove((prevMove) => prevMove - 1);
//         setPlayerX((prevPlayer) => ({
//           ...prevPlayer,
//           score:
//             prevPlayer.score -
//             listMoves[currentMove - 1].moveResult.black_score_change,
//         }));
//         setPlayerO((prevPlayer) => ({
//           ...prevPlayer,
//           score:
//             prevPlayer.score -
//             listMoves[currentMove - 1].moveResult.white_score_change,
//         }));
//       }
//     } catch (error: any) {
//       console.error("Server error");
//     }
//   };

//   const reapply = async () => {
//     try {
//       const { success, newBoard } = await api.reapplyMove(
//         userId,
//         listMoves[currentMove].moveResult
//       );
//       if (success) {
//         setBoard(newBoard);
//         setCurrentMove((prevMove) => prevMove + 1);
//         setPlayerX((prevPlayer) => ({
//           ...prevPlayer,
//           score:
//             prevPlayer.score +
//             listMoves[currentMove].moveResult.black_score_change,
//         }));
//         setPlayerO((prevPlayer) => ({
//           ...prevPlayer,
//           score:
//             prevPlayer.score +
//             listMoves[currentMove].moveResult.white_score_change,
//         }));
//       }
//     } catch (error) {
//       console.error("Server error");
//     }
//   };

//   const reset = async () => {
//     try {
//       const { success, message } = await api.resetGame(userId);
//       if (success) {
//         setBoard(emptyBoard(size));
//         setListMoves([]);
//         setCurrentMove(0);
//         setWinner(0);
//         setPlayerX({ score: 0, time: 0 });
//         setPlayerO({ score: 0, time: 0 });
//         setTimerStarted(ruleStyle === Rule.Standard || ruleStyle === Rule.Pro);
//         setIsGameOver(false);

//         setXIsAI(
//           Boolean(
//             new URLSearchParams(window.location.search).get("starter") === "1"
//           )
//         );
//       } else {
//         console.error(message);
//       }
//     } catch (error) {
//       console.error("Server error");
//     }
//   };

//   return {
//     board,
//     listMoves,
//     currentMove,
//     xIsNext,
//     winner,
//     playerX,
//     playerO,
//     suggestionBoard,
//     handleClick: handleClick,
//     handleReverse: reverse,
//     handleReapply: reapply,
//     handleReset: reset,
//   };
// };

// export default useGameLogic;
export {};
