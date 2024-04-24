export interface MoveResult {
  cell_changes: {
    row: number;
    col: number;
    old_value: number;
    new_value: number;
  }[];
  white_score_change: number;
  black_score_change: number;
}

export interface MoveHistory {
  row: number;
  col: number;
  moveResult: MoveResult;
}

export interface MoveEvaluation {
  move: number[]; // [row, col]
  score: number;
  listMoves?: MoveEvaluation[];
}

export enum Mode {
  PVAI,
  PVP,
}

export enum Rule {
  Standard,
  Pro,
  Swap,
}

export interface Player {
  color: number;
  score: number;
  time: number;
  isAI: boolean;
}

export interface ActionResult {
  success: boolean;
  message: string;
  _board: number[][];
  _isGameOver: boolean;
  _winner: number;
  _nextPlayer: number;
  _nextAction: number;
  _listMoves: string[];
  _currentMove: number;
  _players: Player[];
  _hasPendingAction: boolean;
}

export enum StoneType {
  Standard,
  Hover,
  Suggestion,
  Display,
}
