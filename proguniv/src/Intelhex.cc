/***************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 * 
 *            Intelhex.cc
 *
 *  Copyright  2005  Luís Mendes
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


#include <string.h>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "../include/Intelhex.hh"
#include "../include/lista.hh"
#include "../include/Memoria.hh"
//#include "stdafx.h"

using std::fstream;
using std::ios;

using namespace std;

Intelhex::Intelhex() {
  Ficheiro();
  ficheiro_aberto = 0;
  modificado = 0;
  segmento_actual = 0;
  endereco_high_actual = 0;
  bytes_por_linha = 32;
}

Intelhex::~Intelhex() {
  //~ficheiro();
}

int Intelhex::possui_enderecamento() {
  return 1;
}

/* Abre e l� o conte�do do ficheiro para o caso de ser aberto no modo de leitura
 * ou leitura_escrita.
 * No caso de escrita cria um novo ficheiro
 */
void Intelhex::abre(string nome, opcoes_ficheiro opcoes) {
  if (ficheiro_aberto == 1) {
    resultado = ficheiro_ja_aberto;
    return;
  }
   
  nome_ficheiro = nome;

  if (opcoes == leitura) {
  	modo = leitura;
    ficheiro_hex.open(nome_ficheiro, ios::in);
  }
  else if (opcoes == escrita) {
  	modo = escrita;
    ficheiro_hex.open(nome_ficheiro, ios::out);
  }
  else { //leitura_escrita
  	modo = leitura_escrita;
    ficheiro_hex.open(nome_ficheiro, ios::in | ios::out);
  }


  if (!ficheiro_hex) {
    resultado = ficheiro_inexistente;
    return;
  }

  ficheiro_aberto = 1;

  if (modo == leitura || modo == leitura_escrita) {
    //TODO Validar e ler conteúdo do ficheiro para buffers
    char c, ca[5];
    int tipo;
    int bytes;
    int endereco_high = 0;
    int endereco_low;
    int endereco;
    int checksum, checksum_lido;
    int fim = 0;
    int linha = 1;
    stringstream tempss;

	tempss.setf(ios::hex, ios::basefield);
	tempss.unsetf(ios::showbase);
	
	ficheiro_hex.clear();
   	while((ficheiro_hex >> c) && (!fim)) {
   		checksum = 0;
   		bytes = 0;
   		endereco_low = 0;

   		if ( c != ':' ) {
   			resultado = erro_formato;
   			return;
   		}

   		if ( !(ficheiro_hex >> setw(3) >> ca) ) {
   			resultado = erro_formato;
   			return;   			
   		}
   	
   		tempss.clear();
   		tempss << ca;
   		tempss >> bytes;
   		
   		checksum += bytes;
   		
   		if ( !(ficheiro_hex >> setw(5) >> ca) ) {
   			resultado = erro_formato;
   			return;	
   		}

	
		tempss.clear();
		tempss << ca;
		tempss >> endereco_low;
		
		checksum += (endereco_low & 0xff) + (endereco_low >> 8);

   		if ( !(ficheiro_hex >> setw(3) >> ca) ) {
   			resultado = erro_formato;
   			return;   			
   		}
   				
   		tempss.clear();
   		tempss << ca;
   		tempss >> tipo;
   			
   		checksum += tipo;
   		
   		if ( tipo == 0 ) {
   			//Ler dados
   			int i;
   			unsigned int byte;
   			Memoria *mem = NULL;
   			
   			if ( bytes > 0 ) {
	   			endereco = (endereco_high_actual << 16) + endereco_low;
	   			
	   			mem = buffers.cria_insere_elem();
	   			if (mem == NULL) {
	   				resultado = impossivel_criar_mem;
	   				return;
	   			}
	   			
	   			mem->configura(bytes, endereco, 8, NULL);
	   			for(i = 0; i < bytes; i++) {
			   		if (!(ficheiro_hex >> setw(3) >> ca)) {
	   					resultado = erro_formato;
	   					return;   			
	   				}
	   				
					tempss.clear();
	   				tempss << ca;
	   				tempss >> byte;
	   				
	   				
	   				mem->escreve(endereco + i, byte);
	   				checksum += byte;
	   			}
   			}
   		}
   		else if (tipo == 1) {
   			//Fim
   			if (bytes != 0) {
       			resultado = erro_formato;
	   			return; 			
   			}
   			
   			fim = 1;
   		}
   		else if (tipo == 2) {
   			//Extended Segment Address Record (CS register offset in 80x86 architecture)
   			//following data is at this segment, until new extended segment address override.
   			int temp;
   			
   			if (bytes != 2) {
       			resultado = erro_formato;
	   			return; 			
   			}   				
   			
   			if (!(ficheiro_hex >> setw(5) >> ca)) {
       			resultado = erro_formato;
	   			return; 			
     		}

			tempss.clear();
			tempss << ca;
			tempss >> temp;   			
			
   			segmento_actual = temp << 4;
   			checksum += ((temp & 0xff) + temp) >> 8;
   		}
   		else if (tipo == 4) {
   			//Extended Linear Address Record
   			//Endereço linear 32bits
   			int temp;
   			
   			if (bytes != 2) {
       			resultado = erro_formato;
	   			return; 			
   			}   				
   			
     		if (!(ficheiro_hex >> setw(5) >> ca)) {
       			resultado = erro_formato;
	   			return; 			
     		}
     		
     		tempss.clear();
     		tempss << ca;
     		tempss >> temp;
   			endereco_high_actual = temp;
   			checksum += ((temp >> 8) & 0x0ff) + (temp & 0x0ff);
   		}
   		else {
   			resultado = erro_formato;
   			return; 			   			
   		}
   		
   		if (!(ficheiro_hex >> setw(3) >> ca)) {
   			resultado = erro_formato;
   			return; 			
   		}
   		
   		tempss.clear();
   		tempss << ca;
   		tempss >> checksum_lido;

   		checksum = ~checksum;
   		checksum++;
   		checksum &= 0x0ff;
   		
   		if (checksum != checksum_lido) {
   			cout << "ERRO: Linha: " << linha << " - Checksum calculado: " << checksum << " Checksum lido: " << checksum_lido << endl;
   			resultado = erro_checksum;
   			return;
   		}
   		
   		ficheiro_hex >> ws;
   		linha++;
   	}
  }
 
  ficheiro_hex.clear(); 
  resultado = ok;
}

