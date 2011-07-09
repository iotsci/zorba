xquery version "3.0";
(: Higher Order Functions :)
(: atomize a function item implicitly :)
(: Author - Michael Kay, Saxonica :)

declare namespace o = "http://www.zorba-xquery.com/options";
declare option o:enable-feature "hof";

declare function local:f($x as xs:integer) as xs:integer {
  $x + 1
};

number(local:f#1)
