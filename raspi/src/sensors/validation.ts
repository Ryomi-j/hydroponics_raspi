export const validateSensorData = (data: string): boolean => {
  // 필수 필드가 모두 포함되어 있는지 확인
  if (
    !data.includes("WATER_LEVEL:") ||
    !data.includes("PH:") ||
    !data.includes("OUTER_TEMP:")
  ) {
    return false;
  }

  // NaN 여부 확인
  const values = data
    .replace("WATER_LEVEL:", "")
    .replace("PH:", "")
    .replace("OUTER_TEMP:", "")
    .replace("OUTER_HUMIDITY:", "")
    .replace("NUTRIENT_TEMP:", "")
    .replace("CONDUCTIVITY:", "")
    .split(",")
    .map(Number);

  return values.every((value) => !isNaN(value));
};
