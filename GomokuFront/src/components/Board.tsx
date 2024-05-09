import React from "react";

import Square from "./Square";

import { formatCoordinate } from "../utils/utils";

interface IBoard {
  nextPlayerColor: number;
  board: number[][];
  handleClick: (row: number, col: number) => void;
  suggestionBoard: number[][][];
  height: number;
}

const Board: React.FC<IBoard> = ({
  nextPlayerColor,
  board,
  handleClick,
  suggestionBoard,
  height,
}) => {
  const [hoveredCell, setHoveredCell] = React.useState<[number, number]>([
    -1, -1,
  ]);
  const innerHeight = height - 32;
  const squareHeight = innerHeight / 21;
  const heightString = squareHeight.toString();
  return (
    <>
      <div style={{ width: innerHeight }} className="flex">
        <div
          style={{
            width: `${heightString}px`,
            lineHeight: `${heightString}px`,
          }}
        ></div>{" "}
        {/* Placeholder for the top-left corner */}
        {board[0].map((_, col) => (
          <div
            key={col}
            style={{
              width: `${heightString}px`,
              lineHeight: `${heightString}px`,
            }}
            className={
              "text-center" +
              (hoveredCell[1] === col ? " font-bold text-lg" : "")
            }
          >
            {formatCoordinate(col)}
          </div>
        ))}
      </div>
      {board.map((row, i) => (
        <div
          style={{ width: innerHeight }}
          className="flex"
          key={formatCoordinate(i)}
        >
          <div
            style={{
              width: `${heightString}px`,
              height: `${heightString}`,
              lineHeight: `${heightString}px`,
            }}
            className={
              "text-center" + (hoveredCell[0] === i ? " font-bold text-lg" : "")
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
              height={squareHeight}
            />
          ))}
          <div
            style={{
              width: `${heightString}px`,
              height: `${heightString}`,
              lineHeight: `${heightString}px`,
            }}
            className={
              "text-center" + (hoveredCell[0] === i ? " font-bold text-lg" : "")
            }
          >
            {formatCoordinate(i)}
          </div>
        </div>
      ))}
      <div style={{ width: innerHeight }} className="flex">
        <div
          style={{
            width: `${heightString}px`,
            lineHeight: `${heightString}px`,
          }}
        ></div>{" "}
        {/* Placeholder for the top-left corner */}
        {board[0].map((_, col) => (
          <div
            key={col}
            style={{
              width: `${heightString}px`,
              lineHeight: `${heightString}px`,
            }}
            className={
              "text-center" +
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
