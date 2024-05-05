import { io } from "socket.io-client";

const URL = process.env.REACT_APP_BACKEND_URL || "http://localhost:5000";

const socket = io(URL);

export default socket;
