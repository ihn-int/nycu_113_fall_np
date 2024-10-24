# Assignment 2

> https://www.cs.nycu.edu.tw/~lhyen/np/ass2/np24f_ass2p.html

## 作業重點

- `getsockname()` 需要先配置空間，需要呼叫 `socklen = sizeof(ss);`
- `getsockname()` 接收的第二個參數是 `sockaddr` 結構，而非 `sockaddr_in` ，需要像先前投影片一樣在傳遞 pointer 時進行 casting 。
- `inet_ntoa()` 接收的參數的應該是 `sockaddr_in` 的 `sin_addr` 欄位，不是一個整數。
- 接收到 stop 訊息的時候需要回傳資料，這部分需要在 socket 的區塊內完成，因為這個過程不會涉及到使用者的輸入。