import { MoveEvaluation, MoveHistory } from "../interface";

export const getBestMove = (
  moveEvaluation: MoveEvaluation,
  predicate: (move: number[]) => boolean
) => {
  let bestScore = -Infinity;
  let bestMove = moveEvaluation.move;
  moveEvaluation.listMoves?.forEach((move) => {
    if (move.score > bestScore && predicate(move.move)) {
      bestScore = move.score;
      bestMove = move.move;
    }
  });
  return bestMove;
};

export const uniqueUserID = () => {
  return Math.random().toString(36).substring(2, 15);
};

export const emptyBoard = (size: number) => {
  return Array(size)
    .fill(null)
    .map(() => Array(size).fill(null));
};

export const formatMove = (row: number, col: number) => {
  const coordinates = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  return coordinates[row] + coordinates[col];
};

export const logMoveHistory = (listMoves: MoveHistory[]) => {
  let str = "";
  for (const move of listMoves) {
    str += formatMove(move.row, move.col) + ",";
  }

  console.log(str.slice(0, -1));
};
