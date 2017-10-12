#!/bin/bash
declare -x LANG="zh_CN.UTF-8"
declare -x LANGUAGE="zh_CN:zh:en_US:en"
declare -x PATH="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/opt/stardict"
declare -x SHELL="/bin/bash"

if [[ $# -ne 1  &&  $# -ne 2 ]]
then	
   echo "Usage: $0 word"

   echo "      Speak and Search: $0 -s word"

   echo "      Only Speak: $0 -v word"
   exit 1
fi

cd $STARDICT_HOME

#only speak
if [ "$1" == "-v" ]
then
    which mplayer >/dev/null 2>&1 
    if [ $? -eq 0 ]
    then
        url="http://v2.redlinux.org/t2v.aspx?t=mp3&l=cn&s="${2// /%20}
        mplayer "$url" >/dev/null  2>&1
    else
        echo "Install mplayer please"
    fi

#speak and look for
elif [ "$1" == "-s" ]
then
    which mplayer >/dev/null 2>&1 
    if [ $? -eq 0 ]
    then
        url="http://v2.redlinux.org/t2v.aspx?t=mp3&l=cn&s="${2// /%20}
        mplayer "$url" >/dev/null  2>&1
    else
        echo "Install mplayer please"
    fi
    /opt/stardict/stardict.exe "$2" | more

#only look for
else
    /opt/stardict/stardict.exe "$1" | more
fi
