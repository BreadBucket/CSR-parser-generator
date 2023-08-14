#!/bin/bash


grep "compares" <&0 | grep -oP "\).*" | grep -oP "\w.*" | sed -e "s/-->/->/g"