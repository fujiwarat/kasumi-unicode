/*********************************************************************
 * 
 * KasumiException.hxx
 * 
 * Kasumi - a management tool for a private dictionary of anthy
 * 
 * Copyright (C) 2004-2006 Takashi Nakamoto
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301, USA.
 * 
*********************************************************************/

#ifndef __KASUMI_EXCEPTION_HXX__
#define __KASUMI_EXCEPTION_HXX__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

using namespace std;

enum _output {STDOUT, STDERR, ERR_DIALOG, WARNING_DIALOG};
typedef _output Output;

enum _disposal {KILL, QUIT, ALERT_ONLY};
typedef _disposal Disposal;

class KasumiException{
private:
  string message;
  Output output;
  Disposal disposal;
public:
  KasumiException(string aMessage, Output aOutput, Disposal aDisposal){
    message = aMessage;
    output = aOutput;
    disposal = aDisposal;
  }
  string getMessage() { return message; }
  Output getOutput() { return output; }
  Disposal getDisposal() { return disposal; }
};

void handleException(KasumiException e);

#endif
