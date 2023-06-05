/****************************************************************************
 * ProgUniv is a multi-chip programmer through multi-vendor programmer hardware,
 * plugin extensible, hence the name "Programador Universal" or Universal Programmer
 * 
 *            MemoryInterceptorProxy.cc
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

#include "../include/MemoryInterceptorProxy.hh"

MemoryInterceptorProxy::MemoryInterceptorProxy(int tamanho_palavra, const char *novo_nome) : Memoria( tamanho_palavra, novo_nome ) {
}

MemoryInterceptorProxy::MemoryInterceptorProxy(unsigned int tamanho, unsigned int novo_endereco, int tamanho_palavra, const char *novo_nome) : Memoria( tamanho, novo_endereco, tamanho_palavra, novo_nome ){
}

MemoryInterceptorProxy::~MemoryInterceptorProxy() {
}

void MemoryInterceptorProxy::addMemoryListener(std::shared_ptr<Property> listener) {
	listeners.insert( listeners.begin(), listener );
}

void MemoryInterceptorProxy::escreve(unsigned int endereco, unsigned int valor) {
	Memoria::escreve( endereco, valor );
	list<shared_ptr<Property>>::iterator i = listeners.begin();
	while ( i != listeners.end() ) {
		shared_ptr<Property> listener = (*i);
		listener->doReverseUpdate();
	}
}

void MemoryInterceptorProxy::escreve(unsigned int valor) {
	Memoria::escreve( valor );
	list<shared_ptr<Property>>::iterator i = listeners.begin();
	while ( i != listeners.end() ) {
		Property &listener = *((*i).get());
		listener.doReverseUpdate();
		i++;
	}
}
