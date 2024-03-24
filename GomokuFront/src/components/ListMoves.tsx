import React from "react";

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
        <td>{i + 1}</td>
        <td style={currentMove === i + 1 ? { fontWeight: "bold" } : {}}>
          {firstMove ? `(${firstMove.row}, ${firstMove.col})` : ""}
        </td>
        <td style={currentMove === i + 2 ? { fontWeight: "bold" } : {}}>
          {secondMove ? `(${secondMove.row}, ${secondMove.col})` : ""}
        </td>
      </tr>
    );
  }
  return (
    <table>
      <thead>
        <tr>
          <th>Move</th>
          <th>X</th>
          <th>O</th>
        </tr>
      </thead>
      <tbody>{formattedMoves}</tbody>
    </table>
  );
};

export default ListMoves;
