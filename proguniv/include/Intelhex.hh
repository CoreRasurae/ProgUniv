/***************************************************************************
 *            intelhex.hh
 *
 *  Copyright  2005  Lu�s Mendes
 *  Email
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _INTELHEX_HH_
#define _INTELHEX_HH_

#include <iostream>
#include <fstream>
#include "Ficheiro.hh"

using std::ios;
using std::fstream;

using namespace std;

#define TAM_NOME_FICH 255

class Intelhex : public Ficheiro {
private:
  int ficheiro_aberto;
  int modificado;
  int segmento_actual;
  unsigned int endereco_high_actual;
  int bytes_por_linha;
  fstream ficheiro_hex;

protected:
  void escreve_buffer(Memoria *buf);
  void criaEntradaDeExtendedLinearAddress();

public:
  Intelhex();
  ~Intelhex();
  
  int possui_enderecamento();
  void abre(std::string nome, opcoes_ficheiro opcoes);
  void fecha();
  void le(unsigned int endereco, unsigned char *valor);
  void escreve(unsigned int endereco, unsigned char valor);
  
};

#endif /* _INTELHEX_HH_ */
