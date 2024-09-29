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
