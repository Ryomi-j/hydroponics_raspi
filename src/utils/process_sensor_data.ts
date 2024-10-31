import fs from "fs";
import { saveSensorDataToDatabase } from "../firebase/firestore";

// 데이터 파일이 저장된 경로
const dataDir = "/home/eunjeong/Desktop/hydroponics-rasp/data/";

// 파일을 읽고 평균을 계산하여 DB에 저장하는 함수
const processFiles = () => {
  fs.readdirSync(dataDir).forEach((file) => {
    const filePath = `${dataDir}/${file}`;
    const sensorData = fs.readFileSync(filePath, "utf-8").trim().split("\n");

    if (sensorData.length > 0) {
      // 데이터 파싱 및 min/max 제외한 평균 계산
      const parsedData = sensorData.map(parseSensorLine);
      const averagedData = calculateAverages(parsedData);

      // DB에 저장
      const deviceId = file.replace(".txt", "");
      saveSensorDataToDatabase(deviceId, averagedData);

      // 파일 초기화
      fs.writeFileSync(filePath, "");
    }
  });
};

// 각 센서값에서 min/max 제외하고 평균 계산
const calculateAverages = (dataArray: any[]) => {
  const calculateWithoutMinMax = (values: number[]) => {
    const sortedValues = values.sort((a, b) => a - b);
    const withoutMinMax = sortedValues.slice(1, -1); // min/max 제거
    const average =
      withoutMinMax.reduce((sum, val) => sum + val, 0) / withoutMinMax.length;
    return average;
  };

  return {
    waterLevel: calculateWithoutMinMax(dataArray.map((d) => d.waterLevel)),
    phValue: calculateWithoutMinMax(dataArray.map((d) => d.phValue)),
    outerTemp: calculateWithoutMinMax(dataArray.map((d) => d.outerTemp)),
    outerHumidity: calculateWithoutMinMax(
      dataArray.map((d) => d.outerHumidity)
    ),
    nutrientTemp: calculateWithoutMinMax(dataArray.map((d) => d.nutrientTemp)),
    conductivity: calculateWithoutMinMax(dataArray.map((d) => d.conductivity)),
  };
};

// 센서 데이터의 각 라인을 파싱하는 함수
const parseSensorLine = (line: string) => {
  const [
    waterLevel,
    phValue,
    outerTemp,
    outerHumidity,
    nutrientTemp,
    conductivity,
  ] = line.split(",").map(Number);
  return {
    waterLevel,
    phValue,
    outerTemp,
    outerHumidity,
    nutrientTemp,
    conductivity,
  };
};

// 5분마다 센서 파일들을 처리
processFiles();
