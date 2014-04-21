//実行本体

// ページが変わったときに呼ばれる
function checkForValidUrl(tabId, changeInfo, tab) {
  var index = 0;
  for (var i = 0; i < videourls.length; i++) {
    // タブのURLから文字列検索
    index = tab.url.indexOf(videourls[i]);
    if (index == 0) {	//検索結果(前方一致の場合のみ処理)
      // ページアクションのアイコン表示
      chrome.pageAction.show(tabId);
      break;
    }
  }
};

// Listen for any changes to the URL of any tab.
chrome.tabs.onUpdated.addListener(checkForValidUrl);

//クリックされた時
chrome.pageAction.onClicked.addListener(
function(tab) {
  var now = new Date();
  //alert(tab.url);	//アラート表示
  
  //送信
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "http://localhost:54321/?" + tab.url, true);
  xhr.send();
  
 // parent.main.document.write(now.toLocaleString() + "送信\"" + tab.url + "\"\n");	//書き込み
}
);
