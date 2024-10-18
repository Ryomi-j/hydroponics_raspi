import express from 'express';
import bodyParser from 'body-parser';
import settingsRouter from './routes/settings';
import { initializeSerialCommunication } from './serial';

const app = express();
app.use(bodyParser.json());

// 라우트 설정
app.use('/set-values', settingsRouter);

// 시리얼 통신 초기화
initializeSerialCommunication();

export default app;
