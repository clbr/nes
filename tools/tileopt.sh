#!/bin/sh
#
# Optimize (remove identical tiles) from a PNG image.
#
# Works for any paletted PNG with <= 16 colors.
#

[ "$#" -lt 1 ] && echo "Usage: $0 file.png [out.png]" && exit

in=$1

if [ "$#" -gt 1 ]; then
	out=$2
else
	out=${1%.png}_opt.png
fi

echo Optimizing $in to $out
file $in

dir=`mktemp -d`

convert $in -crop 8x8 $dir/foo_%03d.bmp

# Remove identical ones
md5sum $dir/foo*bmp | sort > $dir/sums

prevsum=
while read sum name; do
	[ "$sum" = "$prevsum" ] && rm $name

	prevsum=$sum
done < $dir/sums

montage -geometry +0+0 -background black -tile 16x $dir/*bmp $out
mifcon -indexed $out
pngreorder -s $in $out

file $out

rm -rf $dir
