import { SerialPort } from "serialport";
import { ReadlineParser } from "@serialport/parser-readline";
import { processData } from "./process";

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