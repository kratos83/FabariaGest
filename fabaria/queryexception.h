/****************************************************************************
**
** Copyright (C) 2006-2018 Angelo e Calogero Scarna
** Contact: Angelo Scarnà (angelo.scarna@codelinsoft.it)
**          Calogero Scarnà (calogero.scarna@codelinsoft.it)
**          Team Codelinsoft (info@codelinsoft.it)
**
** This file is part of the project FabariaGest
**
** LGPLv3 License
**
**  You may use this file under the terms of the LGPLv3 license as follows:
*
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Codelinsoft and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
****************************************************************************/
#ifndef QUERYEXCEPTION_H
#define QUERYEXCEPTION_H

#include <iostream>
#include <QtGui>

using namespace std;

/**********************************************************
 * La classe QueryException serve a gestire le eccezioni
 * durante l'inserimento, aggiornamento e eliminazione
 * di una query;
 * *********************************************************/
class QueryException
{
public:
      QueryException(const char* query="") : messaggio(query) {}
      virtual string get_messagges() throw() {return messaggio;}
private:
  string messaggio;
};

/**********************************************************
 * La classe InvalidQuery serve a gestire le eccezioni
 * durante l'inserimento, aggiornamento e eliminazione
 * di una query;inoltre visualizza un messaggio di errore
 * se l'operazione non va a buon fine...
 * *********************************************************/
class InvalidQuery : public QueryException
{
public:
      InvalidQuery(string messaggio) : QueryException(messaggio.c_str()) {}
};

/**********************************************************
 * La classe InvalidFile serve a gestire le eccezioni
 * durante l'inserimento di un file. 
 * *********************************************************/
class InvalidFile : public QueryException
{
public:
      QString file_path;
      InvalidFile(string messaggio, QString path_file) 
      : QueryException(messaggio.c_str()),file_path(path_file) {}
};

//Gestisco eccezione software
class FabariaException : public std::exception
{
public:
      
      inline virtual const char* what() const throw() { return "FabariaException"; }     
};
#endif // QUERYEXCEPTION_H

