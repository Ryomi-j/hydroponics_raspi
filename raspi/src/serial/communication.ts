import fs from "fs";
import { SerialPort } from "serialport";
import { ReadlineParser } from "@serialport/parser-readline";
import { validateSensorData } from "../sensors/validation";

const portPaths = ["/dev/ttyUSB0", "/dev/ttyUSB1", "/dev/ttyACM0"]; // 다중 포트 정의

// 각 시리얼 포트에 대한 초기화
portPaths.forEach((portPath) => {
  const port = new SerialPort({ path: portPath, baudRate: 115200 });
  const parser = port.pipe(new ReadlineParser({ delimiter: "\n" }));

  parser.on("data", (data: string) => {
    processData(data, portPath); // 각 포트에서 들어오는 데이터를 처리
  });

  port.on("open", () => {
    console.log(`Port ${portPath} opened`);
  });

  port.on("error", (err) => {
    console.error(`Error on port ${portPath}: `, err);
  });
});

// 데이터를 처리하는 함수
export const processData = (data: string, portPath: string) => {
  console.log(`Received data from ${portPath}: ${data}`);

  // 센서 데이터 유효성 검사
  if (!validateSensorData(data)) {
    console.log("Invalid data, skipping save.");
    return;
  }

  // 데이터 파싱 및 deviceId 추출
  const { deviceId, sensorData } = parseSensorData(data);

  // 데이터 파일에 저장
  saveSensorDataToFile(deviceId, sensorData);
};

// 데이터 파싱 및 deviceId 처리
const parseSensorData = (data: string) => {
  const [
    deviceId,
    waterLevel,
    phValue,
    outerTemp,
    outerHumidity,
    nutrientTemp,
    conductivity,
  ] = data
    .replace("DEVICE_ID:", "")
    .replace("WATER_LEVEL:", "")
    .replace("PH:", "")
    .replace("OUTER_TEMP:", "")
    .replace("OUTER_HUMIDITY:", "")
    .replace("NUTRIENT_TEMP:", "")
    .replace("CONDUCTIVITY:", "")
    .split(",")
    .map((value) => value.trim());

  return {
    deviceId,
    sensorData: {
      waterLevel: Number(waterLevel),
      phValue: Number(phValue),
      outerTemp: Number(outerTemp),
      outerHumidity: Number(outerHumidity),
      nutrientTemp: Number(nutrientTemp),
      conductivity: Number(conductivity),
    },
  };
};

// deviceId별로 파일 저장
const saveSensorDataToFile = (deviceId: string, sensorData: any) => {
  const filePath = `/home/eunjeong/Desktop/hydroponics-rasp/data/${deviceId}.txt`;
  const dataString = `${sensorData.waterLevel},${sensorData.phValue},${sensorData.outerTemp},${sensorData.outerHumidity},${sensorData.nutrientTemp},${sensorData.conductivity}\n`;

  // 파일에 데이터 추가
  fs.appendFileSync(filePath, dataString, "utf8");
};

// 설정값을 아두이노로 전송하는 함수
export const sendSettingsToArduino = (
  deviceId: string,
  settings: {
    temperature: number;
    ph: number;
    conductivity: number;
    nutrientTemperature: number;
    pumpOnDuration: number;
    pumpOffDuration: number;
  }
) => {
  const {
    temperature,
    ph,
    conductivity,
    nutrientTemperature,
    pumpOnDuration,
    pumpOffDuration,
  } = settings;

  const command = `SET:DEVICE_ID:${deviceId},TEMP:${temperature},PH:${ph},COND:${conductivity},NUT_TEMP:${nutrientTemperature},PUMP_ON:${pumpOnDuration},PUMP_OFF:${pumpOffDuration}\n`;

  // 시리얼 포트에서 deviceId에 해당하는 포트 찾기
  const port = findPortForDevice(deviceId); // deviceId에 맞는 포트 식별 함수 추가

  if (!port) {
    return console.log(`No port found for device: ${deviceId}`);
  }

  // 시리얼 포트를 통해 아두이노로 설정값 전송
  port.write(command, (err) => {
    if (err) {
      return console.log(`Error on write to device ${deviceId}: `, err.message);
    }
    console.log(`Settings sent to Arduino (${deviceId}):`, command);
  });
};

// deviceId에 맞는 시리얼 포트 식별 함수
const findPortForDevice = (deviceId: string): SerialPort | undefined => {
  // 미리 연결된 포트 정보와 deviceId 매핑을 통해 포트 식별
  const devicePortMap: { [key: string]: SerialPort } = {
    device1: new SerialPort({ path: "/dev/ttyUSB0", baudRate: 115200 }),
    device2: new SerialPort({ path: "/dev/ttyUSB1", baudRate: 115200 }),
    // 추가적인 포트 연결을 이곳에 정의
  };

  return devicePortMap[deviceId];
};