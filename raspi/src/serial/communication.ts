import { SerialPort } from "serialport";
import { ReadlineParser } from "@serialport/parser-readline";
import { saveSensorData } from "../firebase/firestore";
import { validateSensorData } from "../sensors/validation";

// 시리얼 포트 초기화
const port = new SerialPort({ path: "/dev/ttyUSB0", baudRate: 9600 });
const parser = port.pipe(new ReadlineParser({ delimiter: "\n" })); // 줄 단위로 데이터 파싱

// 시리얼 포트에서 수신한 데이터 처리
parser.on("data", (data: string) => {
  processData(data);
});

export const processData = (data: string) => {
  console.log(`Received data: ${data}`);

  // 센서 데이터 유효성 검사
  if (!validateSensorData(data)) {
    console.log("유효하지 않은 데이터. 저장을 건너뜁니다.");
    return;
  }

  // 데이터 파싱
  const sensorData = parseSensorData(data);

  // Firestore에 데이터 저장
  saveSensorData(sensorData);
};

const parseSensorData = (data: string) => {
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

  return {
    waterLevel,
    phValue,
    outerTemp,
    outerHumidity,
    nutrientTemp,
    conductivity,
  };
};

// 설정값을 아두이노로 전송하는 함수
export const sendSettingsToArduino = (settings: {
  temperature: number;
  ph: number;
  conductivity: number;
  nutrientTemperature: number;
  pumpOnDuration: number;
  pumpOffDuration: number;
}) => {
  const {
    temperature,
    ph,
    conductivity,
    nutrientTemperature,
    pumpOnDuration,
    pumpOffDuration,
  } = settings;
  const command = `SET:TEMP:${temperature},PH:${ph},COND:${conductivity},NUT_TEMP:${nutrientTemperature},PUMP_ON:${pumpOnDuration},PUMP_OFF:${pumpOffDuration}\n`;

  // 시리얼 포트를 통해 아두이노로 전송
  port.write(command, (err) => {
    if (err) {
      return console.log("Error on write: ", err.message);
    }
    console.log("Settings sent to Arduino:", command);
  });
};
