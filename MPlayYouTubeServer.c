#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define DEAMON 1		//デーモンとして起動するかどうか

#define MPLAYSCRIPT ~/.playFlashVideo.sh
#define PORT 54321	//使用するポート


int mainLoop()
{
 int sock0;
 struct sockaddr_in addr;
 struct sockaddr_in client;
 int len;
 int sock;
 int yes = 1;

 char buf[2048];
 char inbuf[2048];
 char syscom[2048];	//system()で実行するコマンド

 //解析用
 int loop;	//ループするかどうか
 int p, q, r;	//解析した文字数
 char request[1024], url[1024];	//解析結果（GETリクエスト）

 // sou
 const char buf_tmplate[] = ""
	 "HTTP/1.0 200 OK\n"
	 "Content-Length: %lu\n"
	 "Content-Type: text/html\n"
	 "\n%s";
 const char buf_html[] = "<html><body><h1>OK!</h1></body></html>\n";
 const char buf_htmlexit[] = ""
	 		"<html><body><h1>OK!</h1>"
			"<p>MPlayYouTubeServerを終了します。</p>"
			"</body></html>\n";
 sock0 = socket(AF_INET, SOCK_STREAM, 0);
 if (sock0 < 0) {
	 perror("socket");
	 return 1;
 }

 addr.sin_family = AF_INET;
 addr.sin_port = htons(PORT);
 addr.sin_addr.s_addr = INADDR_ANY;

 setsockopt(sock0,
   SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes));

 if (bind(sock0, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
	 perror("bind");
	 return 1;
 }

 if (listen(sock0, 5) != 0) {
	 perror("listen");
	 return 1;
 }

 // 応答用HTTPメッセージ作成
 memset(buf, 0, sizeof(buf));
 sprintf(buf, buf_tmplate, sizeof(buf_html), buf_html );
 printf("送信メッセージ:\n%s\n",buf);

//受信ループ
 while (1) {
   printf("待機中。。。\n");
   
   len = sizeof(client);
   sock = accept(sock0, (struct sockaddr *)&client, &len);	//接続があるまで待機
   if (sock < 0) {
	   perror("error: accept\n");
	   break;
   }

   memset(inbuf, 0, sizeof(inbuf));
   recv(sock, inbuf, sizeof(inbuf), 0);
   printf("受信:\n\n%s", inbuf);
	
	// 解析
	if( !strncmp(inbuf, "GET", 3) ){
		p=4; q=0; r=0; loop=1;
		while( loop ){
			request[q]=inbuf[p++];
			switch(request[q++]){
				case '?':
					if( loop==1 ){ loop=2; }
					else if( loop==2 ) url[r++] = request[q-1];
					break;
				case '\0':
				case ' ':
					loop=0;
					request[q]='\0';
					url[r]='\0';
					break;
				default:
					if(loop==2) url[r++] = request[q-1];
			}
		}
	}else{
		sprintf(request, "error");
	}
	printf("解析結果%d: %s\n", q, url);

	// exitが来ていたらHTTPメッセージを変える
	if( strcmp(url, "exit") == 0 ){
		sprintf(buf, buf_tmplate, sizeof(buf_htmlexit), buf_htmlexit );
		printf("送信メッセージ(exit用):\n%s\n",buf);
	}

	// 相手が何を言おうとダミーHTTPメッセージ送信
	send(sock, buf, (int)strlen(buf), 0);

	close(sock);
	
	//exit 判定
	if( strcmp(url, "exit") == 0 ) break;
	
	//シェルスクリプト実行
	printf("playFlashVideo.sh 実行！\n");
	sprintf(syscom, "nohup MPLAYSCRIPT \"%s\" > /dev/null &", url);
	system(syscom);

 }

 close(sock0);

 printf("終了\n");

 return 0;
}


int main(void) {
#if DEAMON
	if(daemon(0, 0) == 0) {
#else
	if(1){
#endif
		mainLoop();
	} else {
		printf("error\n");
	}
	return 0;
}
