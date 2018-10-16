#!/bin/bash

filename=$1
while read -r line
do 
    wget -P pages/ "https://commoncrawl.s3.amazonaws.com/$line"
done < "$filename"
