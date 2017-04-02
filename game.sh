#!/bin/bash

pipe1=/tmp/pipe1
pipe2=/tmp/pipe2
role_type=0
gameLoop=true
gameOver=false
noHod=false

function deleteByRoleFIFO {
	if [ $role_type -eq 1 ]; then
			rm $pipe1
		else
			if [ $role_type -eq 2 ]; then
				rm $pipe2
			fi
	fi
}

function deleteFIFO {
	if [ -p $pipe1 ]; then
		rm $pipe1
	fi
	if [ -p $pipe2 ]; then
		rm $pipe2
	fi
}

function detectPlayer {
	if [ ! -p $pipe1 ]; then
		mkfifo $pipe1
		role_type=1
		return
	fi


	if [ ! -p $pipe2 ]; then
		mkfifo $pipe2
		role_type=2
		return
	fi

	role_type=3
}

function initGame {

	a[1]=' '
	a[2]=' '
	a[3]=' '
	a[4]=' '
	a[5]=' '
	a[6]=' '
	a[7]=' '
	a[8]=' '
	a[9]=' '
	clear
	set_cursor 0 0
	showTable
}

set_cursor() {
	tput cup $1 $2
}

function showTable {
	echo ┌─┬─┬─┐
	echo │${a[1]}│${a[2]}│${a[3]}│
	echo ├─┼─┼─┤
	echo │${a[4]}│${a[5]}│${a[6]}│
	echo ├─┼─┼─┤
	echo │${a[7]}│${a[8]}│${a[9]}│
	echo └─┴─┴─┘
	echo Player: $role_type
}
function checkNoHod {
    countHod=0
    if [[ ${a[1]} != ' ' ]]; then
    	countHod=$((countHod + 1))
    fi
    if [[ ${a[2]} != ' ' ]]; then
        countHod=$((countHod + 1))
    fi
    if [[ ${a[3]} != ' ' ]]; then
        countHod=$((countHod + 1))
    fi
    if [[ ${a[4]} != ' ' ]]; then
        countHod=$((countHod + 1))
    fi
    if [[ ${a[5]} != ' ' ]]; then
        countHod=$((countHod + 1))
    fi
    if [[ ${a[6]} != ' ' ]]; then
        countHod=$((countHod + 1))
    fi
    if [[ ${a[7]} != ' ' ]]; then
        countHod=$((countHod + 1))
    fi
    if [[ ${a[8]} != ' ' ]]; then
        countHod=$((countHod + 1))
    fi
    if [[ ${a[9]} != ' ' ]]; then
        countHod=$((countHod + 1))
    fi
    if [[ $countHod -eq 9 ]]; then
        noHod=true
    fi
}
function checkWin {

	win[1]=${a[1]}${a[2]}${a[3]}
	win[2]=${a[4]}${a[5]}${a[6]}
	win[3]=${a[7]}${a[8]}${a[9]}

	win[4]=${a[1]}${a[4]}${a[7]}
	win[5]=${a[2]}${a[5]}${a[8]}
	win[6]=${a[3]}${a[6]}${a[9]}

	win[7]=${a[1]}${a[5]}${a[9]}
	win[8]=${a[3]}${a[5]}${a[7]}

	for i in {1..8}
	do
		if [[ "${win[$i-1]}" == "xxx" || "${win[$i-1]}" == "ooo" ]]; then
			gameOver=true
		fi
	done
}


# START:

detectPlayer

myHod=false

if [ $role_type == 1 ]; then
	myHod=true
	mySymbol='x'
	enemySymbol='o'
	out=$pipe1
	inp=$pipe2
fi

if [ $role_type == 2 ]; then
	myHod=false
	mySymbol='o'
	enemySymbol='x'
	out=$pipe2
	inp=$pipe1
fi

initGame

error=true

if [ $role_type == 3 ]; then
	while $gameLoop
		do
			inp=$pipe1
			read numberToArray < $inp
			echo $numberToArray
		done
fi

while $gameLoop
	do 
		if [ $myHod == true ]; then
			set_cursor 8 0
			echo '                                   '
			set_cursor 8 0
			read -p 'Ваш ход: ' hod
			col=${hod:2:3}
			row=${hod:0:1}
			errorRow=true
			errorCol=true

			if [ $col == 1 ]; then
                errorCol=false
            fi
            if [ $col == 2 ]; then
                errorCol=false
            fi
            if [ $col == 3 ]; then
                errorCol=false
            fi

			if [ $row == 1 ]; then
				errorRow=false
				numberToArray=$col
			fi
			if [ $row == 2 ]; then
				errorRow=false
				numberToArray=$col+3
			fi
			if [ $row == 3 ]; then
				errorRow=false
				numberToArray=$col+6
			fi

			if [[ $errorCol == false && $errorRow == false ]]; then
                error=false
            fi

			if [ $error == true ]; then
			    echo '                                   '
                set_cursor 8 0
			fi

			if [ $error == false ]; then
				a[$numberToArray]=$mySymbol

				set_cursor 0 0
				showTable

				checkWin
				checkNoHod
				
				echo Waiting opponent ...
				echo -n $numberToArray > $out

				myHod=false

				if [ $gameOver == true ]; then
					set_cursor 8 0
					echo '                                   '
					set_cursor 8 0
					echo "you win"
					gameLoop=false
				fi
				if [ $noHod == true ]; then
                    set_cursor 8 0
                    echo '                                   '
                    set_cursor 8 0
                    echo "no way"
                    gameLoop=false
                fi
			fi
		else 
			read numberToArray < $inp

			a[$numberToArray]=$enemySymbol

			set_cursor 0 0
			showTable

			myHod=true

			checkWin
			checkNoHod

			if [ $gameOver == true ]; then
				set_cursor 8 0
				echo '                                   '
				set_cursor 8 0
				echo "you loose"
				gameLoop=false
			fi
			if [ $noHod == true ]; then
                set_cursor 8 0
                echo '                                   '
                set_cursor 8 0
                echo "no way"
                gameLoop=false
            fi
		fi
	done

deleteByRoleFIFO