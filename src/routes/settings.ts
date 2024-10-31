import express from "express";
import { sendSettingsToArduino } from "../serial/communication";

const router = express.Router();

router.post("/", (req, res) => {
  const {
    deviceId,
    temperature,
    ph,
    conductivity,
    nutrientTemperature,
    pumpOnDuration,
    pumpOffDuration,
  } = req.body;

  if (!deviceId) {
    return res.status(400).send("Device ID is required");
  }

  // deviceId와 함께 설정값 전송
  sendSettingsToArduino(deviceId, {
    temperature,
    ph,
    conductivity,
    nutrientTemperature,
    pumpOnDuration,
    pumpOffDuration,
  });

  return res.send("Settings updated");
});

export default router;
