import express from 'express';
import { saveSettings } from '../firebase/firestore';
import { sendSettingsToArduino } from '../serial/communication';

const router = express.Router();

router.post('/', async (req, res) => {
  const { temperature, ph, conductivity, ledStart, ledEnd, pumpOnDuration, pumpOffDuration, pumpActivated } = req.body;

  // Firestore에 설정값 저장
  await saveSettings({ temperature, ph, conductivity, ledStart, ledEnd, pumpOnDuration, pumpOffDuration, pumpActivated });

  // 아두이노로 설정값 전송
  sendSettingsToArduino({ temperature, ph, conductivity, ledStart, ledEnd, pumpOnDuration, pumpOffDuration, pumpActivated });

  res.send('Settings updated');
});

export default router;
