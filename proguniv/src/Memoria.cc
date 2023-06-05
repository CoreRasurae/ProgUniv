/***************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 *
 *            Memoria.cc
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

#include <math.h>
#include <string.h>
#include <iostream>

using std::ios;
using namespace std;

//#include "stdafx.h"
#include "../include/Memoria.hh"
#include "../include/Ficheiro.hh"

const unsigned int_bits = 32;

/* Defini��o dos valores das flags */
const int mem_nao_configurada = 1; /* Indica se a mem�ria ainda n�o tem nenhuma configura��o atribu�da */
const int usa_enderecos       = 2; /* Indica se a mem�ria utiliza endere�os ou funciona como um registo */

void Memoria::inicializa() {
  flags = 0;
  coloca_usa_enderecos(0);
  coloca_mem_nao_configurada(1);
  tamanho = 0;
  endereco_inicial = 0x0000;
  tamanho_palavra = 0;
  bytes_palavra = 0;
  dados = NULL;
  nome = NULL;
	
  resultado_oper = ok;
}

void Memoria::inicializa(unsigned int novo_tamanho, unsigned int novo_endereco, int novo_tamanho_palavra, const char *novo_nome) {
  inicializa();

  if (novo_tamanho_palavra <= 0 || novo_tamanho_palavra > 32) {
    resultado_oper = tamanho_palavra_invalido;
    return;
  }

  if (novo_tamanho <= 0) {
    resultado_oper = tamanho_invalido;
    return;
  }

  if (novo_endereco + novo_tamanho < novo_endereco) {
    resultado_oper = tamanho_invalido;
    return;
  }

  coloca_usa_enderecos(1);
  coloca_mem_nao_configurada(0);

  if (novo_nome != NULL) {
    this->nome = new char[MEM_TAM_NOME + 1];
    strncpy(this->nome, novo_nome, MEM_TAM_NOME);
  }

  tamanho = novo_tamanho;
  tamanho_palavra = novo_tamanho_palavra;
  endereco_inicial = novo_endereco;
  dados = new unsigned int[tamanho];

  bytes_palavra = (int)ceil((double) tamanho_palavra / 8.0);

  //bytes_palavra = tamanho_palavra >> 3;
  //if (tamanho_palavra & 0x07 > 0)
  //  bytes_palavra++;

  resultado_oper = ok;
}

void Memoria::inicializa(int novo_tamanho_palavra, const char *novo_nome) {
  inicializa();

  if (novo_nome == NULL) {
    resultado_oper = nome_invalido;
    return;
  }

  if (novo_tamanho_palavra < 0 || novo_tamanho_palavra > 32) {
    resultado_oper = tamanho_palavra_invalido;
    return;
  }

  this->nome = new char[MEM_TAM_NOME + 1];
  strncpy(this->nome, novo_nome, MEM_TAM_NOME);
  tamanho_palavra = novo_tamanho_palavra;
  tamanho = 1;
  endereco_inicial = 0;
  dados = new unsigned int[1];

  coloca_usa_enderecos(0);
  coloca_mem_nao_configurada(0);

  bytes_palavra = (int)ceil((double) tamanho_palavra / 8.0);

  //bytes_palavra = tamanho_palavra >> 3;
  //if (tamanho_palavra & 0x07 > 0)
  //  bytes_palavra++;

  resultado_oper = ok;
}

Memoria::Memoria() {
	inicializa();
}

Memoria::Memoria(unsigned int novo_tamanho, unsigned int novo_endereco, int novo_tamanho_palavra, const char *novo_nome) {
	inicializa(novo_tamanho, novo_endereco, novo_tamanho_palavra, novo_nome);
}

Memoria::Memoria(int novo_tamanho_palavra, const char *novo_nome) {
	inicializa(novo_tamanho_palavra, novo_nome);
}

Memoria::~Memoria() {
  if (dados != NULL) {
    delete [] dados;
  }
  
  dados = NULL;
}

unsigned int Memoria::aplica_mascara(unsigned int valor) {
  unsigned int i;
  unsigned int mascara;

  mascara = 0;

  for (i = 0; i < tamanho_palavra; i++)
    mascara |= 1 << i;
    
  return valor & mascara;
}

int Memoria::obtem_mem_nao_configurada() {
  return (flags & mem_nao_configurada) >> (mem_nao_configurada - 1);
}

