import React from "react";

interface IButton {
  onClick: () => void;
  disabled?: boolean;
  text: string;
  classNames?: string;
}

const Button: React.FC<IButton> = ({
  onClick,
  disabled = false,
  text,
  classNames = "",
}) => {
  return (
    <button
      className={
        "bg-[#333] mt-5 color-white px-5 py-2.5 text-center text-based duration-400 cursor-pointer border-1 border-solid border-[#fff3] shadow-buttonBlur hover:shadow-buttonBlurHover " +
        classNames
      }
      onClick={onClick}
      disabled={disabled}
    >
      {text}
    </button>
  );
};

export default Button;
