# Assignment 4

> https://people.cs.nycu.edu.tw/~lhyen/np/ass4/np24f_ass4tn.html

> 本次上機難度較高，時間限制較鬆散。如同作業 3 提供測試用和 final server 。少數問題是，如果使用 wsl 實作， wsl 的網路介面設計會讓 server 取得 IP 時出現錯誤。

> 因為 IP 或 OS 問題無法連線 server 測試，會根據 E3 上的程式碼和繳交時間進行評分。大家都賭它會過。

## 作業概述（引用）

## 作業重點

可以修改範例程式，此次上機和先前內容較沒有關係。

- 程式主要分為兩個部分：
    - 使用 UDP client 向老師程式的 UDP server 通訊。可以理解為使用 UDP 進行簡單的 handshake 。
    - 使用 TCP server 服務 TCP client ，服務數量由 UDP 的 handshake 得知。
- 為了取得自己的 IP ，UDP socket 應該要使用 Connect 。
- 使用 `Write()` 函式應該要將寫入資料量透過 `strlen()` 函式決定，因為儘管 TCP client 使用 `Readline()` 函式讀取，這個讀取很可能是連續的，因此會重複讀取到 buffer 後面無關的資料。
- 進入到 TCP 部分後， server parent 需要做的僅有使用 `waitpid()` 消除 zombie ，和所有連線建立完成後等待 UDP server 的回應。
- 服務固定數量 client 應該要使用 for 迴圈，不過需要注意 signal 導致迴圈被跳過。