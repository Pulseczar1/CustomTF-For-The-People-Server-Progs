#!/bin/bash
# Warning: This is a bash script. Running this in a non-bash shell could cause destruction of your data.
# PZ: This is used to generate progs header files from source files. It currently only grabs functions.
# PZ NOTE: Compile individual files with:  g++ -std=c++11 -DSERVERONLY -DPROGS_CUTF -m32 -Wall -g -I./ -I.. -I../../client -Wno-trigraphs -c airfist.cpp

# Use this for debugging the script.
#set -xv

# check that input file exists
if [ ! -f "$1" ]; then
    echo "File not found!"
	exit 1
fi

fileName="$1"
nameWithoutExtension=${fileName%.cpp}
newFileName=${nameWithoutExtension}.h

# warn if output file already exists
if [ -f "$newFileName" ]; then
	read -p "$newFileName already exists. Enter 'Y' to overwrite it. " -r
	if [[ ! $REPLY =~ ^[Yy]$ ]]
	then
		exit 1
	fi
fi

# clear the file
#echo | tee $newFileName        # this also adds a new line
> $newFileName

# file header
nameWithoutExtension=${nameWithoutExtension^^}                     # make uppercase
echo "#ifndef ${nameWithoutExtension}_H" | tee -a $newFileName
echo "#define ${nameWithoutExtension}_H" | tee -a $newFileName
echo | tee -a $newFileName                                         # this adds a new line
echo "namespace Progs {" | tee -a $newFileName
echo | tee -a $newFileName


# example:  void AGR_think()
# file meat  (This grabs all function headings (mustn't end in semi-colon). Then it trashes everything after right parenthesis, and places a semi-colon on the end.)
#grep -io '^[A-Za-z_].*[A-Za-z_].*(.*)[^;={]*[ \t]*$' $fileName | sed 's#(.*)#\0;#' | tee -a $newFileName
#grep -io '^[A-Za-z_].*\b[A-Za-z_].*(.*)[\b]*[^;]*$' $fileName | sed 's#\(^[A-Za-z_].*\b[A-Za-z_].*(.*)\).*$#\1;#' | tee -a $newFileName
#grep -io '^[A-Za-z_][A-Za-z_0-9]*\s[A-Za-z_][A-Za-z_0-9]*[\s]*(.*)[\s]*[^;]*$' $fileName | sed 's#\(^[A-Za-z_][A-Za-z_0-9]*\s[A-Za-z_][A-Za-z_0-9]*[\s]*(.*)\).*$#\1;#' | tee -a $newFileName

# TRY THIS ONE TO GRAB ALL FRAME STATE FUNCTIONS. Warning: This will also grab all prototypes from the file.
#grep -io '^[A-Za-z_][A-Za-z_0-9]*\s[A-Za-z_][A-Za-z_0-9]*[\s]*([^{}():;=]*)[\s]*[^;]*' $fileName | sed 's#\(^[A-Za-z_][A-Za-z_0-9]*\s[A-Za-z_][A-Za-z_0-9]*[\s]*([^{}():;=]*)\).*$#\1;#' | tee -a $newFileName
grep -io '^[A-Za-z_][A-Za-z_0-9]*\s[A-Za-z_][A-Za-z_0-9]*[\s]*([^{}():;=]*)[\s]*[^;]*$' $fileName | sed 's#\(^[A-Za-z_][A-Za-z_0-9]*\s[A-Za-z_][A-Za-z_0-9]*[\s]*([^{}():;=]*)\).*$#\1;#' | tee -a $newFileName

# file footer
echo | tee -a $newFileName
echo "} // END namespace Progs" | tee -a $newFileName
echo | tee -a $newFileName
echo "#endif // END ${nameWithoutExtension}_H" | tee -a $newFileName
echo | tee -a $newFileName

