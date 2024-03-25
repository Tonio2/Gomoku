import React from "react";

import Square from "./Square";

interface IBoard {
  xIsNext: boolean;
  board: number[][];
  handleClick: (row: number, col: number) => void;
}

const Board: React.FC<IBoard> = ({ xIsNext, board, handleClick }) => {
  return (
    <>
      {board.map((row, i) => (
        <div className="flex" key={i}>
          {row.map((value, j) => (
            <Square
              key={j}
              value={value === 1 ? "X" : value === 2 ? "O" : ""}
              onSquareClick={() => handleClick(i, j)}
              xIsNext={xIsNext}
            />
          ))}
        </div>
      ))}
    </>
  );
};

export default Board;
