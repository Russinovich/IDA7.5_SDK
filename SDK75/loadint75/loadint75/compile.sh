#! /bin/bash

#set -x
set -e

# NLSPATH should point to a directory with IDA.HLP file!

NLSPATH=.

if [ `uname` == "Darwin" ]
then
  SYSNAME=mac
else
  SYSNAME=linux
fi

$SYSNAME/loadint     comment.cmt ida.int
$SYSNAME/loadint64   comment.cmt ida64.int
$SYSNAME/btc ida.int idabase.tmp
cp idabase.tmp ida.int
rm idabase.tmp
$SYSNAME/btc ida64.int idabase.tmp
cp idabase.tmp ida64.int
rm idabase.tmp
