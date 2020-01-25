FabariaGest 2.0
=========
(C) Copyright 2010 Angelo Scarnà. Tutti diritti riservati.

Software per la gestione di aziende
------------------------------------------------------------------------
Un software gestionale che ti aiuta ad organizzare le attività commerciali della tua azienda. FabariaGest permette di gestire in modo semplice e intuitivo fatture, prezzi, schede prodotto, clienti e molto altro.
Dopo una semplice installazione, il programma è già pronto per l'utilizzo e non ha bisogno di altre operazioni. Grazie alla struttura del database, la ricerca degli elementi negli archivi è facile ed estremamente veloce. Puoi esportare tutti i documenti (clienti, fornitori, articoli, preventivi, ordini, inventari, elenchi vari..) nei principali formati d'uso (.doc, .xls, .pdf).


Per poter installare FabariaGest da sorgente bisogna 
scaricare il programma all'indirizzo:

http://www.codelinsoft.it/sito

Una volta scaricato aprire il terminale e dicitare i seguenti comandi

qt5-qtbase
qt5-qtgui-private
qt5-qtbase-mysql

Per linux lsb_release

zlib

-------------------------------------------
Se utilizzato come server mysql installare
mysql-server
mysql-client

altrimenti utilizzare MariaDB

mariadb sia client che server

--------------------------------------------

Installazione FabariaGest

tar -xvzf fabariagest-2.0.tar.gz

cd fabariagest-2.0

Compilare con le Qt in versione 5 e
eseguire il seguente comando

cmake -DCMAKE_INSTALL_PREFIX=/opt/FabariaGest -DCMAKE_INSTALL_LIBDIR=/opt/FabariaGest/lib

make-j5

sudo make install

---------------------------------------------
ATTENZIONE: una volta installato il software, il database deve essere modificato nel seguente modo:

1) Creare un backup di sicurezza
2) Una volta creato il backup, ci pensa il software ad aggiornare il database.
