# Assignment 1

> https://www.cs.nycu.edu.tw/~lhyen/np/ass1/np24f_ass1.html

## 作業概述（引用）

此次作業要求你寫一個client程式，與一個已經存在的server程式通訊。

你可以改寫第五章的 Echo client (tcpcliserv/tcpcli01.c) 程式， 使此 client 與 server 的連結建立後(port=SERV_PORT)，先送出你的學號。Server 收到後會送出兩個 正整數，你的 client 收到後請回傳這兩個數字的gcd，並等待 server 的回應。如果 server 回傳 "ok\n"，表示上傳成功且結果正確，client 可關閉連線。 如果 server 回傳 "nak\n"，表示上傳不成功，client 須接收另一組數字，並 重覆上述計算與上傳過程。如果 server 回傳 "wrong\n"，表示上傳成功但結果不正確， 且 server 不再接受此 client 的資料。一個可能的執行流程範例如下圖所示。

1. 進入工作目錄 unpv13e 下面的 tcpcliserv 目錄。
2. 建議先將tcpcliserv/tcpcli01.c檔copy成另一個複本(如ass1cli24.c)。接下來 你可以用 Unix 下的 vi 或 GNOME 桌面下的 gedit 來編輯此程式。 事實上你要做的是改寫該程式中main呼叫的 str_cli 函數 (位於 lib/str_cli.c)。你可以先將此main呼叫的函數改名 (如xchg_data)，然後將此函數的定義寫在同一個程式檔案(即ass1cli24.c)中。
3. Client要送出學號資料給Server，需使用writen(或作者提供的包裝版Writen)函數。用法請參考 lib/str_cli.c檔。基本上第二個參數(字元指標)指向你要送出的資料，第三個參數說明要送出多少字元("\n"也算一個字元)。
4. 提醒各位：此server程式呼叫Readline函數接收client送過去的資料，所以記得client端必須在要送出去資料的最後加上換行字元， server才讀得到。Readline函數的原始程式碼在 test 目錄下的 readline1.c 中。
5. 此外，雖然server的 "nak\n" 與接下來兩組數字是分開送出的，但請記住 TCP 是 stream-based 的，client 可能一次就將 "nak\n" 與接下來兩組數字一次讀入。選用正確的讀取函數可以讓 "\n" 成為區隔這兩筆資料的分隔符號。
6. 計算gcd的函數程式碼如下
// Function to implement the Euclidean algorithm
int gcd(int a, int b) {
    // Base case: if b is 0, gcd is a
    if (b == 0)
        return a;
    
    // Recursively call gcd with b and a % b
    return gcd(b, a % b);
}
7. 修改完程式後，請參考此網頁來編譯你的程式。

## 作業重點

- `Readline()` 函式本來就會從 buffer 讀取到換行字元，並且回傳內容。
- 伺服器端也是使用 `Readline()`函式讀取資料，因此需要在字串末端加上換行字元。
- `sscanf()` 和 `sprintf()` 的使用會更簡潔。

## 備註

後來我在 demo 的過程才知道應該要把學號包裝在程式裡自動發送的。