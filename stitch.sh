#!/bin/bash

sid=`sed 1q < session.id`
while read M A B C D; do
	printf '<room roomId="%s">' $sid
    case $M in
        N)
            printf '<data class="NullMove"/>'
            ;;
        S)
            printf '<data class="SetMove" setX="%s" setY="%s"/>' $A $B
            ;;
        R)
            printf '<data class="RunMove" fromX="%s" fromY="%s" toX="%s" toY="%s"/>' $A $B $C $D
            ;;
        *)
            printf '<data class="NullMove"><hint content="Oops! M: %s A: %s B: %s C: %s D: %s"/></data>' $M $A $B $C $D
            ;;
    esac
    echo '</room>'
done
