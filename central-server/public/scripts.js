document.addEventListener('DOMContentLoaded', function() {
    loadDevices();
  
    // 初始化 Socket.IO
    const socket = io();
  
    // 監聽來自 ESP32 的訊息
    socket.on('esp32Message', function(data) {
      // 顯示訊息，例如在頁面上新增一個訊息區域
      const messageArea = document.getElementById('messageArea');
      if (messageArea) {
        messageArea.innerHTML += `<p><strong>${data.deviceName}:</strong> ${data.message}</p>`;
      }
    });
  
    // 新增裝置表單提交事件
    document.getElementById('addDeviceForm').addEventListener('submit', function(e) {
      e.preventDefault();
      const name = document.getElementById('name').value;
      const ip = document.getElementById('ip').value;
  
      fetch('/api/devices', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ name, ip })
      })
      .then(response => response.json())
      .then(data => {
        alert(data.message);
        loadDevices();
        document.getElementById('addDeviceForm').reset();
      })
      .catch(error => {
        console.error('Error:', error);
        alert('新增裝置失敗');
      });
    });
  
    // 發送 IR 指令表單提交事件
    document.getElementById('sendForm').addEventListener('submit', function(e) {
      e.preventDefault();
      const device = document.getElementById('device').value;
      const code = document.getElementById('code').value;
  
      fetch(`/api/devices/${device}/send`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ code: parseInt(code) })
      })
      .then(response => response.json())
      .then(data => {
        alert(data.message);
      })
      .catch(error => {
        console.error('Error:', error);
        alert('發送指令失敗');
      });
    });
  });
  
  // 加載 ESP32 裝置列表並更新前端介面
  function loadDevices() {
    fetch('/api/devices')
      .then(response => response.json())
      .then(devices => {
        const deviceTable = document.getElementById('deviceTable');
        const deviceSelect = document.getElementById('device');
        deviceTable.innerHTML = '';
        deviceSelect.innerHTML = '';
  
        devices.forEach(device => {
          // 更新裝置列表表格
          const row = document.createElement('tr');
          row.innerHTML = `
            <td>${device.name}</td>
            <td>${device.ip}</td>
            <td>
              <button class="delete" onclick="deleteDevice('${device.name}')">刪除</button>
            </td>
          `;
          deviceTable.appendChild(row);
  
          // 更新發送指令選單
          const option = document.createElement('option');
          option.value = device.name;
          option.textContent = device.name;
          deviceSelect.appendChild(option);
        });
      })
      .catch(error => {
        console.error('Error:', error);
        alert('載入裝置失敗');
      });
  }
  
  // 刪除裝置函數
  function deleteDevice(name) {
    if (confirm(`確定要刪除裝置 "${name}" 嗎？`)) {
      fetch(`/api/devices/${name}`, {
        method: 'DELETE'
      })
      .then(response => response.json())
      .then(data => {
        alert(data.message);
        loadDevices();
      })
      .catch(error => {
        console.error('Error:', error);
        alert('刪除裝置失敗');
      });
    }
  }
  