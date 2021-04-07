# exam1
一開始先將slew rate = 1輸出在uLCD上，
然後使用無限迴圈不斷重複以下步驟
透過上下按鈕來選擇頻率，當下要被選到的slew rate會呈現綠色文字背景，
當按下select按鈕後，被選定的頻率會顯示紅色背景，表示目前產生的波對應到這個slew rate。
(slew rate有1, 1/2, 1/4, 1/8四種)

接著呼叫wave generator函式，
計算當前的slew rate對應到的波形，得到一週期的波形公式並輸出波形，並無限反覆輸出波形。

最後使用thread加上eventqueue來呼叫ADC_sampling這個函式來對波型做sampling並將資料存取到陣列裡，
在ADC_sampling這個函式裡的最後同樣使用thread加上eventqueue來呼叫screen_print這個函式
來將存取好的資料輸出給screen，在screen_print函式的最後也呼叫ADC_sampling函式，
使得sample跟print的函式不斷互相呼叫，進而達成sample與print時間互相錯開的效果，
這麼做是因為print所需要的時間較長，因此先在不影響波形的狀況下對波進行sample，
存完後再讓資料一次輸出，並且不採樣這時扭曲的波形，以確保sample的品質。
除此之外，由於使用了thread的方法來存取sample到的資料以及輸出資料到screen上，因此也比較不會造成時間上的延遲。


使用FFT python code來對sample好的資料作圖，結果如下:
