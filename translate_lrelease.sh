#!/bin/sh

echo "Creo traduzione fabaria della lingua italiana"
lrelease  fabaria/language/Italian.ts -qm  fabaria/language/Italian.qm
echo "Creo traduzione fabaria della lingua inglese"
lrelease  fabaria/language/English.ts -qm  fabaria/language/English.qm
