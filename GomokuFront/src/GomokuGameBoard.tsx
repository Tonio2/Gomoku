import React, { useCallback, useEffect, useState } from "react";
import { socket } from "./socket";

const Square: React.FC<{ value: string; onSquareClick: () => void }> = ({
  value,
  onSquareClick,
}) => {
  return (
    <button className="square" onClick={onSquareClick}>
      {value}
    </button>
  );
};

type BoardProps = {
  xIsNext: boolean;
  board: number[][];
  winner: string | null;
  handleClick: (row: number, col: number) => void;
};

const Board: React.FC<BoardProps> = ({
  xIsNext,
  board,
  winner,
  handleClick,
}) => {
  let status: string;
  if (winner) {
    status = "Winner: " + winner;
  } else {
    status = "Next player: " + (xIsNext ? "X" : "O");
  }

  return (
    <>
      <div className="status">{status}</div>
      {board.map((row, i) => (
        <div className="board-row" key={i}>
          {row.map((value, j) => (
            <Square
              key={j}
              value={value === 1 ? "X" : value === 2 ? "O" : ""}
              onSquareClick={() => handleClick(i, j)}
            />
          ))}
        </div>
      ))}
    </>
  );
};

type MoveHistory = {
  row: number;
  col: number;
  moveResult: any;
};

type MoveEvaluation = {
  move: number[]; // [row, col]
  score: number;
  listMoves?: MoveEvaluation[];
};

export default function Game() {
  const [listMoves, setListMoves] = useState<MoveHistory[]>([]);
  const [currentMove, setCurrentMove] = useState<number>(0);
  const [winner, setWinner] = useState<string | null>(null);
  const [board, setBoard] = useState<number[][]>(() =>
    Array.from({ length: 19 }, () => Array(19).fill(0))
  );
  const xIsNext = currentMove % 2 === 0;
  const mode = Number(new URLSearchParams(window.location.search).get("mode"));

  useEffect(() => {
    if (mode === 0 && !xIsNext) {
      get_AI_suggestion();
    }

    const handleSuggestion = (response: any) => {
      if (mode === 0 && !xIsNext) {
        const bestMove: number[] = getBestMove(response.moveEvaluation);
        play(bestMove[0], bestMove[1]);
      }
    };

    socket.on("suggestion", handleSuggestion);

    // Cleanup on unmount
    return () => {
      socket.off("suggestion", handleSuggestion);
    };
  }, [currentMove, mode, xIsNext]);

  const getBestMove = useCallback(
    (moveEvaluation: MoveEvaluation): number[] => {
      let bestScore = -Infinity;
      let bestMove = moveEvaluation.move;
      moveEvaluation.listMoves?.forEach((move) => {
        if (move.score > bestScore) {
          bestScore = move.score;
          bestMove = move.move;
        }
      });
      return bestMove;
    },
    []
  );

  function get_AI_suggestion() {
    socket.emit("getSuggestion");
  }

  const play = useCallback(
    (row: number, col: number) => {
      socket.emit("makeMove", row, col, (response: any) => {
        if (response.success) {
          setBoard(response.board);
          setListMoves((prevListMoves) => [
            ...prevListMoves.slice(0, currentMove),
            { row, col, moveResult: response.moveResult },
          ]);
          setWinner(response.winner);
          setCurrentMove((prevMove) => prevMove + 1);
        } else {
          console.error("Invalid move");
        }
      });
    },
    [currentMove, xIsNext]
  );

  function onClick(row: number, col: number) {
    if (mode === 1 || (mode === 0 && xIsNext)) {
      play(row, col);
    }
  }

  const moves = listMoves.map((move, index) => (
    <li key={index}>
      <button>
        {(index % 2 === 0 ? "X" : "O") +
          ": (" +
          move.row +
          ", " +
          move.col +
          ")"}
      </button>
    </li>
  ));

  return (
    <div className="game">
      <div className="game-board">
        <Board
          xIsNext={xIsNext}
          board={board}
          winner={winner}
          handleClick={onClick}
        />
      </div>
      <div className="game-info">
        <ol>{moves}</ol>
      </div>
    </div>
  );
}
