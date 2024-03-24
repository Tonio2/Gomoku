import React from "react";

interface ISquare {
  value: string;
  onSquareClick: () => void;
}

const Square: React.FC<ISquare> = ({ value, onSquareClick }) => {
  return (
    <button className="square" onClick={onSquareClick}>
      {value}
    </button>
  );
};

export default Square;
