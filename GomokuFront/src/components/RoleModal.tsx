import React from "react";
import "../RoleModal.css";

interface IRoleModal {
  roles: boolean[];
  onRoleSelected: (index: number) => void;
  isVisible: boolean;
}

const style = (role: boolean) => {
  let style = "p-4 cursor-pointer rounded text-center text-xl ";
  style += role ? "bg-[#112d4e] hover:bg-[#3f72af]" : "bg-[#111]";
  return style;
};

const roleNames = ["Spectator", "Player 1", "Player 2"];

const RoleModal: React.FC<IRoleModal> = ({
  roles,
  onRoleSelected,
  isVisible,
}) => {
  if (!isVisible) return null;

  return (
    <>
      <div className="fixed inset-0 bg-[#0a0f0d] opacity-75"></div>
      <div className="fixed inset-0 flex items-center justify-center">
        <div className="bg-dark-blue p-5 rounded-lg shadow-neon">
          <ul className="grid grid-cols-3 gap-4">
            {roles.map((role, index) => (
              <button
                key={index}
                className={style(role)}
                onClick={() => onRoleSelected(index)}
                disabled={!role}
              >
                {`${roleNames[index]}`}
              </button>
            ))}
          </ul>
        </div>
      </div>
    </>
  );
};

export default RoleModal;
