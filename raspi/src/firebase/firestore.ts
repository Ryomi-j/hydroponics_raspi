import { db } from ".";
import admin from "firebase-admin";

// 센서 데이터를 Firestore에 저장
export const saveSensorData = async (sensorData: any) => {
  await db.collection("sensorData").add({
    ...sensorData,
    timestamp: admin.firestore.FieldValue.serverTimestamp(),
  });
};

// 설정값을 Firestore에 저장
export const saveSettings = async (settings: any) => {
  await db
    .collection("settings")
    .doc("currentSettings")
    .set({
      ...settings,
      updatedAt: admin.firestore.FieldValue.serverTimestamp(),
    });
};
