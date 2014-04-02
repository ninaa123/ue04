#!/bin/bash

function createPdf {
#    rm "$1.png"
    rm "$1.pdf"
    cat envelope.tex-part1 > tmp.tex
    echo "\input{$1}" >> tmp.tex
    cat envelope.tex-part2 >> tmp.tex
    pdflatex -shell-escape -synctex=1 -interaction=nonstopmode --src-specials tmp.tex
    pdfcrop tmp.pdf "$1.pdf"
#    convert -density 1200x1200 "$1.pdf" "$1.png"
    rm "$1.tex"
    rm "$1.eps"
}

gnuplot ue04.gp
createPdf "rep_var_boxes20"
createPdf "rep1e8_boxes_var"

rm tmp.tex
rm tmp.pdf
rm *.synctex.gz
rm *.aux
rm *.log
rm *eps-converted-to.pdf

exit 0

