import React from "react";

import Square from "./Square";

import { formatCoordinate } from "../utils/utils";

interface IBoard {
  nextPlayerColor: number;
  board: number[][];
  handleClick: (row: number, col: number) => void;
  suggestionBoard: number[][][];
}

const Board: React.FC<IBoard> = ({
  nextPlayerColor,
  board,
  handleClick,
  suggestionBoard,
}) => {
  const [hoveredCell, setHoveredCell] = React.useState<[number, number]>([
    -1, -1,
  ]);
  return (
    <>
      <div className="flex">
        <div className="w-[40px] leading-[40px]"></div>{" "}
        {/* Placeholder for the top-left corner */}
        {board[0].map((_, col) => (
          <div
            key={col}
            className={
              "w-[40px] leading-[40px] text-center" +
              (hoveredCell[1] === col ? " font-bold text-lg" : "")
            }
          >
            {formatCoordinate(col)}
          </div>
        ))}
      </div>
      {board.map((row, i) => (
        <div className="flex" key={formatCoordinate(i)}>
          <div
            className={
              "h-[40px] w-[40px] text-center leading-[40px]" +
              (hoveredCell[0] === i ? " font-bold text-lg" : "")
            }
          >
            {formatCoordinate(i)}
          </div>
          {row.map((value, j) => (
            <Square
              key={j}
              value={value === 1 ? "X" : value === 2 ? "O" : ""}
              suggestion={suggestionBoard[i][j]}
              onSquareClick={() => handleClick(i, j)}
              nextPlayerColor={nextPlayerColor || 0}
              onMouseEnter={() => setHoveredCell([i, j])}
              onMouseLeave={() => setHoveredCell([-1, -1])}
              horizontalHighlight={i === hoveredCell[0]}
              verticalHighlight={j === hoveredCell[1]}
            />
          ))}
          <div
            className={
              "h-[40px] w-[40px] text-center leading-[40px]" +
              (hoveredCell[0] === i ? " font-bold text-lg" : "")
            }
          >
            {formatCoordinate(i)}
          </div>
        </div>
      ))}
      <div className="flex">
        <div className="w-[40px] leading-[40px]"></div>{" "}
        {/* Placeholder for the top-left corner */}
        {board[0].map((_, col) => (
          <div
            key={col}
            className={
              "w-[40px] leading-[40px] text-center" +
              (hoveredCell[1] === col ? " font-bold text-lg" : "")
            }
          >
            {formatCoordinate(col)}
          </div>
        ))}
      </div>
    </>
  );
};

export default Board;
