# Assignment 2

> https://www.cs.nycu.edu.tw/~lhyen/np/ass2/np24f_ass2p.html

## 作業概述（引用）

如同作業一的client程式，但port改為SERV_PORT+1。

此 client 與 server 的連結建立後，client程式先送出你的學號及client的IP位址(以空格隔開)給server。
Server 收到後，會設定一個大於等於3的亂數，令為 n。然後 server 會讀取 n 列由 client 端使用者輸入的文字(每列必須超過10字元)。請注意 server 並不會將此 n 值告訴 client 端。

如果 server讀取client送來的某列文字時，發現字元數目小於等於10，或基於某種神祕的原因無法接受所收到的文字訊息，server會對 client 送出 "bad" (小寫)。此時剛剛送出的那列文字作廢，不計入成功送至server的文字列數。

client收到server送來的 "bad" 訊息後，仍可以繼續輸入下一列文字給 server，但須注意方才送出的那列文字不列入正確列數計算。

當讀畢 n 列的資料後，server 會對 client 送出 "stop" (小寫)。client 程式收到後要立刻回傳已傳送至 server 端的正確文字列數及client的IP位址(以空格隔開)。如果此數目與 server 端的 n 值吻合，且IP位址與開頭傳送的一致，server 會回傳 "ok" 並結束。如果有任一不正確或時間過長，server 會回傳 "nak" 並結束(判定失敗)。

一個正確的執行流程範例如下圖所示。



假設我們先將Assignment 1的 client 程式 ass1cli24.c 複製成為另一個 ass2cli24.c。接下來 你可以用 Unix 下的 vi 或 GNOME 桌面下的 gedit 來編輯此程式。 事實上你要做的事有六件：
1. 改寫 main 中程式碼，使連結建立的 port 為 SERV_PORT+1。
2. 以程式碼填入資料方式送出你的學號和IP位址。程式需使用 getsockname (第四章) 及 inet_ntoa (第三章) 得到client的IP位址並以字串方式送出。
3. 參考 select/strcliselect01.c 或 select/strcliselect02.c 改寫 ass2cli24.c 中 的 xchg_data 函數 (假設你是依照Assignment 1的建議命名)， 使無論 stdin 或 socket 有輸入都可以處理，不必依照一定的順序。這樣你才可以 讓使用者輸入任意列數的資料(每列超過10字元)，而且可以正確處理 server 傳來的 "bad", "stop", "ok", "nak" 訊息。
4. 於收到 "bad" 訊息時，校正正確文字列數。
5. 於收到 "stop" 訊息時，以程式碼填入資料方式送出正確文字列數和IP位址。
6. 確定收到 "ok" 訊息以確定程式最終正確性。

修改完程式後，請參考此網頁來編譯你的程式。

## 作業重點

- `getsockname()` 需要先配置空間，需要呼叫 `socklen = sizeof(ss);`
- `getsockname()` 接收的第二個參數是 `sockaddr` 結構，而非 `sockaddr_in` ，需要像先前投影片一樣在傳遞 pointer 時進行 casting 。
- `inet_ntoa()` 接收的參數的應該是 `sockaddr_in` 的 `sin_addr` 欄位，不是一個整數。
- 接收到 stop 訊息的時候需要回傳資料，這部分需要在 socket 的區塊內完成，因為這個過程不會涉及到使用者的輸入。