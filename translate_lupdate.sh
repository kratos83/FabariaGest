#!/bin/sh

echo "Creo traduzione fabaria della lingua italiana"
lupdate  * -ts fabaria/language/Italian.ts
echo "Creo traduzione fabaria della lingua inglese"
lupdate  * -ts fabaria/language/English.ts
