import React from "react";

import { StoneType } from "../interface";

const stoneStyle = (value: string, type: StoneType) => {
  const suffix = [
    "",
    " opacity-0 hover:opacity-50 duration-400",
    " opacity-50 text-red-500 text-center text-lg font-bold line-height-[30px]",
    "",
  ][type];
  if (value === "") return suffix;
  let position = " absolute top-[50%] left-[50%] ml-[-15px] mt-[-15px]";
  let style = "rounded-full shadow-blur w-[30px] h-[30px]";
  let animation = " hover:scale-110 cursor-pointer";
  if (value === "X") {
    style += " bg-black";
  } else if (value === "O") {
    style += " bg-white";
  }
  return (
    style + (type != StoneType.Display ? position + animation : "") + suffix
  );
};

interface IStone {
  value: string;
  type: StoneType;
  text?: string;
}

const Stone: React.FC<IStone> = ({ value, type, text }) => {
  return <div className={stoneStyle(value, type)}>{text && text}</div>;
};

export default Stone;
