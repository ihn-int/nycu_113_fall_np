# Assignment 5

> https://people.cs.nycu.edu.tw/~lhyen/np/ass5/np24f_ass5az.html

> 作業 5 和 6 因為較複雜，且期末時間緊湊，改為回家作業，以後測評分且不以繳交時間扣分。

## 作業概述（引用）

## 作業重點

- 使用 fork() 函式建立 child process 時，要考慮到四名使用者的情況。
    - 這會建立兩個 child ，需要測試記憶體空間是否有正確被區隔開。
    - 或是全部自己建立和複製。
- 要仔細處理好 user 加入和離開的情況，使用 `shutdown()` 函式送出FIN 位元。
- 我一直會遇到一個問題： `readline()` 函式加上 `sprintf()` 函式會讓送出去的資料在某個階段莫名加上第二個換行。導致 user 會收到一個空行的訊息，後來使用 readline 的回傳值（讀取的位元數）區分是否收到空行。