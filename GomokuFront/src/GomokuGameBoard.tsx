import React, { useState } from "react";
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
  onPlay: (row: number, col: number) => void;
};

const Board: React.FC<BoardProps> = ({ xIsNext, board, winner, onPlay }) => {
  const handleClick = (i: number, j: number) => {
    onPlay(i, j);
  };

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

export default function Game() {
  const [listMoves, setListMoves] = useState<MoveHistory[]>([]);
  const [currentMove, setCurrentMove] = useState<number>(0);
  const [winner, setWinner] = useState<string | null>(null);
  const xIsNext: boolean = currentMove % 2 === 0;
  const [board, setBoard] = useState<number[][]>(
    Array(19).fill(Array(19).fill(0))
  );

  function handlePlay(row: number, col: number) {
    console.log("Move: " + row + ", " + col);
    const startTime = new Date().getTime();
    socket.emit("makeMove", row, col, (response: any) => {
      if (response.success) {
        setBoard(response.board);
        const nextListMoves: MoveHistory[] = [
          ...listMoves.slice(0, currentMove),
          {
            row: row,
            col: col,
            moveResult: response.moveResult,
          },
        ];
        setListMoves(nextListMoves);
        setWinner(response.winner);
        setCurrentMove(nextListMoves.length);
      } else {
        console.error("Invalid move");
      }

      const endTime = new Date().getTime();
      console.log("Time taken: " + (endTime - startTime) + "ms");
    });
  }

  const moves: JSX.Element[] = listMoves.map(
    (move: MoveHistory, index: number) => {
      return (
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
      );
    }
  );

  function reverseMove() {
    socket.emit(
      "reverseMove",
      listMoves[currentMove - 1].moveResult,
      (response: any) => {
        if (response.success) {
          setBoard(response.board);
          setWinner(null);
          setCurrentMove(currentMove - 1);
        }
      }
    );
  }

  function reapplyMove() {
    socket.emit(
      "reapplyMove",
      listMoves[currentMove].moveResult,
      (response: any) => {
        if (response.success) {
          setBoard(response.board);
          setWinner(response.winner);
          setCurrentMove(currentMove + 1);
        }
      }
    );
  }

  return (
    <div className="game">
      <div className="game-board">
        <Board
          xIsNext={xIsNext}
          board={board}
          winner={winner}
          onPlay={handlePlay}
        />
      </div>
      <div className="game-info">
        <ol>{moves}</ol>
        <button onClick={() => reverseMove()} disabled={currentMove === 0}>
          Back
        </button>
        <button
          onClick={() => reapplyMove()}
          disabled={currentMove === listMoves.length}
        >
          Next
        </button>
      </div>
    </div>
  );
}