/* Fecha o ficheiro
 * Implica a escrita para ficheiro, no caso do buffer ter sido modificado
 * Todos os buffers existentes em mem�ria s�o descaregados por ordem crescente
 * de endere�os
 */
void Intelhex::fecha() {
  contentor_mem *iterador;
  Memoria *mem_elem, *mem_prox_escrever, *mem_escrita;

  if (modificado == 1 && (modo == escrita || modo == leitura_escrita)) {
    //Obter buffers para escrever por ordem crescente de endere�os
    mem_prox_escrever = NULL;
    mem_escrita = NULL;

    for (int i = 0; i < buffers.obtem_numero_elementos(); i++) {
      for (iterador = buffers.obtem_iterador();iterador != NULL;
  		  iterador = iterador->obtem_prox()) 
	  {
		mem_elem = iterador->obtem_elem();
	
		if (mem_prox_escrever == NULL && mem_escrita == NULL)
		  mem_prox_escrever = mem_elem;
		else if (mem_prox_escrever == NULL) {
		  if (mem_escrita->obtem_endereco_inicial() < mem_elem->obtem_endereco_inicial()) {
			mem_prox_escrever = mem_elem;
		  }
		}
		else {
		  if (mem_escrita == NULL) {
			if (mem_elem->obtem_endereco_inicial() < mem_prox_escrever->obtem_endereco_inicial()) {
			  mem_prox_escrever = mem_elem;
			}
		  }
		  else {
			if (mem_escrita->obtem_endereco_inicial() < mem_elem->obtem_endereco_inicial()) {
			  if (mem_elem->obtem_endereco_inicial() < mem_prox_escrever->obtem_endereco_inicial()) {
			mem_prox_escrever = mem_elem;
			  }
			}
		  }
		}
	  }
	  mem_escrita = mem_prox_escrever;
	  mem_prox_escrever = NULL;
	
	  //Escrever buffer
	  this->escreve_buffer(mem_escrita);

    }
	
    ficheiro_hex << ":00000001FF" << endl;
    ficheiro_hex << ends;
    ficheiro_hex.flush();
  }

  ficheiro_hex.close();
  ficheiro_aberto = 0;
}

/*
 * L� do buffer (Memoria) carregado a partir do ficheiro, o valor contido
 * no endereco indicado
 *
 * Erros:
 * endereco_inexistente - leitura de uma posi��o que n�o existe no buffer
 */
void Intelhex::le(unsigned int endereco, unsigned char *valor) {
  unsigned int temp;
  Memoria *buffer;

  if (ficheiro_aberto == 0) {
    resultado = ficheiro_por_abrir;
    return;
  }

  buffer = buffers.procura_elem(endereco);
  if (buffer == NULL) {
    resultado = endereco_inexistente;
    return;
  }
  
  buffer->le(endereco, &temp);
  *valor = (unsigned char)(temp & 0xff);

  resultado = ok;
}

/* Escreve no buffer de ficheiro na posi��o endere�o o byte valor
 *
 */
