#!/bin/sh

#youtubeのクオリティーの最大値(fmt=の値)
Youtube_MaxQuality=22

#動画再生コマンド( \$MOVIE_URL:動画URL)
#デフォルトはMPlayer。それでいい場合は0を指定。
#MOVIEPLAYER="gnome-mplayer \${MOVIE_URL}"
MOVIEPLAYER=0


#使い方
help()
{
echo "\
 === playFlashVideo.sh の説明 === \n\
　playFlashVideo.shはWebページに埋め込まれている\
Flash動画をMPlayerで再生しようというものです。\
Flashで再生すると重くて耐えられない、動作の軽い\
MPlayerで再生したいという人向けです。\n\
MPlayerの操作方法は\"mplayer -h\"で見てください。\n\
 === 対応している動画サイト === \n\
video.google.com, Youtube, Photobucket, Dailymotion,\
Metacafe, Facebook, Yahoo!, depositfiles.com \n\
 === 使用ソフト === \n\
動画再生: MPlayer \n\
動画URL解析: youtube-dl \n\
 === コマンドオプション === \n\
playFlashVideo.sh [コマンド] (動画ページのURL) \n\
動画ページのURLは\"\"で囲んで下さい。\n\
オプション: \n\
\t-h\tヘルプ表示 \n\
\t-m\tMPlayerで再生\n\
\t-f (フォーマット)\tフォーマット指定(Youtubeのみ)\n\
\t-g\t動画URLのみ取得\n\
"
}



#変数
URL=""		#最後の引数(サイトのURL)
MOVIE_URL=""	#動画URL
STAT=0	#戻り値を記憶しておく
URL_Only=0	#動画URLを返すかどうか



#
#引数の処理
#

#オプション解析
while getopts mf:gh OPT
do
	case $OPT in
		"m" ) MOVIEPLAYER=0 ;;
		"f" ) Youtube_MaxQuality="$OPTARG" ;;
		"g") URL_Only=1;;
		"h" ) help ; exit 0;;
		\?) echo "不明なオプションです。" ; exit 1 ;;
	esac
done

#最後の引数取得
eval URL=$"$#"

#URLが正しく渡されたかチェック
if [ $(expr ${URL} : "http://*") = 0 ] ; then
	echo "再生したい動画があるページのURLを指定してください。"
	echo "-h\tヘルプを表示します。"
	exit 1
fi


#
# 動画URL解析
#

# youtube-dlで解析
MOVIE_URL=`youtube-dl -g --max-quality=${Youtube_MaxQuality} ${URL}`
STAT=$?
if [ ${STAT} -ne 0 ] ; then
	echo "エラーが発生しました。youtube-dl: ${STAT}"
	exit 1;
fi

#URLのみの場合、動画URLを返して終了
if [ ${URL_Only} -eq 1 ] ; then
	echo ${MOVIE_URL}
	exit 0
fi

#
# 動画再生
#

if [ ${MOVIEPLAYER}=0 ] ; then
	# MPlayerで再生
	mplayer -fs -cache 10000 -cache-min 1 -framedrop ${MOVIE_URL} 
else
	eval ${MOVIEPLAYER}		#その他のプレイヤーで再生
fi
STAT=$?
if [ ${STAT} -ne 0 ] ; then
	echo "エラーが発生しました。動画再生: ${STAT}"
	exit 1;
fi


#終了
exit 0
