#!/bin/bash
# apt-get install p7zip-full
# 7z x stardict.7z 

# ubuntu
apt-get install p7zip-full gcc espeak espeak-data mplayer

# CentOS
# 由于CentOS使用yum install安装时很多软件默认没有，而且依赖做的很差，所以自行安装以上软件吧，如果不发音可以不安装espeak espeak-data mplayer

7z x dicts.7z

[ -f stardict ] || gcc -o stardict.exe stardict.c

if [ -d ../stardict-master ]
then
   [ -d /opt/stardict ] || cp -pfr ../stardict-master /opt/stardict
fi

if [ -d ../stardict ]
then
   [ -d /opt/stardict ] || cp -pfr ../stardict /opt/
fi

chmod +x stardict.sh
ln -s /opt/stardict/stardict.sh /opt/stardict/s

grep STARDICT_HOME ~/.bashrc >/dev/null || cat >>~/.bashrc <<_EOF_

export STARDICT_HOME=/opt/stardict
export PATH=\$PATH:\$STARDICT_HOME
alias sp="s -s "
alias sv="s -v "
_EOF_

echo install finished

echo you should run command "source ~/.bashrc", and then you can look for a word.
