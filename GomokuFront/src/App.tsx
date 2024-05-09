import React from "react";
import { BrowserRouter as Router, Route, Routes } from "react-router-dom";
import GameSetting from "./GameSettings";
import GomokuGameBoard from "./GomokuGameBoard";
import OnlineRoomPage from "./pages/OnlineRoomPage";

import { ToastContainer, toast } from "react-toastify";
import "react-toastify/dist/ReactToastify.css";

const App: React.FC = () => {
  const notify = (msg: string, type: string) => {
    if (type == "error") {
      toast.error(msg);
    }
    if (type === "info") {
      toast.info(msg);
    }
  };

  return (
    <Router>
      <Routes>
        <Route path="/" element={<GameSetting />} />
        <Route path="/game" element={<GomokuGameBoard notify={notify} />} />
        <Route
          path="/online_game"
          element={<OnlineRoomPage notify={notify} />}
        />
      </Routes>
      <ToastContainer
        position="bottom-right"
        autoClose={5000}
        hideProgressBar={false}
        newestOnTop={false}
        closeOnClick
        rtl={false}
        pauseOnFocusLoss
        draggable
        pauseOnHover
        theme="light"
      />
    </Router>
  );
};

export default App;
