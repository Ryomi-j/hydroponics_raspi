import { SerialPort } from "serialport";
import { ReadlineParser } from "@serialport/parser-readline";
import { processData } from "./process";

// 시리얼 포트 초기화 함수
export const initializeSerialCommunication = () => {
  const portPath = "/dev/ttyACM0"; // 포트 경로 설정
  const port = new SerialPort({ path: portPath, baudRate: 115200 });
  const parser = port.pipe(new ReadlineParser({ delimiter: "\n" }));

  parser.on("data", (data: string) => {
    processData(data, portPath); // 두 번째 인수로 portPath 전달
  });

  port.on("open", () => {
    console.log(`Port ${portPath} opened`);
  });

  port.on("error", (err) => {
    console.error(`Error on port ${portPath}: `, err);
  });
};