void Intelhex::escreve(unsigned int endereco, unsigned char valor) {
  unsigned int dados;
  Memoria *buffer;
  
  if (ficheiro_aberto == 0) {
    resultado = ficheiro_por_abrir;
    return;
  }

  if (escrita_em_curso == 0) {
    resultado = escrita_nao_preparada;
    return;
  }   

  buffer = buffers.procura_elem(endereco);
  if (buffer == NULL) {
    resultado = endereco_inexistente;
    return;
  }
  
  buffer->le(endereco, (unsigned int *)&dados);
  if (dados != valor)
    modificado = 1;

  buffer->escreve(endereco, valor);

  resultado = ok;
}

void Intelhex::criaEntradaDeExtendedLinearAddress() {
   //Start by writting initial Extended Linear Address Record with initial 32-bit address 2 higher address bytes.
   ficheiro_hex << ":02000004" << setw(4) << setfill('0') << (endereco_high_actual & 0x0ffff);
   ficheiro_hex << setw(2) << setfill('0') << 
   	((1 + ~((6 + ((endereco_high_actual >> 8) & 0x0ff)+ (endereco_high_actual & 0x0ff)) % 256)) & 0x0ff);
   ficheiro_hex << endl;
}

/*
 * M�todo escreve_buffer - uso interno da classe e descendentes
 * Descarrega o conte�do de um buffer (mem�ria) de ficheiro para ficheiro
 */
void Intelhex::escreve_buffer(Memoria *buf) {
  unsigned int endereco_inicio;
  unsigned int tamanho;
  int i = 0;
  unsigned int offset;
  unsigned int checksum;
  int bytes_a_escrever;

  if (buf == NULL)
    assert(buf != NULL);
 

  endereco_inicio = buf->obtem_endereco_inicial();
  if (buf->obtem_resultado_oper() == Memoria::memoria_nao_configurada) {
	cout << "erro: memoria não configurada";
	//Incluir mecanismo de tratamentos de erros mais sofisticado
	resultado = erro;
	return;
  }
  else if (buf->obtem_resultado_oper() == Memoria::operacao_nao_suportada) {
	//Não existe endereçamento na memória
	endereco_inicio = 0;
	endereco_high_actual = 0;
  }
  else {
  	endereco_high_actual = (endereco_inicio >> 16) & 0xffff;  
  }

  tamanho = buf->obtem_tamanho();
  
  //Activar escrita hexadecimal de inteiros, sem base (0x) e em maiúsculas
  ficheiro_hex.setf(ios::hex, ios::basefield);
  ficheiro_hex.unsetf(ios::showbase);
  ficheiro_hex.setf(ios::uppercase);
  
  //Proceder a escrita do ficheiro, começando pelo endereço inicial
  criaEntradaDeExtendedLinearAddress();
  
  offset = 0;
  while (offset < tamanho) {
	unsigned int palavra;
	  
	if ((offset + bytes_por_linha) > tamanho)
		bytes_a_escrever = tamanho - offset;
	else
		bytes_a_escrever = bytes_por_linha;
	
	if (((offset + endereco_inicio) >> 16) != ((bytes_a_escrever + offset + endereco_inicio) >> 16)) {
		//Bytes a escrever ultpassam fronteira do 64kB a meio o buffer, o que não se pode permitir
		for (int i = 0; i < bytes_a_escrever; i++) {
			if ( ((offset + i + endereco_inicio) >> 16) != ((offset + endereco_inicio) >> 16) ) {
			        bytes_a_escrever = i; 
				break; 
			}			
		}
	}

	if (((offset + endereco_inicio) >> 16) != endereco_high_actual) {
	   endereco_high_actual = (offset + endereco_inicio) >> 16;
	   criaEntradaDeExtendedLinearAddress();
	}
	
	//TODO Processar endereço linear (no caso de troca a meio da escrita)
	ficheiro_hex << ":" << setw(2) << setfill('0') << bytes_a_escrever;
	ficheiro_hex << setw(4) << setfill('0') << 
		((endereco_inicio + offset) & 0xffff) << "00";


	
	checksum = bytes_a_escrever + ((endereco_inicio + offset) >> 8) +
		((endereco_inicio + offset) & 0xff);
  	for (i=0; i < bytes_a_escrever; i++) {
  		palavra = 0;
		buf->le(endereco_inicio + offset + i, &palavra);
		if (buf->obtem_resultado_oper() != Memoria::ok) {
			resultado = erro;
			//TODO Eliminar ficheiro
			return;
		}
	
		ficheiro_hex << setw(2) << setfill('0') << palavra;
		checksum += palavra;
  	}
  	//Actualizar offset com bytes escritos
	offset += bytes_a_escrever;	
  	
	//Calcular checksum da linha
	ficheiro_hex << setw(2) << setfill('0') << (((~(checksum % 256)) + 1 ) & 0xff);
	ficheiro_hex << endl;
  }
    
  resultado = ok;
}
