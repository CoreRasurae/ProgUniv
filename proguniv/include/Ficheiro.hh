/***************************************************************************
 *            ficheiro.hh
 *
 *  Copyright  2007  Luís Mendes
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

#include <string>

#ifndef _FICHEIRO_HH_
#define _FICHEIRO_HH_

#include "lista.hh"

#define TAM_NOME_FICH 255

class Ficheiro {
public:
  const static std::string NOME_INDEFINIDO;
  enum estado_ficheiro {stat_ok, stat_erros_crc, stat_erros_formato};
  enum resultado_oper_ficheiro {ok, erro, apenas_leitura, ficheiro_inexistente, endereco_inexistente,
			      endereco_repetido, escrita_em_curso, escrita_nao_preparada, impossivel_criar_mem,
			      operacao_nao_suportada, ficheiro_ja_aberto, ficheiro_por_abrir, erro_formato, erro_checksum};
  enum opcoes_ficheiro {leitura, escrita, leitura_escrita};

  Ficheiro();
  virtual ~Ficheiro();

  virtual void abre(std::string nome, opcoes_ficheiro opcoes) {};
  virtual void fecha() {};
  virtual void le(unsigned int endereco, unsigned char *valor) {};
  virtual void escreve(unsigned int deslocamento, unsigned char valor) {};
  int obtem_estado(); /* Devolve estado_ficheiro */
  int obtem_resultado_oper(); /* Devolve resultado_oper_ficheiro */

  void prepara_escrita(unsigned int endereco, unsigned int tamanho);
  void finaliza_escrita();

  void coloca_endereco_inicial(unsigned int endereco);
  unsigned int obtem_endereco_inicial();
  virtual int possui_enderecamento();

  const char *obtem_nome_formato();
  unsigned int obtem_bytes_dados_lidos();
  int obtem_modo_ficheiro(); /* Devolve enum opcoes ficheiro */

protected:
  unsigned int endereco_inicial;
  estado_ficheiro estado;
  resultado_oper_ficheiro resultado;
  opcoes_ficheiro modo;
  unsigned int bytes_dados_lidos;
  char *nome_formato;
  int escrita_em_curso_flag;
  std::string nome_ficheiro;

  lista_mem buffers; //Lista buffers com conteúdo do ficheiro

  //Cada buffer tem memórias com tamanho de palavra de 8 bits,
  //num formato próprio.
  //Para formatos de ficheiro em que cada palavra tem mais de 8 bits,
  //no endereço mais baixo vem o byte menos significativo
};

#endif /* _FICHEIRO_HH_ */
