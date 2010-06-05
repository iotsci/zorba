/*
 * Copyright 2006-2008 The FLWOR Foundation.
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

#include "runtime/full_text/utf8_chars.h"

namespace zorba {

//
// These words come from http://snowball.tartarus.org/.
//
extern char const *const stop_words_hu[] = {
  "a",
  "abban",
  "ahhoz",
  "ahogy",
  "ahol",
  "aki",
  "akik",
  "akkor",
  "alatt",
  aacute "ltal",
  aacute "ltal" aacute "ban",
  "amely",
  "amelyek",
  "amelyekben",
  "amelyeket",
  "amelyet",
  "amelynek",
  "ami",
  "am�g",
  "amikor",
  "amit",
  "amolyan",
  "annak",
  "arra",
  "arr" oacute "l",
  aacute "t",
  "az",
  "az" eacute "rt",
  "azok",
  "azon",
  "azonban",
  "azt",
  "azt" aacute "n",
  "azut" aacute "n",
  "azzal",
  "b" aacute "r",
  "be",
  "bel" uuml "l",
  "benne",
  "cikk",
  "cikkek",
  "cikkeket",
  "csak",
  "de",
  "e",
  "ebben",
  "eddig",
  "eg" eacute "sz",
  "egy",
  "egy" eacute "b",
  "egyes",
  "egyetlen",
  "egyik",
  "egyre",
  "ehhez",
  "ekkor",
  "el",
  "el" eacute "g",
  "ellen",
  "el" otilde,
  "el" otilde "sz" ouml "r",
  "el" otilde "tt",
  "els" otilde,
  "emilyen",
  eacute "n",
  "ennek",
  eacute "ppen",
  "erre",
  eacute "s",
  "ez",
  "ezek",
  "ezen",
  "ez" eacute "rt",
  "ezt",
  "ezzel",
  "fel",
  "fel" eacute,
  "hanem",
  "hiszen",
  "hogy",
  "hogyan",
  "igen",
  "�gy",
  "ill.",
  "illetve",
  "ilyen",
  "ilyenkor",
  "ism" eacute "t",
  "ison",
  "itt",
  "j" oacute,
  "jobban",
  "j" oacute "l",
  "kell",
  "kellett",
  "keress" uuml "nk",
  "kereszt" uuml "l",
  "ki",
  "k�v" uuml "l",
  "k" ouml "z" ouml "tt",
  "k" ouml "z" uuml "l",
  "legal" aacute "bb",
  "legyen",
  "lehet",
  "lehetett",
  "lenne",
  "lenni",
  "lesz",
  "lett",
  "maga",
  "mag" aacute "t",
  "majd",
  "m" aacute "r",
  "m" aacute "s",
  "m" aacute "sik",
  "meg",
  "m" eacute "g",
  "mellett",
  "mely",
  "melyek",
  "mert",
  "mi",
  "mi" eacute "rt",
  "m�g",
  "mikor",
  "milyen",
  "minden",
  "mindenki",
  "mindent",
  "mindig",
  "mint",
  "mintha",
  "mit",
  "mivel",
  "most",
  "nagy",
  "nagyobb",
  "nagyon",
  "ne",
  "n" eacute "ha",
  "n" eacute "h" aacute "ny",
  "nekem",
  "neki",
  "n" eacute "lk" uuml "l",
  "nem",
  "nincs",
  otilde,
  otilde "k",
  otilde "ket",
  "olyan",
  ouml "ssze",
  "ott",
  "pedig",
  "persze",
  "r" aacute,
  "s",
  "saj" aacute "t",
  "sem",
  "semmi",
  "sok",
  "sokat",
  "sokkal",
  "sz" aacute "m" aacute "ra",
  "szemben",
  "szerint",
  "szinte",
  "tal" aacute "n",
  "teh" aacute "t",
  "teljes",
  "t" ouml "bb",
  "tov" aacute "bb",
  "tov" aacute "bb" aacute,
  uacute "gy",
  "ugyanis",
  uacute "j",
  uacute "jabb",
  uacute "jra",
  "ut" aacute "n",
  "ut" aacute "na",
  "utols" oacute,
  "vagy",
  "vagyis",
  "vagyok",
  "valaki",
  "valami",
  "valamint",
  "val" oacute,
  "van",
  "vannak",
  "vele",
  "vissza",
  "viszont",
  "volna",
  "volt",
  "voltak",
  "voltam",
  "voltunk",

  0
};

} // namespace zorba
/* vim:set et sw=2 ts=2: */
