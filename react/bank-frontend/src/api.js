import axios from 'axios';

const API_BASE = 'http://localhost:8080/api';

const api = axios.create({
    baseURL: API_BASE,
    headers: { 'Content-Type': 'application/json' }
});

export const getAccounts = () => api.get('/accounts');
export const createAccount = (data) => api.post('/accounts', data);
export const getAccountByUsername = (username) => api.get(`/accounts/${username}`);
export const deposit = (data) => api.post('/deposit', data);
export const withdraw = (data) => api.post('/withdraw', data);
export const transfer = (data) => api.post('/transfer', data);
export const deleteAccount = (data) => api.delete('/accounts', { data });