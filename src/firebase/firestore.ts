import { db } from ".";
import admin from "firebase-admin";

// Firestore에 센서 데이터 저장
export const saveSensorDataToDatabase = async (
  deviceId: string,
  averagedData: any
) => {
  await db.collection("sensorData").add({
    deviceId,
    ...averagedData,
    timestamp: admin.firestore.FieldValue.serverTimestamp(),
  });
};
