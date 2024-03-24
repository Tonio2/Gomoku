import { MoveEvaluation } from "../interface";

export const getBestMove = (moveEvaluation: MoveEvaluation) => {
  let bestScore = -Infinity;
  let bestMove = moveEvaluation.move;
  moveEvaluation.listMoves?.forEach((move) => {
    if (move.score > bestScore) {
      bestScore = move.score;
      bestMove = move.move;
    }
  });
  return bestMove;
};

export const uniqueUserID = () => {
  return Math.random().toString(36).substring(2, 15);
};

export const emptyBoard = () => {
  return Array(19)
    .fill(null)
    .map(() => Array(19).fill(null));
};
