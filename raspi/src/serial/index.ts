import { SerialPort } from "serialport";
import { ReadlineParser } from "@serialport/parser-readline";
import { processData } from "./communication";

export const initializeSerialCommunication = () => {
  const port = new SerialPort({ path: "/dev/ttyACM0", baudRate: 9600 });
  const parser = port.pipe(new ReadlineParser({ delimiter: "\n" }));

  parser.on("data", (data: string) => {
    processData(data);
  });
}
