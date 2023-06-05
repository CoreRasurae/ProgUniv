/***************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 * 
 *            Intelhex16.cc
 *
 *  Copyright  2005  Lu�s Mendes
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
#include "../include/Intelhex16.hh"
#include "../include/lista.hh"
#include "../include/Ficheiro.hh"
//#include "stdafx.h"

using std::fstream;
using std::ios;

using namespace std;

Intelhex16::Intelhex16() {
  Ficheiro();
  ficheiro_aberto = 0;
  modificado = 0;
  segmento_actual = 0;
  bytes_por_linha = 16;
}

Intelhex16::~Intelhex16() {
  //~ficheiro();
}

int Intelhex16::possui_enderecamento() {
  return 1;
}

/* Abre e l� o conte�do do ficheiro para o caso de ser aberto no modo de leitura
 * ou leitura_escrita.
 * No caso de escrita cria um novo ficheiro
 */
void Intelhex16::abre(string nome, opcoes_ficheiro opcoes) {
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
    //TODO Validar e ler conte�do do ficheiro para buffers
    char c, ca[5];
    int tipo;
    unsigned int bytes;
    int endereco_high = 0;
    int endereco_low;
    int endereco;
    int checksum, checksum_lido;
    int fim = 0;
    stringstream tempss;

	tempss.setf(ios::hex, ios::basefield);
	tempss.unsetf(ios::showbase);
	
	ficheiro_hex.clear();
   	while((ficheiro_hex >> c) && (!fim)) {
   		checksum = 0;
   		bytes = 0;
   		endereco_low = 0;

   		if (c != ':') {
   			resultado = erro_formato;
   			return;
   		}

   		if (!(ficheiro_hex >> setw(3) >> ca)) {
   			resultado = erro_formato;
   			return;   			
   		}
   	
   		tempss.clear();
   		tempss << ca;
   		tempss >> bytes;
   		
   		checksum += bytes;
   		bytes <<= 1;
   		
   		if (!(ficheiro_hex >> setw(5) >> ca)) {
   			resultado = erro_formato;
   			return;	
   		}

	
		tempss.clear();
		tempss << ca;
		tempss >> endereco_low;
		
		checksum += (endereco_low & 0xff) + (endereco_low >> 8);

   		if (!(ficheiro_hex >> setw(3) >> ca)) {
   			resultado = erro_formato;
   			return;   			
   		}
   				
   		tempss.clear();
   		tempss << ca;
   		tempss >> tipo;
   			
   		checksum += tipo;
   		
   		if (tipo == 0) {
   			//Ler dados
   			int i;
   			unsigned int palavra;
   			Memoria *mem = NULL;
   			
   			if (bytes > 0) {
	   			endereco = endereco_high + endereco_low;
	   			
	   			mem = buffers.cria_insere_elem();
	   			if (mem == NULL) {
	   				resultado = impossivel_criar_mem;
	   				return;
	   			}
	   			
	   			mem->configura(bytes, (endereco << 1), 8, NULL);
	   			for(i = 0; i < bytes; i+=2) {
			   		if (!(ficheiro_hex >> setw(5) >> ca)) {
	   					resultado = erro_formato;
	   					return;   			
	   				}
	   				
					tempss.clear();
	   				tempss << ca;
	   				tempss >> palavra;
	   				
	   				
	   				mem->escreve((endereco << 1) + i, (palavra & 0xff));
	   				mem->escreve((endereco << 1) + i + 1, (palavra >> 8));
	   				checksum += (palavra & 0xff);
	   				checksum += (palavra >> 8);
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
   			//Segmento
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
			
   			endereco_high = temp << 4;
   			checksum += ((temp & 0xff) + temp) >> 8;
   		}
   		else if (tipo == 4) {
   			//Endere�o linear 32bits
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
     		     		
   			endereco_high = temp << 16;
   			checksum += ((temp & 0xff) + temp) >> 8;
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
   		checksum &= 0xff;
   		
   		if (checksum != checksum_lido) {
   			cout << "Checksum calculado: " << checksum << " Checksum lido: " << checksum_lido << endl;
   			resultado = erro_checksum;
   			return;
   		}
   		
   		ficheiro_hex >> ws;
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
void Intelhex16::fecha() {
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
 * L� do buffer () carregado a partir do ficheiro, o valor contido
 * no endereco indicado
 *
 * Erros:
 * endereco_inexistente - leitura de uma posi��o que n�o existe no buffer
 */
void Intelhex16::le(unsigned int endereco, unsigned char *valor) {
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
void Intelhex16::escreve(unsigned int endereco, unsigned char valor) {
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

/*
 * M�todo escreve_buffer - uso interno da classe e descendentes
 * Descarrega o conte�do de um buffer (mem�ria) de ficheiro para ficheiro
 */
void Intelhex16::escreve_buffer(Memoria *buf) {
  unsigned int endereco_inicio;
  unsigned int tamanho;
  int i = 0;
  unsigned int offset;
  unsigned int checksum;
  int bytes_a_escrever;

  if (buf == NULL)
    assert(buf != NULL);
 

  endereco_inicio = buf->obtem_endereco_inicial();
  endereco_inicio >>= 1;
  if (buf->obtem_resultado_oper() == Memoria::memoria_nao_configurada) {
	cout << "erro: memoria n�o configurada";
	//Incluir mecanismo de tratamentos de erros mais sofisticado
	resultado = erro;
	return;
  }
  else if (buf->obtem_resultado_oper() == Memoria::operacao_nao_suportada) {
	//N�o existe endere�amento na mem�ria
	endereco_inicio = 0;
	segmento_actual = 0;
  }
  else {
  	segmento_actual = (endereco_inicio >> 16) & 0xffff;  
  }

  tamanho = buf->obtem_tamanho();
  
  //Activar escrita hexadecimal de inteiros, sem base (0x) e em mai�sculas
  ficheiro_hex.setf(ios::hex, ios::basefield);
  ficheiro_hex.unsetf(ios::showbase);
  ficheiro_hex.setf(ios::uppercase);
  
  if (segmento_actual != 0) {
	  //Proceder � escrita do ficheiro
	  ficheiro_hex << ":02000004" << setw(4) << setfill('0') << segmento_actual;
	  ficheiro_hex << setw(2) << setfill('0') << 
  		((1 + ~((6 + (segmento_actual >> 8) + segmento_actual) % 256)) & 0xff);
	  ficheiro_hex << endl;
  }
  
  offset = 0;
  while (offset < tamanho) {
	unsigned int palavra;
	unsigned int byte;
	  
	if ((offset + bytes_por_linha) > tamanho)
		bytes_a_escrever = tamanho - offset;
	else
		bytes_a_escrever = bytes_por_linha;
	
	//TODO Processar endere�o linear (no caso de troca a meio da escrita)
	
	ficheiro_hex << ":" << setw(2) << setfill('0') << (bytes_a_escrever >> 1);
	ficheiro_hex << setw(4) << setfill('0') << 
		((endereco_inicio + (offset >> 1)) & 0xffff) << "00";


	checksum = (bytes_a_escrever >> 1) + ((endereco_inicio + (offset >> 1)) >> 8) +
		((endereco_inicio + (offset >> 1)) & 0xff);
  	for (i=0; i < bytes_a_escrever; i+=2) {
  		palavra = 0;
		buf->le((endereco_inicio << 1) + offset + i, &byte);
		if (buf->obtem_resultado_oper() != Memoria::ok) {
			resultado = erro;
			//TODO Eliminar ficheiro
			return;
		}
		palavra |= byte; 
		checksum += byte;

		buf->le((endereco_inicio << 1) + offset + i + 1, &byte);
		if (buf->obtem_resultado_oper() != Memoria::ok) {
			resultado = erro;
			//TODO Eliminar ficheiro
			return;
		}
		checksum += byte;	
		palavra |= (byte << 8);
		
		ficheiro_hex << setw(4) << setfill('0') << palavra;
  	}
  	
  	//Actualizar offset com bytes escritos
	offset += bytes_a_escrever;	
  	
	//Calcular checksum da linha
	ficheiro_hex << setw(2) << setfill('0') << (((~(checksum % 256)) + 1 ) & 0xff);
	ficheiro_hex << endl;
  }
    
  resultado = ok;
}
