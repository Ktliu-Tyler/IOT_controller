const express = require('express');
const axios = require('axios');
const path = require('path');
const http = require('http'); // 新增
const socketIo = require('socket.io'); // 新增
const app = express();
const port = 3000;

// 中央伺服器管理的 ESP32 裝置列表
let esp32Devices = [
  { name: 'ESP32_1', ip: '192.168.1.101' },
  { name: 'ESP32_2', ip: '192.168.1.102' },
  { name: 'ESP32_3', ip: '192.168.1.103' }
];

// 中央伺服器使用 JSON 格式解析請求體
app.use(express.json());
app.use(express.urlencoded({ extended: true }));

// 提供靜態檔案（前端）
app.use(express.static(path.join(__dirname, 'public')));

// 創建 HTTP 伺服器
const server = http.createServer(app);

// 創建 Socket.IO 伺服器
const io = socketIo(server);

// 當有新的客戶端連接時
io.on('connection', (socket) => {
  console.log('新客戶端已連接');
  
  // 可以在這裡處理來自 Web 介面的事件
  socket.on('disconnect', () => {
    console.log('客戶端已斷開連接');
  });
});

// API: 獲取所有 ESP32 裝置
app.get('/api/devices', (req, res) => {
  res.json(esp32Devices);
});

// API: 發送 IR 指令給特定 ESP32 裝置
app.post('/api/devices/:name/send', async (req, res) => {
  const deviceName = req.params.name;
  const { code } = req.body;

  const device = esp32Devices.find(d => d.name === deviceName);
  if (!device) {
    return res.status(404).json({ message: '設備未找到' });
  }

  try {
    const response = await axios.post(`http://${device.ip}/send`, { code });
    res.json({ message: '指令已發送', data: response.data });
  } catch (error) {
    res.status(500).json({ message: '發送失敗', error: error.message });
  }
});

// API: 新增 ESP32 裝置
app.post('/api/devices', (req, res) => {
  const { name, ip } = req.body;
  if (!name || !ip) {
    return res.status(400).json({ message: '缺少名稱或 IP 地址' });
  }
  esp32Devices.push({ name, ip });
  res.json({ message: '設備已新增', devices: esp32Devices });
});

// API: 刪除 ESP32 裝置
app.delete('/api/devices/:name', (req, res) => {
  const deviceName = req.params.name;
  esp32Devices = esp32Devices.filter(d => d.name !== deviceName);
  res.json({ message: '設備已刪除', devices: esp32Devices });
});

// API: 接收來自 ESP32 的訊息
app.post('/api/esp32/message', (req, res) => {
  const { deviceName, message } = req.body;

  // 將訊息透過 Socket.IO 推送給所有連接的客戶端
  io.emit('esp32Message', { deviceName, message });

  res.json({ message: '訊息已接收並推送' });
});

// 啟動伺服器
server.listen(port, () => {
  console.log(`中央伺服器運行在 http://localhost:${port}`);
});
