/*
 * Copyright 2006-2016 zorba.io
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "stdafx.h"

#include "utf8_chars.h"

namespace zorba {

//
// These words come from http://snowball.tartarus.org/.
//
extern char const *const stop_words_pt[] = {
  "a",
  agrave,
  "ao",
  "aos",
  "aquela",
  "aquelas",
  "aquele",
  "aqueles",
  "aquilo",
  "as",
  agrave "s",
  "at" eacute,
  "com",
  "como",
  "da",
  "das",
  "de",
  "dela",
  "delas",
  "dele",
  "deles",
  "depois",
  "do",
  "dos",
  "e",
  eacute,
  "ela",
  "elas",
  "ele",
  "eles",
  "em",
  "entre",
  "era",
  "eram",
  eacute "ramos",
  "essa",
  "essas",
  "esse",
  "esses",
  "esta",
  "est" aacute,
  "estamos",
  "est" atilde "o",
  "estas",
  "estava",
  "estavam",
  "est" aacute "vamos",
  "este",
  "esteja",
  "estejam",
  "estejamos",
  "estes",
  "esteve",
  "estive",
  "estivemos",
  "estiver",
  "estivera",
  "estiveram",
  "estiv" eacute "ramos",
  "estiverem",
  "estivermos",
  "estivesse",
  "estivessem",
  "estiv" eacute "ssemos",
  "estou",
  "eu",
  "foi",
  "fomos",
  "for",
  "fora",
  "foram",
  "f" ocirc "ramos",
  "forem",
  "formos",
  "fosse",
  "fossem",
  "f" ocirc "ssemos",
  "fui",
  "h" aacute,
  "haja",
  "hajam",
  "hajamos",
  "h" atilde "o",
  "havemos",
  "havia",
  "hei",
  "houve",
  "houvemos",
  "houver",
  "houvera",
  "houver" aacute,
  "houveram",
  "houv" eacute "ramos",
  "houver" atilde "o",
  "houverei",
  "houverem",
  "houveremos",
  "houveria",
  "houveriam",
  "houver" iacute "amos",
  "houvermos",
  "houvesse",
  "houvessem",
  "houv" eacute "ssemos",
  "isso",
  "isto",
  "j" aacute,
  "lhe",
  "lhes",
  "mais",
  "mas",
  "me",
  "mesmo",
  "meu",
  "meus",
  "minha",
  "minhas",
  "muito",
  "na",
  "n" atilde "o",
  "nas",
  "nem",
  "no",
  "nos",
  "n" oacute "s",
  "nossa",
  "nossas",
  "nosso",
  "nossos",
  "num",
  "numa",
  "o",
  "os",
  "ou",
  "para",
  "pela",
  "pelas",
  "pelo",
  "pelos",
  "por",
  "qual",
  "quando",
  "que",
  "quem",
  "s" atilde "o",
  "se",
  "seja",
  "sejam",
  "sejamos",
  "sem",
  "ser",
  "ser" aacute,
  "ser" atilde "o",
  "serei",
  "seremos",
  "seria",
  "seriam",
  "ser" iacute "amos",
  "seu",
  "seus",
  "s" oacute,
  "somos",
  "sou",
  "sua",
  "suas",
  "tamb" eacute "m",
  "te",
  "tem",
  "t" eacute "m",
  "t" ecirc "m",
  "temos",
  "tenha",
  "tenham",
  "tenhamos",
  "tenho",
  "ter",
  "ter" aacute,
  "ter" atilde "o",
  "terei",
  "teremos",
  "teria",
  "teriam",
  "ter" iacute "amos",
  "teu",
  "teus",
  "teve",
  "tinha",
  "tinham",
  "t" iacute "nhamos",
  "tive",
  "tivemos",
  "tiver",
  "tivera",
  "tiveram",
  "tiv" eacute "ramos",
  "tiverem",
  "tivermos",
  "tivesse",
  "tivessem",
  "tiv" eacute "ssemos",
  "tu",
  "tua",
  "tuas",
  "um",
  "uma",
  "voc" ecirc,
  "voc" ecirc "s",
  "vos",

  0
};

} // namespace zorba
/* vim:set et sw=2 ts=2: */
