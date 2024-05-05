#!/bin/bash
cd "$(dirname "$0")"
rm -rf ./dist
mkdir ./dist
gzip -k ./*
mv ./*.gz ./dist
