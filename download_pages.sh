#!/bin/bash

filename=$1
while read -r line
do 
    wget "https://commoncrawl.s3.amazonaws.com/$line"
done < "$filename"
