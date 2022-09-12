#!/bin/bash
if [ "$1" -eq "1" ] ; then 
	touch wynik.txt
	echo "Adres MAC" > wynik.txt
	ip a | grep "link/ether" | awk  '{print $2}' >> wynik.txt
	echo "Brama domyslna" >> wynik.txt
	ip route | grep "default via" | awk  '{print $3}' >> wynik.txt
	echo "Adres IP/Maska sieciowa" >> wynik.txt
	ip a | grep "inet" | grep "brd" | awk  '{print $2}' >> wynik.txt
	echo "Adres rozgloszeniowy" >> wynik.txt
	ip a | grep "inet" | grep "brd" | awk  '{print $4}' >> wynik.txt
else 
	if [ "$1" -eq "2" ] ; then 
		echo "Adres MAC"
		ip a | grep "link/ether" | awk  '{print $2}'
		echo "Brama domyslna"
		ip route | grep "default via" | awk  '{print $3}'
		echo "Adres IP/Maska sieciowa"
		ip a | grep "inet" | grep "brd" | awk  '{print $2}'
		echo "Adres rozgloszeniowy"
		ip a | grep "inet" | grep "brd" | awk  '{print $4}'
		
	else
		echo "BLAD"
	fi
fi