void Memoria::coloca_mem_nao_configurada(int estado) {
  estado &= 1;

  if (estado)
    flags |= mem_nao_configurada;
  else
    flags &= ~(mem_nao_configurada);
}

int Memoria::obtem_usa_enderecos() {
  return (flags & usa_enderecos) >> (usa_enderecos - 1);
}

void Memoria::coloca_usa_enderecos(int estado) {
  estado &= 1;

  if (estado)
    flags |= usa_enderecos;
  else
    flags &= ~(usa_enderecos);
}


int Memoria::pertence_gama_enderecos(unsigned int endereco) {
  if (obtem_mem_nao_configurada()) {
    resultado_oper = memoria_nao_configurada;
    return 0;
  }
    
  if (obtem_usa_enderecos() == 0) {
    resultado_oper = operacao_nao_suportada;
    return 0;
  }
    
  if (tamanho == 0 || dados == NULL) {
  	resultado_oper = endereco_invalido;
    return 0;
  }

  if (endereco_inicial <= endereco && (endereco_inicial + tamanho - 1) >= endereco) {
  	resultado_oper = ok;
    return 1;
  }
  else {
  	resultado_oper = endereco_invalido;
    return 0;
  }
}

void Memoria::configura(unsigned int tamanho, unsigned int novo_endereco_inicial, int novo_tamanho_palavra, const char *novo_nome) {
  if (dados != NULL)
    delete dados;

  if (nome != NULL)
    delete nome;

  inicializa(tamanho, novo_endereco_inicial, novo_tamanho_palavra, novo_nome);
}

void Memoria::configura(int tamanho_palavra, const char *novo_nome) {
  if (dados != NULL)
    delete dados;

  if (nome != NULL)
    delete nome;

  inicializa(tamanho_palavra, novo_nome);
}

unsigned int Memoria::obtem_endereco_inicial() {
  if (obtem_mem_nao_configurada()) {
    resultado_oper = memoria_nao_configurada;
    return 0; /* Pode ser confundido com endere�o v�lido */
  }
    
  if (obtem_usa_enderecos() == 0) {
    resultado_oper = operacao_nao_suportada;
    return 0;
  }

  return endereco_inicial;
}

/* Devolve tamanho da mem�ria (n�mero total de palavras)
 */
unsigned int Memoria::obtem_tamanho() {
  return tamanho;
}

char *Memoria::obtem_nome() {
  if ( obtem_mem_nao_configurada() ) {
    resultado_oper = memoria_nao_configurada;
    return NULL;
  }
    
  return this->nome;
}

void Memoria::le(unsigned int endereco, unsigned int *valor) {
  if (obtem_mem_nao_configurada()) {
    resultado_oper = memoria_nao_configurada;
    return;
  }
    
  if (obtem_usa_enderecos() == 0) {
    resultado_oper = operacao_nao_suportada;
    return;
  }

  if (!pertence_gama_enderecos(endereco)) {
    resultado_oper = endereco_invalido;
    return;
  }

  if (valor == NULL) {
    resultado_oper = erro;
    return;
  }
  
  endereco -= endereco_inicial;

  resultado_oper = ok;

  *valor = dados[endereco];
}

void Memoria::le(unsigned int *valor) {
  if (obtem_mem_nao_configurada()) {
    resultado_oper = memoria_nao_configurada;
    return;
  }
    
  if (obtem_usa_enderecos() == 1) {
    resultado_oper = operacao_nao_suportada;
    return;
  }

  resultado_oper = ok;

  *valor = *dados;
}

void Memoria::escreve(unsigned int endereco, unsigned int valor) {
  if (obtem_mem_nao_configurada()) {
    resultado_oper = memoria_nao_configurada;
    return;
  }
    
  if (obtem_usa_enderecos() == 0) {
    resultado_oper = operacao_nao_suportada;
    return;
  }

  if (!pertence_gama_enderecos(endereco)) {
    resultado_oper = endereco_invalido;
    return;
  }

  endereco -= endereco_inicial;
  dados[endereco] = aplica_mascara(valor);

  resultado_oper = ok;
}

