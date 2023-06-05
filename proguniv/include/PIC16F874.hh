/***************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 * 
 *            PIC16F874.hh
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


#ifndef _PIC16F874_HH_
#define _PIC16F874_HH_

#include "Memoria.hh"
#include "Programmer.hh"

class PIC16F874
{
public:
	enum chips_result {ok, erro, chip_incorrecto, erro_programacao_programa};

    static const unsigned int CP1    = 0x2020;
    static const unsigned int CP0    = 0x1010;
    static const unsigned int RESV   = 0x0800;
    static const unsigned int WRT    = 0x0200;
    static const unsigned int CPD    = 0x0100;
    static const unsigned int LVP    = 0x0080;
    static const unsigned int BODEN  = 0x0040;
    static const unsigned int _PWRTE = 0x0008;
    static const unsigned int WDTE   = 0x0004;
    static const unsigned int FOSC1  = 0x0002;
    static const unsigned int FOSC0  = 0x0001;
    static const unsigned int RC     = 0x0003;
    static const unsigned int HS     = 0x0002;
    static const unsigned int XT     = 0x0001;
    static const unsigned int LP     = 0x0000;
    static const unsigned int UNIMPL = 0x0400;
                            
	PIC16F874();
	virtual ~PIC16F874();
	
	void le_ficheiro(Ficheiro *fich);
	void escreve_ficheiro(Ficheiro *fich);
	int compara_ficheiro(Ficheiro *fich);
	void carregaConfiguracao(unsigned int config);
	bool confirmaVersao();
	void programaConfiguracao();
	void programa(bool fullEraseFirst);
	void verifica();
	void le();
	void apaga();
	void associaProgramador(Programmer *programador);
	int obtem_resultado();
	
private:
	Memoria program_mem;
	Memoria configuration_mem;
	Memoria data_mem;
	Programmer *programador;
	chips_result resultado_oper;
	bool programaVersao();
};

#endif //_PIC16F874_HH_
