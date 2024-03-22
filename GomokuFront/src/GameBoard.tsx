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
  board: string[];
  winner: string | null;
  onPlay: (move: number) => void;
};

const Board: React.FC<BoardProps> = ({ xIsNext, board, winner, onPlay }) => {
  const handleClick = (i: number) => {
    onPlay(i);
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
      <div className="board-row">
        <Square value={board[0]} onSquareClick={() => handleClick(0)} />
        <Square value={board[1]} onSquareClick={() => handleClick(1)} />
        <Square value={board[2]} onSquareClick={() => handleClick(2)} />
      </div>
      <div className="board-row">
        <Square value={board[3]} onSquareClick={() => handleClick(3)} />
        <Square value={board[4]} onSquareClick={() => handleClick(4)} />
        <Square value={board[5]} onSquareClick={() => handleClick(5)} />
      </div>
      <div className="board-row">
        <Square value={board[6]} onSquareClick={() => handleClick(6)} />
        <Square value={board[7]} onSquareClick={() => handleClick(7)} />
        <Square value={board[8]} onSquareClick={() => handleClick(8)} />
      </div>
    </>
  );
};

export default function Game() {
  const [listMoves, setListMoves] = useState<number[]>([]);
  const [currentMove, setCurrentMove] = useState<number>(0);
  const [winner, setWinner] = useState<string | null>(null);
  const xIsNext: boolean = currentMove % 2 === 0;
  const [board, setBoard] = useState<string[]>(Array(9).fill(null));

  function handlePlay(move: number) {
    console.log("Move: " + move);
    const startTime = new Date().getTime();
    socket.emit("makeMove", move, (response: any) => {
      if (response.success) {
        const endTime = new Date().getTime();
        console.log("Time taken: " + (endTime - startTime) + "ms");
        setBoard(response.board);
        const nextListMoves: number[] = [
          ...listMoves.slice(0, currentMove),
          move,
        ];
        setListMoves(nextListMoves);
        setWinner(response.winner);
        setCurrentMove(nextListMoves.length);
      } else {
        console.error("Invalid move");
      }
    });
  }

  const moves: JSX.Element[] = listMoves.map((index: number, move: number) => {
    return (
      <li key={move}>
        <button>{(move % 2 === 0 ? "X" : "O") + ": " + index}</button>
      </li>
    );
  });

  function reverseMove() {
    socket.emit("reverseMove", listMoves[currentMove - 1], (response: any) => {
      if (response.success) {
        setBoard(response.board);
        setWinner(null);
        setCurrentMove(currentMove - 1);
      }
    });
  }

  function reapplyMove() {
    socket.emit("reapplyMove", listMoves[currentMove], (response: any) => {
      if (response.success) {
        setBoard(response.board);
        setWinner(response.winner);
        setCurrentMove(currentMove + 1);
      }
    });
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
