/***************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 * 
 *            Ficheiro.cc
 *
 *  Copyright  2007  Lu�s Mendes
 *  Email
 ****************************************************************************/

/*
 * This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


//#include "stdafx.h"
#include <string.h>            //Para saber a const NULL
#include "../include/Ficheiro.hh"
#include <iostream>

using std::ios;
using namespace std;

const std::string Ficheiro::NOME_INDEFINIDO = "Default Format";

Ficheiro::Ficheiro() {
  endereco_inicial = 0x0000;
  estado = stat_ok;
  resultado = ok;
  bytes_dados_lidos = 0;
  modo = leitura;
  nome_formato = NULL;
  nome_ficheiro[0] = '\0';
  escrita_em_curso_flag = 0;
}

Ficheiro::~Ficheiro() {
}


int Ficheiro::obtem_estado() {
  return estado;
}

int Ficheiro::obtem_resultado_oper() {
  return resultado;
}

void Ficheiro::prepara_escrita(unsigned int endereco, unsigned int tamanho) {
  Memoria *mem;

  if (escrita_em_curso_flag == 1) {
    resultado = escrita_em_curso;
    return;
  }

  if (buffers.procura_elem(endereco) != NULL) {
    resultado = endereco_repetido;
    return;
  }

  mem = buffers.cria_insere_elem();
  if (mem == NULL) {
    //Isto estraga a consistência (adicionar códigos de erro na lista.cc, para que se possam gerar msgs)
    resultado = impossivel_criar_mem;
    return;
  }

  cout << "A Preparar escrita - tamanho: " << tamanho << " endereco: " << endereco << endl;
  mem->configura(tamanho, endereco, 8, NULL);

  resultado = ok;
}

void Ficheiro::finaliza_escrita() {
  escrita_em_curso_flag = 0;
}

void Ficheiro::coloca_endereco_inicial(unsigned int endereco) {
  if (!this->possui_enderecamento())
    endereco_inicial = endereco;
}

unsigned int Ficheiro::obtem_endereco_inicial() {
  return endereco_inicial;
}

int Ficheiro::possui_enderecamento() {
  return 0;
}

const char *Ficheiro::obtem_nome_formato() { 
  if (nome_formato == NULL)
    return Ficheiro::NOME_INDEFINIDO.c_str();
  
  return nome_formato;
}

unsigned int Ficheiro::obtem_bytes_dados_lidos() {
  return bytes_dados_lidos;
}

int Ficheiro::obtem_modo_ficheiro() {
  return modo;
}
