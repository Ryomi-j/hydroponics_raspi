import express from 'express';
import { saveSettings } from '../firebase/firestore';
import { sendSettingsToArduino } from '../serial/communication';

const router = express.Router();

router.post('/', async (req, res) => {
  const { temperature, ph, conductivity, nutrientTemperature, pumpOnDuration, pumpOffDuration } = req.body;

  // Firestore에 설정값 저장 (워터 펌프 주기 포함)
  await saveSettings({ temperature, ph, conductivity, nutrientTemperature, pumpOnDuration, pumpOffDuration });

  // 아두이노로 설정값 전송 (워터 펌프 주기 포함)
  sendSettingsToArduino({ temperature, ph, conductivity, nutrientTemperature, pumpOnDuration, pumpOffDuration });

  res.send('Settings updated');
});

export default router;
