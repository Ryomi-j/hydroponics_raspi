# 수경재배 모니터링 시스템(Raspberry Pi)

이 프로젝트는 아두이노 기반 수경재배 시스템의 센서 데이터를 수집하고 관리하는 라즈베리파이용 애플리케이션입니다.

## 주요 기능

### 1. 다중 디바이스 지원

- 하나의 라즈베리파이에 여러 아두이노 장치 연결 가능
- 각 장치별로 독립적인 데이터 수집 및 관리
- 지원되는 시리얼 포트: `/dev/ttyUSB0`, `/dev/ttyUSB1`, `/dev/ttyACM0`

### 2. 센서 데이터 수집

다음과 같은 센서 데이터를 수집합니다:

- 수위 (Water Level) : 비접촉식 수위 센서
- pH 값
- 외부 온도
- 외부 습도
- 양액 온도
- 전도도

### 3. 데이터 처리

- 5분마다 자동으로 데이터 수집 및 처리
- 최소값과 최대값을 제외한 평균값 계산
- Firebase Firestore에 데이터 저장

### 4. 원격 제어

REST API를 통해 다음 설정값을 제어할 수 있습니다:

- 온도 설정
- pH 설정
- 전도도 설정
- 양액 온도 설정
- 펌프 작동 시간
- 펌프 정지 시간

## 설치 방법

1. 필요한 패키지 설치:

```bash
npm install
```

2. 환경 변수 설정:
   `.env` 파일을 생성하고 다음 값들을 설정하세요:

```
FIREBASE_PROJECT_ID=your_project_id
FIREBASE_PRIVATE_KEY=your_private_key
FIREBASE_CLIENT_EMAIL=your_client_email
```

3. Crontab 설정:

```bash
*/5 * * * * /usr/bin/node /path/to/process_sensor_data.ts
```

## API 사용법

### 설정값 변경

```bash
POST /set-values
Content-Type: application/json

{
  "deviceId": "device1",
  "temperature": 25,
  "ph": 6.5,
  "conductivity": 1000,
  "nutrientTemperature": 20,
  "pumpOnDuration": 300,
  "pumpOffDuration": 900
}
```

## 데이터 형식

### 아두이노에서 전송되는 데이터 형식

```
DEVICE_ID:{id},WATER_LEVEL:{value},PH:{value},OUTER_TEMP:{value},OUTER_HUMIDITY:{value},NUTRIENT_TEMP:{value},CONDUCTIVITY:{value}
```

### Firebase에 저장되는 데이터 구조

```javascript
{
  deviceId: string,
  waterLevel: number,
  phValue: number,
  outerTemp: number,
  outerHumidity: number,
  nutrientTemp: number,
  conductivity: number,
  timestamp: Timestamp
}
```

## 자동 실행 설정

### 1. Bash 스크립트 설정

1. 스크립트 파일 생성:

```bash
nano /path/to/folder/start_script.sh
```

2. 스크립트 내용 작성:

```bash
#!/bin/bash
cd /path/to/folder/hydroponics-rasp
npm run build  # TypeScript를 JavaScript로 컴파일
node /path/to/folder/hydroponics-rasp/lib/index.js  # 변환된 JavaScript 파일 실행
```

3. 실행 권한 부여:

```bash
chmod +x /path/to/folder/hydroponics/start_script.sh
```

### 2. 부팅 시 자동 실행 설정

1. crontab 편집:

```bash
crontab -e
```

2. 다음 내용 추가:

```bash
@reboot /path/to/folder/hydroponics/start_script.sh
```

### 3. 실행 상태 확인

1. 시스템 재부팅:

```bash
sudo reboot
```

2. 프로세스 실행 확인:

```bash
ps aux | grep node
```

## 주의사항

1. 시리얼 포트 권한 설정이 필요합니다:

```bash
sudo usermod -a -G dialout $USER
```

2. 데이터 저장 경로가 존재하는지 확인하세요:

```bash
mkdir -p /path/to/folder/hydroponics-rasp/data/
```
3. Firebase 인증 정보가 올바르게 설정되어 있는지 확인하세요.
4. 스크립트 경로가 올바른지 확인하세요
5. TypeScript 컴파일 설정(`tsconfig.json`)이 올바른지 확인하세요
6. 필요한 npm 패키지들이 모두 설치되어 있는지 확인하세요

## 문제 해결

- 시리얼 통신 오류 발생 시 포트 연결 상태와 권한을 확인하세요
- 데이터가 저장되지 않을 경우 Firebase 인증 정보를 확인하세요
- 센서 데이터 형식이 올바른지 확인하세요

## 라이선스

본 프로젝트는 MIT 라이선스를 따릅니다:

```
MIT License

Copyright (c) 2024 Kim Eunjeong

이 소프트웨어의 복제본과 관련된 문서화 파일("소프트웨어")을 획득하는 사람은 
누구라도 소프트웨어를 별다른 제한 없이 무상으로 사용할 수 있는 권한을 
부여받습니다. 여기에는 소프트웨어의 복제본을 무제한으로 사용, 복제, 수정, 
병합, 공표, 배포, 서브라이선스 설정 및 판매할 수 있는 권리와 이상의 행위를 
소프트웨어를 제공받은 다른 수취인들에게 허용할 수 있는 권리가 포함되며, 
다음과 같은 조건을 충족시키는 것을 전제로 합니다.

위와 같은 저작권 안내 문구와 본 허용 문구가 소프트웨어의 모든 복제본 및 
중요 부분에 포함되어야 합니다.

이 소프트웨어는 상품성, 특정 목적 적합성, 그리고 비침해에 대한 보증을 
포함한 어떠한 형태의 보증도 명시적이나 묵시적으로 설정되지 않은 "있는 
그대로의" 상태로 제공됩니다. 소프트웨어를 개발한 프로그래머나 저작권자는 
어떠한 경우에도 소프트웨어나 소프트웨어의 사용 등의 행위와 관련하여 
일어나는 어떤 요구사항이나 손해 및 기타 책임에 대해 계약상, 불법행위 또는 
기타 이유로 인한 책임을 지지 않습니다.
```