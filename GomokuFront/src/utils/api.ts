import axios from "axios";
import { MoveEvaluation, ActionResult } from "../interface";

const API_URL = process.env.REACT_APP_BACKEND_URL || "http://localhost:5000";
const CREATE_GAME_URL = `${API_URL}/create_room`;
const RESET_GAME_URL = `${API_URL}/reset_room`;
const MAKE_MOVE_URL = `${API_URL}/make_move`;
const GET_SUGGESTION_URL = `${API_URL}/get_suggestion`;
const REVERSE_MOVE_URL = `${API_URL}/reverse_move`;
const REAPPLY_MOVE_URL = `${API_URL}/reapply_move`;
const AI_TURN_URL = `${API_URL}/ai_turn`;
const SWAP_URL = `${API_URL}/swap`;
const CREATE_ONLINE_GAME_URL = `${API_URL}/create_online_room`;
const GET_AVAILABLE_ROLES_URL = `${API_URL}/get_online_room_roles/`;

const createGame = async (
  userId: string,
  mode: number,
  size: number,
  ruleStyle: number,
  starter: number
): Promise<ActionResult> => {
  const response = await axios.post(CREATE_GAME_URL, {
    userId: userId,
    mode: mode,
    size: size,
    rule_style: ruleStyle,
    ai_player: 1 - starter,
  });
  return response.data;
};

const createOnlineGame = async (
  size: number,
  ruleStyle: number
): Promise<{
  success: boolean;
  roomId: string;
}> => {
  const response = await axios.post(CREATE_ONLINE_GAME_URL, {
    size: size,
    rule_style: ruleStyle,
  });
  return response.data;
};

const getAvailableRoles = async (roomId: string): Promise<boolean[]> => {
  const response = await axios.get(GET_AVAILABLE_ROLES_URL + roomId);
  return response.data;
};

const resetGame = async (userId: string): Promise<ActionResult> => {
  const response = await axios.post(RESET_GAME_URL, {
    userId: userId,
  });
  return response.data;
};

const makeMove = async (
  userId: string,
  row: number,
  col: number
): Promise<ActionResult> => {
  const response = await axios.post(MAKE_MOVE_URL, {
    user_id: userId,
    row,
    col,
  });
  return response.data;
};

const swap = async (userId: string, swap: boolean): Promise<ActionResult> => {
  const response = await axios.post(SWAP_URL, { user_id: userId, swap: swap });
  return response.data;
};

const getSuggestion = async (
  userId: string
): Promise<{
  success: boolean;
  message: string;
  moveEvaluation: MoveEvaluation;
}> => {
  const response = await axios.get(GET_SUGGESTION_URL + `?user_id=${userId}`);
  return response.data;
};

const reverseMove = async (userId: string): Promise<ActionResult> => {
  const response = await axios.post(REVERSE_MOVE_URL, {
    user_id: userId,
  });
  return response.data;
};

const reapplyMove = async (userId: string): Promise<ActionResult> => {
  const response = await axios.post(REAPPLY_MOVE_URL, {
    user_id: userId,
  });
  return response.data;
};

const aiTurn = async (userId: string): Promise<ActionResult> => {
  const response = await axios.post(AI_TURN_URL, {
    user_id: userId,
  });
  return response.data;
};

const api = {
  createGame,
  createOnlineGame,
  getAvailableRoles,
  resetGame,
  makeMove,
  swap,
  getSuggestion,
  reverseMove,
  reapplyMove,
  aiTurn,
};

export default api;
