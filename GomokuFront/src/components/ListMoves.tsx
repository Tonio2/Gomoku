import React, { useEffect, useRef } from "react";

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
  const endOfListRef = useRef<HTMLTableRowElement>(null);

  for (let i = 0; i < moves.length; i += 1) {
    const firstMove = moves[i];

    const ref = currentMove === i + 1 ? endOfListRef : null
    formattedMoves.push(
      <tr key={i} ref={ref}>
        <td className="text-left py-2" style={moveStyle(currentMove === i + 1)}>
          {i + 1}. {firstMove}
        </td>
      </tr>,
    );
  }

  useEffect(() => {
    // Scrolls to the bottom of the list every time the 'moves' array changes
    if (endOfListRef.current) {
      endOfListRef.current.scrollIntoView({ behavior: 'smooth' });
    }
  }, [moves, currentMove]); // Only re-run the effect if 'moves' changes

  return (
    <div className="flex-grow overflow-y-auto shadow-blur custom-scrollbar">
      <table className="border-separate border-spacing-0 w-full bg-[rgba(10,23,38,0.8)]">
        <tbody>
          {formattedMoves}
        </tbody>
      </table>
    </div>
  );
};

export default ListMoves;

