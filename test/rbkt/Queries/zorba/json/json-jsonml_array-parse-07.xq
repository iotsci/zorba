import module namespace jx = "http://zorba.io/modules/json-xml";

let $json := '{ "args" : [ "<div><span>foo:</span> parse</div>" ] }'
let $options := { "json-format" : "JsonML-array" }
return jx:json-string-to-xml( $json, $options )

(: vim:set et sw=2 ts=2: :)
