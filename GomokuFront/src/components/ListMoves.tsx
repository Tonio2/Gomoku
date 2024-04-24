import React from "react";

const moveStyle = (isCurrentMove: boolean) => ({
  fontWeight: isCurrentMove ? "bold" : "normal",
  color: isCurrentMove ? "#4affef" : "#fff", // Neon color for current move
  backgroundColor: isCurrentMove ? "rgba(10, 23, 38, 0.5)" : "transparent", // Dark translucent background for contrast
});

interface IListMoves {
  moves: string[];
  currentMove: number;
}

const ListMoves: React.FC<IListMoves> = ({ moves, currentMove }) => {
  const formattedMoves = [];
  for (let i = 0; i < moves.length; i += 1) {
    const firstMove = moves[i];

    formattedMoves.push(
      <tr key={i}>
        <td className="text-left py-2" style={moveStyle(currentMove === i + 1)}>
          {i + 1}. {firstMove}
        </td>
      </tr>,
    );
  }

  return (
    <div className="flex-grow overflow-y-auto shadow-blur custom-scrollbar">
      <table className="border-separate border-spacing-0 w-full bg-[rgba(10,23,38,0.8)]">
        <tbody>{formattedMoves}</tbody>
      </table>
    </div>
  );
};

export default ListMoves;
