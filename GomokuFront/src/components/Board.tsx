import React from "react";

import Square from "./Square";

interface IBoard {
  xIsNext: boolean;
  board: number[][];
  winner: string | null;
  handleClick: (row: number, col: number) => void;
}

const Board: React.FC<IBoard> = ({ xIsNext, board, winner, handleClick }) => {
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

export default Board;
