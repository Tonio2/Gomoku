import React from "react";

import { StoneType } from "../interface";

const stoneClassName = (value: string, type: StoneType, height: number) => {
  const suffix = [
    "",
    " opacity-0 hover:opacity-50 duration-400",
    ` opacity-50 text-red-500 text-center text-lg font-bold`,
    "",
  ][type];
  if (value === "") return suffix;
  let position = ` absolute top-[50%] left-[50%]`;
  let style = `rounded-full shadow-blur`;
  let animation = " hover:scale-110 cursor-pointer";
  if (value === "X") {
    style += " bg-black";
  } else if (value === "O") {
    style += " bg-white";
  }
  return (
    style + (type !== StoneType.Display ? position + animation : "") + suffix
  );
};

const stoneStyle = (value: string, type: StoneType, height: number) => {
  const stoneHeight = Math.floor(height * 0.75);
  const halfStoneHeight = Math.floor(stoneHeight / 2);

  const style: any = {
    lineHeight: `${stoneHeight}px`,
  };

  if (value === "") return style;

  style.width = `${stoneHeight}px`;
  style.height = `${stoneHeight}px`;

  if (type !== StoneType.Display) {
    style.marginLeft = `-${halfStoneHeight}px`;
    style.marginTop = `-${halfStoneHeight}px`;
  }

  return style;
};

interface IStone {
  value: string;
  type: StoneType;
  text?: string;
  height: number;
}

const Stone: React.FC<IStone> = ({ value, type, text, height }) => {
  const style = stoneStyle(value, type, height);
  return (
    <div style={style} className={stoneClassName(value, type, height)}>
      {text && text}
    </div>
  );
};

export default Stone;
