import React from "react";

const PlayerAvailableStatus: React.FC<{ready: boolean}> = ({ready}) => {
  const colorClass = ready ? 'bg-green-500' : 'bg-gray-500';

  return (
    <div className={`w-6 h-6 rounded-full ${colorClass}`}></div>
  );
}

export default PlayerAvailableStatus;
