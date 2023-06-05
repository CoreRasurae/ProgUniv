/***************************************************************************
 *            lista.cc
 *
 *  Copyright  2005  Luís Mendes
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

//#include "stdafx.h"
#include <string.h>
#include "../include/lista.hh"

/* Métodos da classe contentor_mem */

contentor_mem::contentor_mem() {
  mem_elem = NULL;
  prox = NULL;
  flag_apagar = 0;
}

contentor_mem::~contentor_mem() {
  if (flag_apagar)
    delete mem_elem;
}

Memoria *contentor_mem::obtem_elem() {
  return mem_elem;
}

void contentor_mem::coloca_elem(Memoria *elem) {
  if (!flag_apagar)
    mem_elem = elem;
}

contentor_mem *contentor_mem::obtem_prox() {
  return prox;
}

void contentor_mem::coloca_prox(contentor_mem *novo_prox) {
  prox = novo_prox;
}

void contentor_mem::cria_mem() {
  mem_elem = new Memoria();
  flag_apagar = 1;
}


/* Métodos da classe lista_mem */

lista_mem::lista_mem() {
  primeiro = NULL;
  ultimo = NULL;
  num_elem = 0;
}

lista_mem::~lista_mem() {
  elimina_lista();
}

contentor_mem *lista_mem::obtem_iterador() {
  return primeiro;
}

Memoria *lista_mem::cria_insere_elem() {
  contentor_mem *contentor;

  contentor = new contentor_mem();
  if (contentor == NULL)
    return NULL;

  contentor->cria_mem();
  contentor->coloca_prox(NULL);

	if (ultimo != NULL)
  		ultimo->coloca_prox(contentor);
  	else {
	  	primeiro = contentor;
  	}

  	ultimo = contentor;
    num_elem++;

  return contentor->obtem_elem();
}

void lista_mem::insere_elem(Memoria *elem) {
  contentor_mem *contentor;

  if (elem == NULL)
    return;

  contentor = new contentor_mem();

  contentor->coloca_elem(elem);
  contentor->coloca_prox(NULL);

  if (ultimo != NULL) {
    ultimo->coloca_prox(contentor);
  }

  ultimo = contentor;

  if (primeiro == NULL) {
    primeiro = ultimo;
  }

  num_elem++;
}

void lista_mem::apaga_elem() {
  contentor_mem *contentor;

  if (primeiro == ultimo)
    ultimo = NULL;
  
  if (primeiro != NULL) {
    contentor = primeiro;
    primeiro = primeiro->obtem_prox();
  }

  delete contentor;
  
  if (num_elem > 0)
    num_elem--;
}

void lista_mem::elimina_lista() {
  while (primeiro != NULL) {
    this->apaga_elem();
  }
}

Memoria *lista_mem::procura_elem(unsigned int endereco) {
  contentor_mem *iterador;
  Memoria *resultado = NULL;

  iterador = this->obtem_iterador();

  while (iterador != NULL) {
    if ((iterador->obtem_elem())->pertence_gama_enderecos(endereco)) {
      resultado = iterador->obtem_elem();
      break;
    }
    
    iterador = iterador->obtem_prox();
  }

  return resultado;
}

Memoria *lista_mem::procura_elem(char *nome) {
  contentor_mem *iterador;
  Memoria *resultado = NULL;

  iterador = this->obtem_iterador();

  while (iterador != NULL) {
    if (strncmp( (iterador->obtem_elem())->obtem_nome(), nome, MEM_TAM_NOME) == 0) {
      resultado = iterador->obtem_elem();
      break;
    }
    
    iterador = iterador->obtem_prox();
  }

  return resultado;
}

int lista_mem::obtem_numero_elementos() {
  return num_elem;
}
