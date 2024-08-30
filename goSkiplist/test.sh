#!/usr/bin/env go

# 编译
go build -o goSkiplist *.go

chmod +x goSkiplist

echo "compile complete."

./goSkiplist

echo "test done, delete compile file."

rm goSkiplist