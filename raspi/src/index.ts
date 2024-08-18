import express from "express";
import bodyParser from "body-parser";
import admin from "firebase-admin";
import { SerialPort } from "serialport";
import { ReadlineParser } from "@serialport/parser-readline";
import dotenv from "dotenv";

// 환경변수 로드
dotenv.config();

// Firebase 초기화
admin.initializeApp();

const db = admin.firestore();

const app = express();
app.use(bodyParser.json());

// 시리얼 포트 설정 (아두이노 연결)
const port = new SerialPort({ path: "/dev/ttyACM0", baudRate: 9600 });
const parser = port.pipe(new ReadlineParser({ delimiter: "\n" }));

// 아두이노에서 데이터 수신
parser.on("data", async (data: string) => {
  console.log(`Received data: ${data}`);
  const [
    waterLevel,
    phValue,
    outerTemp,
    outerHumidity,
    nutrientTemp,
    conductivity,
  ] = data
    .replace("WATER_LEVEL:", "")
    .replace("PH:", "")
    .replace("OUTER_TEMP:", "")
    .replace("OUTER_HUMIDITY:", "")
    .replace("NUTRIENT_TEMP:", "")
    .replace("CONDUCTIVITY:", "")
    .split(",")
    .map(Number);

  // Firebase에 5분 마다 센서 데이터 저장
  await db.collection("sensorData").add({
    waterLevel,
    phValue,
    outerTemp,
    outerHumidity,
    nutrientTemp,
    conductivity,
    timestamp: admin.firestore.FieldValue.serverTimestamp(),
  });
});

// 사용자 설정 API
app.post("/set-sensor-data", async (req, res) => {
  const { temperature, ph, conductivity, nutrientTemperature } = req.body;

  // Firebase에 설정값 저장
  await db.collection("settings").doc("currentSettings").set({
    temperature,
    ph,
    conductivity,
    nutrientTemperature,
    updatedAt: admin.firestore.FieldValue.serverTimestamp(),
  });

  // 아두이노로 설정값 전송
  port.write(
    `SET:TEMP:${temperature},PH:${ph},COND:${conductivity},NUT_TEMP:${nutrientTemperature}\n`
  );

  res.send("Settings updated");
});

app.listen(3000, () => {
  console.log("Server running on http://localhost:3000");
});
