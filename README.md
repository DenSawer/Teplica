
# 🌿 Teplica — Умная теплица

Полноценная IoT-система для мониторинга и управления теплицей.  
Проект развёрнут на: **[teplica.denserver.ru](https://teplica.denserver.ru)**

---

## 📦 Структура проекта

```
/
├── firmware/              # Код для ESP32
│   ├── Teplica/           # Основной код прошивки
│   ├── libraries/         # Сторонние библиотеки
│   └── sd/                # Работа с SD-картой
├── server/
│   ├── backend/           # C++ сервер (Crow)
│   └── web-interface/     # Веб-интерфейс: HTML, JS, CSS
├── LICENSE                # MIT лицензия
└── README.md              # Этот файл
```

---

## ⚙️ Как работает система

1. **ESP32** считывает данные с датчиков (температура, влажность, CO₂ и др.)
2. Отправляет их в формате JSON на API:
   ```
   POST https://teplica.denserver.ru/api/upload/{device_id}
   Content-Type: application/json
   ```
3. **C++ сервер** (на Crow) принимает данные, сохраняет в `data_*.json`
4. **Веб-интерфейс** отображает эти данные в виде графиков

---

## 🔧 Backend

Находится в `server/backend/`

- Написан на C++ с использованием Crow (легковесный REST-фреймворк)
- Обработка POST-запросов от ESP и сохранение в JSON
- Поддержка HTTPS через nginx (конфиг можно добавить отдельно)

### ▶️ Сборка

```bash
cd server/backend
mkdir build && cd build
cmake ..
make
./greenhouse-server
```

---

## 🌐 Веб-интерфейс

Находится в `server/web-interface/`

- `index.html` + `chart.min.js` + `script.js`
- Подключается к `data.json`, отображает текущие и архивные данные

---

## 📡 Firmware (ESP32)

Расположена в `firmware/Teplica/`

- Написана под Arduino/PlatformIO
- Использует Wi-Fi, датчики CO₂ (SCD30, SCD4x) и SD-карту
- Настройки Wi-Fi, ID устройства и URL API задаются в `config.h`

---

## 🌍 Демо

Сайт: **https://teplica.denserver.ru**  
API: `https://teplica.denserver.ru/api/upload/{device_id}`

---

## 🛡️ Лицензия

Проект распространяется под лицензией **MIT** — см. файл [`LICENSE`](LICENSE)

---

## 👤 Автор

**DenSawer**  
GitHub: [github.com/DenSawer/Teplica](https://github.com/DenSawer/Teplica)

---
