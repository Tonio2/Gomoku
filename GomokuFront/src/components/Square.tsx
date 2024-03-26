import React from "react";

const stoneStyle = (value: string) => {
  if (value === "") return "";
  let className =
    "absolute top-[50%] left-[50%] ml-[-15px] mt-[-15px] w-[30px] h-[30px] rounded-full shadow-blur hover:scale-110 cursor-pointer";
  if (value === "X") {
    className += " bg-black";
  } else if (value === "O") {
    className += " bg-white";
  }
  return className;
};

interface ISquare {
  value: string;
  suggestion: number[];
  onSquareClick: () => void;
  xIsNext: boolean;
  onMouseEnter: () => void;
  onMouseLeave: () => void;
  horizontalHighlight: boolean;
  verticalHighlight: boolean;
}

const Square: React.FC<ISquare> = ({
  value,
  suggestion,
  onSquareClick,
  xIsNext,
  onMouseEnter,
  onMouseLeave,
  horizontalHighlight,
  verticalHighlight,
}) => {
  return (
    <div
      className="group relative w-[40px] h-[40px]"
      onClick={onSquareClick}
      onMouseEnter={onMouseEnter}
      onMouseLeave={onMouseLeave}
    >
      <div
        className={
          "absolute top-[50%] left-0 w-full h-[1px] bg-[#00ffff80] shadow-blurLine " +
          (horizontalHighlight ? "bg-[#0ff] shadow-strongBlur" : "")
        }
      ></div>
      <div
        className={
          "absolute left-[50%] top-0 h-full w-[1px] bg-[#00ffff80] shadow-blurLine " +
          (verticalHighlight ? "bg-[#0ff] shadow-strongBlur" : "")
        }
      ></div>
      <div className={stoneStyle(value)}></div>
      {value === "" && (
        <div
          className={
            stoneStyle(xIsNext ? "X" : "O") +
            " opacity-0 hover:opacity-50 duration-400"
          }
        ></div>
      )}
      {suggestion[1] > 0 && (
        <div
          className={
            stoneStyle(["", "X", "O"][suggestion[1]]) +
            " opacity-50 text-red-500 text-center text-lg font-bold line-height-[30px]"
          }
        >
          {suggestion[0]}
        </div>
      )}
    </div>
  );
};

export default Square;
