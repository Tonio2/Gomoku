import React from "react";

const moveStyle = (isCurrentMove: boolean) => ({
  fontWeight: isCurrentMove ? "bold" : "normal",
  color: isCurrentMove ? "#4affef" : "#fff", // Neon color for current move
  backgroundColor: isCurrentMove ? "rgba(10, 23, 38, 0.5)" : "transparent", // Dark translucent background for contrast
});

interface IListMoves {
  moves: { row: number; col: number; moveResult: any }[];
  currentMove: number;
}

const ListMoves: React.FC<IListMoves> = ({ moves, currentMove }) => {
  const formattedMoves = [];
  for (let i = 0; i < moves.length; i += 2) {
    const firstMove = moves[i];
    const secondMove = moves[i + 1];

    formattedMoves.push(
      <tr key={i}>
        <td style={moveStyle(currentMove === i + 1)}>
          {i + 1}. {firstMove ? `(${firstMove.row}, ${firstMove.col})` : ""}
        </td>
        <td style={moveStyle(currentMove === i + 2)}>
          {i + 2}. {secondMove ? `(${secondMove.row}, ${secondMove.col})` : ""}
        </td>
      </tr>
    );
  }

  return (
    <div className="flex-grow overflow-y-auto shadow-blur custom-scrollbar">
      <table className="border-separate border-spacing-0 w-full bg-[rgba(10,23,38,0.8)]">
        <thead>
          <tr>
            <th>X</th>
            <th>O</th>
          </tr>
        </thead>
        <tbody>{formattedMoves}</tbody>
      </table>
    </div>
  );
};

export default ListMoves;
