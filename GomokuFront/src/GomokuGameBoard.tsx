import React, { useCallback, useEffect, useState } from "react";
import axios from "axios";

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

const API_URL = "http://localhost:5000";
const CREATE_GAME_URL = `${API_URL}/create_game`;
const MAKE_MOVE_URL = `${API_URL}/make_move`;
const GET_SUGGESTION_URL = `${API_URL}/get_suggestion`;
const REVERSE_MOVE_URL = `${API_URL}/reverse_move`;
const REAPPLY_MOVE_URL = `${API_URL}/reapply_move`;

export default function Game() {
  const [listMoves, setListMoves] = useState<MoveHistory[]>([]);
  const [currentMove, setCurrentMove] = useState(0);
  const [winner, setWinner] = useState(null);
  const [board, setBoard] = useState(
    Array.from({ length: 19 }, () => Array(19).fill(0))
  );
  const [userId, setUserId] = useState(null);
  const xIsNext = currentMove % 2 === 0;
  const mode = Number(new URLSearchParams(window.location.search).get("mode"));

  // Game Initialization
  useEffect(() => {
    const createGame = async () => {
      try {
        const response = await axios.post(CREATE_GAME_URL, {
          userId: "uniqueUserId",
        }); // Modify to ensure unique userId per session
        if (response.data.success) {
          setUserId(response.data.user_id);
        }
      } catch (error) {
        console.error("Error creating the game:", error);
      }
    };

    createGame();
  }, []);

  useEffect(() => {
    const get_AI_suggestion = async () => {
      if (!userId) return;
      try {
        const response = await axios.get(GET_SUGGESTION_URL, {
          params: { user_id: userId },
        });
        if (response.data.success) {
          const bestMove = getBestMove(response.data.moveEvaluation);
          play(bestMove[0], bestMove[1]);
        }
      } catch (error) {
        console.error("Error getting AI suggestion:", error);
      }
    };

    if (mode === 0 && !xIsNext && currentMove === listMoves.length) {
      get_AI_suggestion();
    }
  }, [currentMove, mode, xIsNext, userId]);

  const getBestMove = useCallback((moveEvaluation: MoveEvaluation) => {
    let bestScore = -Infinity;
    let bestMove = moveEvaluation.move;
    moveEvaluation.listMoves?.forEach((move) => {
      if (move.score > bestScore) {
        bestScore = move.score;
        bestMove = move.move;
      }
    });
    return bestMove;
  }, []);

  const play = useCallback(
    async (row: number, col: number) => {
      if (!userId) return;
      try {
        const response = await axios.post(MAKE_MOVE_URL, {
          user_id: userId,
          row,
          col,
        });
        if (response.data.success) {
          setBoard(response.data.board);
          console.log(currentMove, listMoves.length);
          setListMoves((prevListMoves) => [
            ...prevListMoves.slice(0, currentMove),
            { row, col, moveResult: response.data.moveResult },
          ]);
          setWinner(response.data.winner);
          setCurrentMove((prevMove) => prevMove + 1);
        } else {
          console.error("Invalid move");
        }
      } catch (error) {
        console.error("Error making a move:", error);
      }
    },
    [currentMove, userId]
  );

  const reverse = async () => {
    const response = await axios.post(REVERSE_MOVE_URL, {
      user_id: userId,
      moveResult: listMoves[currentMove - 1].moveResult,
    });
    if (response.data.success) {
      setBoard(response.data.board);
      setCurrentMove((prevMove) => prevMove - 1);
    }
  };

  const reapply = async () => {
    const response = await axios.post(REAPPLY_MOVE_URL, {
      user_id: userId,
      moveResult: listMoves[currentMove].moveResult,
    });
    if (response.data.success) {
      setBoard(response.data.board);
      setCurrentMove((prevMove) => prevMove + 1);
    }
  };

  const onClick = (row: number, col: number) => {
    if (mode === 1 || (mode === 0 && xIsNext)) {
      play(row, col);
    }
  };

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
        <button onClick={() => reverse()} disabled={currentMove === 0}>
          Reverse
        </button>
        <button
          onClick={() => reapply()}
          disabled={currentMove === listMoves.length}
        >
          Reapply
        </button>
      </div>
    </div>
  );
}