void Memoria::escreve(unsigned int valor) {
  if (obtem_mem_nao_configurada()) {
    resultado_oper = memoria_nao_configurada;
    return;
  }
    
  if (obtem_usa_enderecos() == 1) {
    resultado_oper = operacao_nao_suportada;
    return;
  }

  *dados = aplica_mascara(valor);
  
  resultado_oper = ok;
}

int Memoria::obtem_resultado_oper() {
  return resultado_oper;
}

/* Escreve o conte�do da mem�ria para ficheiro, convertendo as palavras no
 * formato de ficheiro
 */
void Memoria::escreve_ficheiro(Ficheiro *fich) {
  unsigned int i, j;

  if (fich == NULL) {
    resultado_oper = ficheiro_invalido;
    return;
  }

  if (fich->possui_enderecamento()) {
    fich->prepara_escrita(endereco_inicial * bytes_palavra, tamanho * bytes_palavra);
    if (fich->obtem_resultado_oper() != Ficheiro::ok) {
    	resultado_oper = erro_ficheiro;
    	return;
    }
  }

  unsigned int base = endereco_inicial * bytes_palavra;
  for (i = 0; i < tamanho; i++) {
    for (j = 0; j < (unsigned)bytes_palavra; j++) {
      fich->escreve(base + (i * bytes_palavra) + j, (dados[i] >> (8 * j)) & 0xff);

      if (fich->obtem_resultado_oper() != Ficheiro::ok) {
		resultado_oper = erro_ficheiro;
		return;
      } 
    }
  }

  fich->finaliza_escrita();

  resultado_oper = ok;
}

/* L� o conte�do de um ficheiro para dentro da mem�ria
 * Adaptando o formato universal de buffer de ficheiro ao seu tamanho de
 * palavra
 */
void Memoria::le_ficheiro(Ficheiro *fich) {
  unsigned int i, j;
  unsigned int palavra;
  unsigned int endereco;
  unsigned char byte_lido;
  int colocar_em_memoria;

  if (fich == NULL) {
    resultado_oper = ficheiro_invalido;
    return;
  }

  for (i = 0; i < tamanho; i++) {
    palavra = 0;
    colocar_em_memoria = 1;

    for (j = 0; j < (unsigned)bytes_palavra; j++) {
      endereco = ((endereco_inicial + i) * bytes_palavra) + j;
      fich->le(endereco, &byte_lido);
      palavra |= byte_lido << 8 * j;

      if (fich->obtem_resultado_oper() != Ficheiro::ok)
		colocar_em_memoria = 0;
    }

    if (colocar_em_memoria == 1) {
      dados[endereco_inicial + i] = palavra;
    }
  }

  resultado_oper = ok;
}

int Memoria::compara(Memoria *mem) {
	unsigned int i;
	unsigned int valor1, valor2;
	int resultado = 1;
	
	if (mem == NULL)
		return 0;
		
	if (mem->flags != this->flags) {
		return 0;
	}
	
	if (this->obtem_mem_nao_configurada()) {
		return 0;
	}
	
	if (this->obtem_usa_enderecos()) {
		if (this->endereco_inicial != mem->endereco_inicial) {
			return 0;
		}
		
		if (this->tamanho != mem->tamanho) {
			return 0;
		}
		
		if (this->tamanho_palavra != mem->tamanho_palavra) {
			return 0;
		}
		
		for (i = 0; i < tamanho; i++) {
			this->le(endereco_inicial + i, &valor1);
			mem->le(endereco_inicial + i, &valor2);
			if (valor1 != valor2) {
				cout << "memorias diferentes this: " << hex << valor1 << " comparada: " <<
				 	valor2 << " endereco " << (endereco_inicial + i) << dec << endl;
				resultado = 0;
				break;
			}
		}
	}
	else {
		this->le(&valor1);
		mem->le(&valor2);
		
		if (valor1  != valor2) {
			resultado = 0;
		}
	}
	
	return resultado;
}

void Memoria::fill_value(unsigned int valor, unsigned int endereco_inicial, unsigned int tamanho) {
	unsigned int i;
	
	if (!pertence_gama_enderecos(endereco_inicial)) {
		return;
	}

	if (!pertence_gama_enderecos(endereco_inicial + tamanho - 1)) {
		return;
	}
		
	for (i = 0; i < tamanho; i++) {
		escreve(endereco_inicial + i, valor);
		if (resultado_oper != ok)
			return;
	}
}
