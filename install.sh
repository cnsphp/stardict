#!/bin/bash
# apt-get install p7zip-full
# 7z x stardict.7z 

apt-get install p7zip-full espeak espeak-data mplayer

[ -f stardict ] || gcc -o stardict.exe stardict.c

[ -d /opt/stardict ] || cp -pfr ../stardict /opt

grep STARDICT_HOME ~/.bashrc >/dev/null || cat >>~/.bashrc <<_EOF_

export STARDICT_HOME=/opt/stardict
export PATH=\$PATH:\$STARDICT_HOME
alias sp="s -s "
alias sv="s -v "
_EOF_

echo install finished

echo you should run command "source ~/.bashrc", and then you can look for a word.

