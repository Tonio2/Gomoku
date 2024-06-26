import React from "react";
import Stone from "./Stone";
import { StoneType } from "../interface";

interface ISquare {
  value: string;
  suggestion: number[];
  onSquareClick: () => void;
  nextPlayerColor: number;
  onMouseEnter: () => void;
  onMouseLeave: () => void;
  horizontalHighlight: boolean;
  verticalHighlight: boolean;
  height: number;
}

const Square: React.FC<ISquare> = ({
  value,
  suggestion,
  onSquareClick,
  nextPlayerColor,
  onMouseEnter,
  onMouseLeave,
  horizontalHighlight,
  verticalHighlight,
  height,
}) => {
  const heightString = height.toString();
  return (
    <div
      style={{
        width: `${heightString}px`,
        height: `${heightString}px`,
      }}
      className={`group relative`}
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
      <Stone value={value} type={StoneType.Standard} height={height} />
      {value === "" && (
        <Stone
          value={nextPlayerColor === 0 ? "X" : "O"}
          type={StoneType.Hover}
          height={height}
        />
      )}
      {suggestion[1] > 0 && (
        <Stone
          value={["", "X", "O"][suggestion[1]]}
          type={StoneType.Suggestion}
          text={suggestion[0].toString()}
          height={height}
        />
      )}
    </div>
  );
};

export default Square;
