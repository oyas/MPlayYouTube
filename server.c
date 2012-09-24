#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main()
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

 sock0 = socket(AF_INET, SOCK_STREAM, 0);
 if (sock0 < 0) {
	 perror("socket");
	 return 1;
 }

 addr.sin_family = AF_INET;
 addr.sin_port = htons(12345);
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
 sprintf(buf,
	 "HTTP/1.0 200 OK\r\n"
	 "Content-Length: 40\r\n"
	 "Content-Type: text/html\r\n"
	 "\r\n"
	 "<html><body><h1>OK!</h1></body></html>\r\n");
   printf("送信メッセージ:\n%s\n",buf);

//受信ループ
 while (1) {
   printf("待機中。。。\n");
   
   len = sizeof(client);
   sock = accept(sock0, (struct sockaddr *)&client, &len);
   if (sock < 0) {
	   perror("accept");
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
	}else{ sprintf(request, "error"); }
	printf("解析結果%d: %s\n", q, url);

   // 相手が何を言おうとダミーHTTPメッセージ送信
   send(sock, buf, (int)strlen(buf), 0);

   close(sock);
	
	//exit 判定
	if( strcmp(url, "exit") == 0 ) break;
	
	//シェルスクリプト実行
	printf("playFlashVideo.sh 実行！\n");
	sprintf(syscom, "nohup ./playFlashVideo.sh \"%s\" > /dev/null &", url);
	system(syscom);

 }

 close(sock0);

 printf("終了\n");

 return 0;
}