import axios from "axios";
import { MoveResult } from "../interface";

const API_URL = "http://localhost:5000";
const CREATE_GAME_URL = `${API_URL}/create_game`;
const RESET_GAME_URL = `${API_URL}/reset_game`;
const MAKE_MOVE_URL = `${API_URL}/make_move`;
const GET_SUGGESTION_URL = `${API_URL}/get_suggestion`;
const REVERSE_MOVE_URL = `${API_URL}/reverse_move`;
const REAPPLY_MOVE_URL = `${API_URL}/reapply_move`;

const createGame = async (userId: string, mode: number, size: number) => {
  const response = await axios.post(CREATE_GAME_URL, {
    userId: userId,
    mode: mode,
    size: size,
  });
  return response.data;
};

const resetGame = async (userId: string) => {
  const response = await axios.post(RESET_GAME_URL, {
    userId: userId,
  });
  return response.data;
};

const makeMove = async (userId: string, row: number, col: number) => {
  const response = await axios.post(MAKE_MOVE_URL, {
    user_id: userId,
    row,
    col,
  });
  return response.data;
};

const getSuggestion = async (userId: string) => {
  const response = await axios.get(GET_SUGGESTION_URL + `?user_id=${userId}`);
  return response.data;
};

const reverseMove = async (userId: string, moveResult: MoveResult) => {
  const response = await axios.post(REVERSE_MOVE_URL, {
    user_id: userId,
    moveResult: moveResult,
  });
  return response.data;
};

const reapplyMove = async (userId: string, moveResult: MoveResult) => {
  const response = await axios.post(REAPPLY_MOVE_URL, {
    user_id: userId,
    moveResult: moveResult,
  });
  return response.data;
};

export default {
  createGame,
  resetGame,
  makeMove,
  getSuggestion,
  reverseMove,
  reapplyMove,
};
